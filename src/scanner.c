#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/********************** data **********************/

// REG as in Regular
typedef enum {
    PROTON = 0,
    WINDOW_EXEC = 1,
    LINUX_EXEC = 2,
    REGFILE = 3,
    REGDIRECTORY = 4,
    OTHER = 5
} data_t;

typedef struct {
    char* name;
    char* path;
    data_t type;
} data_s;

void data_destroy(data_s* data)
{
    if (data == NULL)
        return;
    free(data->path);
    data->path = NULL;
    free(data->name);
    data->name = NULL;
}

data_s* data_new(const char* name, const char* path, data_t type)
{
    data_s* data = malloc(sizeof(*data));
    if (data == NULL)
        return NULL;

    data->name = strdup(name);
    data->path = strdup(path);

    if (data->name == NULL || data->path == NULL) {
        data_destroy(data);
        free(data);
        return NULL;
    }

    data->type = type;
    return data;
}

/********************** scanned_data **********************/

#define INITIAL_CAPACITY 10

typedef struct {
    data_s** data;
    size_t nb_data;
    size_t capacity;
} scanned_data_s;

scanned_data_s* scanned_data_new_empty()
{
    data_s** data = calloc(INITIAL_CAPACITY, sizeof(*data));
    scanned_data_s* scanned_data = malloc(sizeof(scanned_data_s));

    if (data == NULL || scanned_data == NULL) {
        free(data);
        free(scanned_data);
        return NULL;
    }

    scanned_data->data = data;
    scanned_data->nb_data = 0;
    scanned_data->capacity = INITIAL_CAPACITY;

    return scanned_data;
}

void scanned_data_destroy(scanned_data_s* scanned_data)
{
    if (scanned_data == NULL)
        return;

    for (size_t i = 0; i < scanned_data->nb_data; i++) {
        data_destroy(scanned_data->data[i]);
        free(scanned_data->data[i]);
    }

    free(scanned_data->data);
    free(scanned_data);
}

bool scanned_data_add(scanned_data_s* scanned_data, data_s* data)
{
    if (scanned_data->nb_data >= scanned_data->capacity) {
        size_t new_capacity = scanned_data->capacity + INITIAL_CAPACITY;
        data_s** tmp = reallocarray(scanned_data->data, new_capacity, sizeof(*scanned_data->data));

        if (tmp == NULL)
            return false;

        scanned_data->data = tmp;
        scanned_data->capacity = new_capacity;
    }

    scanned_data->data[scanned_data->nb_data++] = data;
    return true;
}

/********************** utility **********************/

static scanned_data_s* scan_directory(const char* path)
{
    scanned_data_s* scanned_data = scanned_data_new_empty();
    if (scanned_data == NULL) {
        perror("scanned_data_new_empty");
        return NULL;
    }

    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        scanned_data_destroy(scanned_data);
        return NULL;
    }

    struct dirent* entry;
    struct stat info;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.'
            && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
            continue;

        if (fstatat(dirfd(dir), entry->d_name, &info, 0) == -1) {
            perror("fstatat");
            continue;
        }

        char* full_path = NULL;

        if (asprintf(&full_path, "%s/%s", path, entry->d_name) == -1)
            continue;

        data_t type = OTHER;

        if (S_ISREG(info.st_mode))
            type = REGFILE;

        else if (S_ISDIR(info.st_mode))
            type = REGDIRECTORY;

        data_s* data = data_new(entry->d_name, full_path, type);
        free(full_path);
        if (data == NULL) {
            perror("data_new");
            continue;
        }

        if (!scanned_data_add(scanned_data, data)) {
            data_destroy(data);
            free(data);
            perror("scanned_data_add");
        }
    }

    closedir(dir);
    return scanned_data;
}
