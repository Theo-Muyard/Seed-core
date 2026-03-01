#ifndef SEED_FILESYSTEM_OS_H
# define SEED_FILESYSTEM_OS_H

# include "dependency.h"

// +===----- OS Directory -----===+ //

/**
 * @brief Create a directory in the os filesystem.
 * @param path The path where the directory that will be created.
 * @param mode The permissions of the directory.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		os_dir_create(char *path, unsigned int mode);

/**
 * @brief Delete a directory in the os filesystem.
 * @param path The path where the directory that will be deleted.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		os_dir_delete(char *path);

/**
 * @brief Move a directory.
 * @param new_path The old path of the directory.
 * @param new_path The new path of the directory.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		os_dir_move(char *old_path, char *new_path);

// +===----- OS Files -----===+ //

/**
 * @brief Create a file in the os filesystem.
 * @param path The path where the file that will be created.
 * @param mode The mode to open the file (r, r+, w, w+, a, a+ : read the man)
 * @return The FILE that will be created.
*/
FILE		*os_file_create(char *path, char *mode);

/**
 * @brief Delete a file in the os filesystem.
 * @param path The path where the file that will be deleted.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		os_file_delete(char *path);

/**
 * @brief Open a file in the os filesystem.
 * @param path The path where the file that will be opened.
 * @param mode The mode to open the file (r, r+, w, w+, a, a+ : read the man)
 * @return The FILE that will be opened.
*/
FILE		*os_file_open(char *path, char *mode);

/**
 * @brief Move a file.
 * @param new_path The old path of the file.
 * @param new_path The new path of the file.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		os_file_move(char *old_path, char *new_path);

/**
 * @brief Write inside a file.
 * @param file The file that will be edited.
 * @param data The new data content.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		os_file_write(FILE *file, char *data);

/**
 * @brief Save a file content.
 * @param file The file that will be saved.
*/
void		os_file_save(FILE *file);

/**
 * @brief Get a file content.
 * @param file The file.
 * @return The content of the file.
*/
char		*os_file_get_data(FILE *file);

#endif