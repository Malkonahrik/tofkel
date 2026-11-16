/**
 * @file directory_data.h
 * @brief Basic directory scanner
 * @copyright Malkonahrik. GPL3, 2026
 **/

#ifndef __DIRECTORY_DATA_H__
#define __DIRECTORY_DATA_H__

#include <stddef.h>

#include "scanner/data.h"

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

/**
 * @brief Get the directory's data
 * @param directory_data a pointer of a @p directory_data_s instance
 * @pre @p directory_data must not be @p NULL
 * @return a list of @p data_s pointer or @p NULL if @p directory_data is @p NULL
 **/
data_s** directory_get_data(directory_data_s* directory_data);

/**
 * @brief Get the directory's number of data
 * @param directory_data a pointer of a @p directory_data_s instance
 * @pre @p directory_data must not be @p NULL
 * @return 0 if @p directory_data is NULL otherwise how many data it has
 **/
size_t directory_get_nb_data(directory_data_s* directory_data);

/**
 * @brief Get the directory's children
 * @param directory_data a pointer of a @p directory_data_s instance
 * @pre @p directory_data must not be @p NULL
 * @return a list of @p directory_data pointer or @p NULL if @p directory_data is @p NULL
 **/
directory_data_s** directory_get_children(directory_data_s* directory_data);

/**
 * @brief Get the directory's number of children
 * @param directory_data a pointer of a @p directory_data_s instance
 * @pre @p directory_data must not be @p NULL
 * @return 0 if @p directory_data is NULL otherwise how many children it has
 **/
size_t directory_get_nb_children(directory_data_s* directory_data);

/**
 * @brief Scan a directory and its children depending of the specified @p depth
 * @details A @p depth of 0 give only the directory content. A @p depth > 0 scans its children.
 * @details A data that is a directory will be scanned and added as a children if the @p depth
 * @details requires it
 * @param path the path to the directory
 * @pre @p path must be a valid linux path
 * @param depth the depth of the scan
 * @pre @p depth must be included between 0 and 5
 * @return @p NULL if one precondition is not met otherwise a @p directory_data representing the
 * directory and its children.
 **/
directory_data_s* scan_directory(const char* path, size_t depth);

#endif
