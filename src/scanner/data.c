#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "scanner/data.h"

struct data_s {
    char* name;
    char* path;
    data_t type;
};

static bool check_name(const char* name)
{
    if (name == NULL || name[0] == '\0')
        return false;

    for (size_t i = 0; name[i] != '\0'; i++) {
        if (name[i] == '/')
            return false;
    }

    return true;
}

static bool check_path(const char* path) { return path != NULL && path[0] != '\0'; }

static bool check_type(const data_t type) { return type >= 0 && type <= 5; }

data_s* data_new(const char* name, const char* path, data_t type)
{
    if (check_name(name) == false || check_path(path) == false || check_type(type) == false)
        return NULL;

    data_s* data = malloc(sizeof(*data));
    if (data == NULL)
        return NULL;

    data->name = strdup(name);
    data->path = strdup(path);

    if (data->name == NULL || data->path == NULL) {
        data_destroy(data);
        return NULL;
    }

    data->type = type;
    return data;
}

const char* data_get_name(const data_s* data)
{
    if (data == NULL)
        return NULL;
    return data->name;
}

const char* data_get_path(const data_s* data)
{
    if (data == NULL)
        return NULL;
    return data->path;
}

data_t data_get_type(const data_s* data)
{
    if (data == NULL)
        return INVALID;
    return data->type;
}

bool data_set_type(data_s* data, data_t type)
{
    if (check_type(type)) {
        data->type = type;
        return true;
    }
    return false;
}

void data_destroy(data_s* data)
{
    if (data == NULL)
        return;
    free(data->path);
    data->path = NULL;
    free(data->name);
    data->name = NULL;
    free(data);
}
