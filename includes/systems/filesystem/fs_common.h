/**
 * @file "systems/filesystem/fs_common.h"
 * 
 * @brief Includes all global dependencies to the
 * 		filesystem.
 * 
 * @ingroup "filesystem"
*/

#ifndef SEED_FILESYSTEM_COMMONL_H
# define SEED_FILESYSTEM_COMMONL_H

# include "seed.h"

/**
 * @brief Join two paths.
 * 
 * @details The paths provided are not modified or free.
 * 		the new path is created from scratch.
 * 
 * @param base The base path must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval The newly allocated joined path.
 * @retval NULL if `base` or `path` is NULL, if allocation fails
 * 		or an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char		*join_path(const char *base, const char *path);

/**
 * @brief Check whether the path is a folder or a file.
 * 
 * @param path The absolute path must not be NULL.
 * 
 * @retval TRUE if its a folder.
 * @retval FALSE if its not a folder or an error occurred.
*/
bool		is_dir(const char *path);

/**
 * @brief Handle the OS folder error.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	handle_dir_error(void);

/**
 * @brief Handle the OS file error.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	handle_file_error(void);

#endif