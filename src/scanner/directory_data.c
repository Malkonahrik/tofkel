#include <dirent.h>
#include <stddef.h>
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
    size_t data_capacity;

    directory_data_s** children;
    size_t nb_children;
    size_t children_capacity;
};

directory_data_s* directory_data_new_empty(void)
{
    data_s** data = calloc(INITIAL_CAPACITY, sizeof(*data));
    directory_data_s** children = calloc(INITIAL_CAPACITY, sizeof(*children));
    directory_data_s* directory_data = malloc(sizeof(*directory_data));

    if (data == NULL || directory_data == NULL || children == NULL) {
        free(data);
        free(directory_data);
        free(children);
        return NULL;
    }

    directory_data->data = data;
    directory_data->nb_data = 0;
    directory_data->data_capacity = INITIAL_CAPACITY;

    directory_data->children = children;
    directory_data->nb_children = 0;
    directory_data->children_capacity = INITIAL_CAPACITY;

    return directory_data;
}

void directory_data_destroy(directory_data_s* directory_data)
{
    if (directory_data == NULL)
        return;

    for (size_t i = 0; i < directory_data->nb_data; i++)
        data_destroy(directory_data->data[i]);

    for (size_t i = 0; i < directory_data->nb_children; i++) {
        directory_data_destroy(directory_data->children[i]);
    }

    free(directory_data->data);
    free(directory_data->children);
    free(directory_data);
}

static bool directory_data_add(directory_data_s* directory_data, data_s* data)
{
    if (data == NULL) {
        return true;
    }

    if (directory_data->nb_data >= directory_data->data_capacity) {
        size_t new_data_capacity = directory_data->data_capacity + INITIAL_CAPACITY;
        data_s** tmp
            = reallocarray(directory_data->data, new_data_capacity, sizeof(*directory_data->data));

        if (tmp == NULL)
            return false;

        directory_data->data = tmp;
        directory_data->data_capacity = new_data_capacity;
    }

    directory_data->data[directory_data->nb_data++] = data;
    return true;
}

static bool directory_children_add(directory_data_s* directory_data, directory_data_s* children)
{
    if (children == NULL) {
        return true;
    }

    if (directory_data->nb_children >= directory_data->children_capacity) {
        size_t new_children_capacity = directory_data->children_capacity + INITIAL_CAPACITY;
        directory_data_s** tmp = reallocarray(
            directory_data->children, new_children_capacity, sizeof(*directory_data->children));

        if (tmp == NULL)
            return false;

        directory_data->children = tmp;
        directory_data->children_capacity = new_children_capacity;
    }

    directory_data->children[directory_data->nb_children++] = children;
    return true;
}

data_s** directory_get_data(directory_data_s* directory_data)
{
    if (directory_data == NULL)
        return NULL;
    return directory_data->data;
}

size_t directory_get_nb_data(directory_data_s* directory_data)
{
    if (directory_data == NULL)
        return 0;
    return directory_data->nb_data;
}

directory_data_s** directory_get_children(directory_data_s* directory_data)
{
    if (directory_data == NULL)
        return NULL;
    return directory_data->children;
}

size_t directory_get_nb_children(directory_data_s* directory_data)
{
    if (directory_data == NULL)
        return 0;
    return directory_data->nb_children;
}

/******************** utility ********************/

static char* path_fix(const char* path)
{
    char* new_path = strdup(path);
    if (new_path == NULL)
        return NULL;

    size_t len = strlen(new_path);

    while (len > 1 && new_path[len - 1] == '/')
        new_path[--len] = '\0';

    return new_path;
}

static directory_data_s* scan_one_directory(const char* path)
{
    char* new_path = path_fix(path);
    directory_data_s* directory_data = directory_data_new_empty();
    if (directory_data == NULL) {
        perror("directory_data_new_empty");
        free(new_path);
        return NULL;
    }

    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        directory_data_destroy(directory_data);
        free(new_path);
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

    free(new_path);
    closedir(dir);
    return directory_data;
}

directory_data_s* scan_directory(const char* path, size_t depth)
{
    directory_data_s* directory_root = scan_one_directory(path);
    if (depth <= 0 || directory_root == NULL)
        return directory_root;

    for (size_t i = 0; i < directory_root->nb_data; i++) {
        if (data_get_type(directory_root->data[i]) == REGDIRECTORY) {
            directory_children_add(
                directory_root, scan_directory(data_get_path(directory_root->data[i]), depth - 1));
        }
    }

    return directory_root;
}
