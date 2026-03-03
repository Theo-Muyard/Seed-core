/**
 * @file "systems/filesystem/fs_os.h"
 * 
 * @brief Manages all interactions with the OS.
 * 
 * @ingroup "filesystem"
*/

#ifndef SEED_FILESYSTEM_OS_H
# define SEED_FILESYSTEM_OS_H

# include "dependency.h"

// +===----- Directory functions -----===+ //

/**
 * @brief Create a folder in the OS filesystem.
 * 
 * @param path The path must not be NULL.
 * @param mode The permission bits mode.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `path` is NULL or an error occurred.
*/
bool		os_dir_create(const char *path, unsigned int mode);

/**
 * @brief Delete a folder in the OS filesystem.
 * `
 * @param path The path must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `path` is NULL or an error occurred.
*/
bool		os_dir_delete(const char *path);

/**
 * @brief Move a directory in the OS filesystem.
 * 
 * @param old_path The old path must not be NULL.
 * @param new_path The new path must not be NULL
 * .
 * @retval TRUE for success.
 * @retval FALSE if `old_path` or `new_path` is NULL or an error occurred.
*/
bool		os_dir_move(const char *old_path, const char *new_path);

// +===----- File functions -----===+ //

/**
 * @brief Create a file in the OS filesystem.
 * 
 * @param path The path must not be NULL.
 * @param mode The mode to open the file (`man fopen`)
 * 
 * @retval A pointer to a FILE.
 * @retval NULL if `path` or `mode` is NULL, if file already exists
 * 		 or an error occurred.
 * 
 * @warning Caller must free returned pointer with `fclose()`.
*/
FILE		*os_file_create(const char *path, const char *mode);

/**
 * @brief Delete a file in the OS filesystem.
 * `
 * @param path The path must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `path` is NULL or an error occurred.
*/
bool		os_file_delete(const char *path);

/**
 * @brief Open a file in the OS filesystem.
 * 
 * @param path The path must a new  not be NULL.
 * @param mode The mode to open the file (`man fopen`)
 * 
 * @retval A pointer to a FILE.
 * @retval NULL if `path` or `mode` is NULL, if file already exists
 * 		 or an error occurred.
 * 
 * @warning Caller must free returned pointer with `fclose()`.
*/
FILE		*os_file_open(const char *path, const char *mode);
 
/**
 * @brief Move a file in the OS filesystem.
 * 
 * @param old_path The old path must not be NULL.
 * @param new_path The new path must not be NULL
 * .
 * @retval TRUE for success.
 * @retval FALSE if `old_path` or `new_path` is NULL or an error occurred.
*/
bool		os_file_move(const char *old_path, const char *new_path);

/**
 * @brief Overwrites the contents of a file with the data.
 * 
 * @param file The file must not be NULL.
 * @param data The data must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `file` or `data` is NULL or an error occurred.
*/
bool		os_file_write(FILE *file, const char *data);

/**
 * @brief Recover the contents of a file.
 * 
 * @param file The file must  a new not be NULL.
 * 
 * @retval A newly allocated string owned by caller.
 * @retval NULL if `file` is NULL or an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char		*os_file_get_data(FILE *file);

#endif