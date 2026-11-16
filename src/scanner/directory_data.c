#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "scanner/data.h"
#include "scanner/directory_data.h"

/******************** directory_data ********************/

#define INITIAL_CAPACITY 10

struct directory_data_s {
    data_s** data;
    size_t nb_data;
    size_t capacity;
};

directory_data_s* directory_data_new_empty(void)
{
    data_s** data = calloc(INITIAL_CAPACITY, sizeof(*data));
    directory_data_s* directory_data = malloc(sizeof(directory_data_s));

    if (data == NULL || directory_data == NULL) {
        free(data);
        free(directory_data);
        return NULL;
    }

    directory_data->data = data;
    directory_data->nb_data = 0;
    directory_data->capacity = INITIAL_CAPACITY;

    return directory_data;
}

void directory_data_destroy(directory_data_s* directory_data)
{
    if (directory_data == NULL)
        return;

    for (size_t i = 0; i < directory_data->nb_data; i++)
        data_destroy(directory_data->data[i]);

    free(directory_data->data);
    free(directory_data);
}

bool directory_data_add(directory_data_s* directory_data, data_s* data)
{
    if (directory_data->nb_data >= directory_data->capacity) {
        size_t new_capacity = directory_data->capacity + INITIAL_CAPACITY;
        data_s** tmp
            = reallocarray(directory_data->data, new_capacity, sizeof(*directory_data->data));

        if (tmp == NULL)
            return false;

        directory_data->data = tmp;
        directory_data->capacity = new_capacity;
    }

    directory_data->data[directory_data->nb_data++] = data;
    return true;
}

/******************** utility ********************/

static const char* path_fix(const char* path)
{
    size_t len = strlen(path);
    char* new_path = strdup(path);
    while (len > 1 && path[len - 1] == '/')
        new_path[--len] = '\0';
    return new_path;
}

directory_data_s* scan_directory(const char* path)
{
    const char* new_path = path_fix(path);
    directory_data_s* directory_data = directory_data_new_empty();
    if (directory_data == NULL) {
        perror("directory_data_new_empty");
        return NULL;
    }

    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        directory_data_destroy(directory_data);
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

        if (asprintf(&full_path, "%s/%s", new_path, entry->d_name) == -1)
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

        if (!directory_data_add(directory_data, data)) {
            data_destroy(data);
            perror("directory_data_add");
        }
    }

    closedir(dir);
    return directory_data;
}
