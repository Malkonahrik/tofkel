/**
 * @file directory_data.h
 * @brief Basic directory scanner
 * @copyright Malkonahrik. GPL3, 2026
 **/

#ifndef __DIRECTORY_DATA_H__
#define __DIRECTORY_DATA_H__

#include <stddef.h>

/**
 * @brief A structure representing a directory's data
 **/
typedef struct directory_data_s directory_data_s;

/**
 * @brief Create a @p directory_data_s instance with no data
 * @details All of its data is @p NULL
 * @ref data_new()
 **/
directory_data_s* directory_data_new_empty(void);

/**
 * @brief Delete the directory's data and frees the allocated memory
 * @param directory_data a pointer of a @p directory_data_s instance
 **/
void directory_data_destroy(directory_data_s* directory_data);

directory_data_s* scan_directory(const char* path);

#endif
