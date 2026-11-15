/**
 * @file data.h
 * @brief Represents a filesystem entry discovered by the scanner.
 * @copyright Malkonahrik. GPL3, 2026
 **/

#ifndef __DATA_H__
#define __DATA_H__

#include <stdbool.h>

/**
 * @brief Different type of file
 * @details REG stands for Regular
 **/
typedef enum {
    PROTON = 0,
    WINDOW_EXEC = 1,
    LINUX_EXEC = 2,
    REGFILE = 3,
    REGDIRECTORY = 4,
    OTHER = 5,
} data_t;

/**
 * @brief The data structure
 **/
typedef struct data_s data_s;

/**
 * @brief Create a @p data_s instance
 * @param name the name of the file
 * @param path the path of the file
 * @param type the type of the file
 * @pre @p name must not be @p NULL nor EMPTY (@p "\0") and must not contain the character @p '/'
 * @pre @p path must not be @p NULL nor EMPTY (@p "\0")
 * @pre @p type must be either @p PROTON, @p WINDOW_EXEC, @p LINUX_EXEC, @p REGFILE, @p REGDIRECTORY
 * @pre or @p OTHER
 * @return a pointer of @p data_s or @p NULL if one precondition is not met
 **/
data_s* data_new(const char* name, const char* path, data_t type);

/**
 * @brief Get the name of the data object
 * @param data a pointer of a @p data_s instance
 * @pre @p data must not be @p NULL
 * @return the name of the data object or @p NULL if @p data is @p NULL
 **/
const char* data_get_name(const data_s* data);

/**
 * @brief Get the path of the data object
 * @param data a pointer of a @p data_s instance
 * @pre @p data must not be @p NULL
 * @return the path of the data object or @p NULL if @p data is @p NULL
 **/
const char* data_get_path(const data_s* data);

/**
 * @brief Get the type of the data object
 * @param data a pointer of a data instance
 * @pre @p data must not be @p NULL
 * @return the type of the data object or @p NULL if @p data is @p NULL
 **/
data_t data_get_type(const data_s* data);

/**
 * @brief Set the type of the data object
 * @param data a pointer of a @p data_s instance
 * @param type the type to be set
 * @pre @p data must not be @p NULL
 * @pre @p type must be either @p PROTON, @p WINDOW_EXEC, @p LINUX_EXEC, @p REGFILE, @p REGDIRECTORY
 * @return @p false if one precondition is not met otherwise @p true
 **/
bool data_set_type(data_s* data, data_t type);

/**
 * @brief Delete the data and frees the allocated memory
 * @param data a pointer of a data instance
 **/
void data_destroy(data_s* d);

#endif
