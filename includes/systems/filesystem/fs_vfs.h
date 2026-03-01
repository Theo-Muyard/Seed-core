#ifndef SEED_FILESYSTEM_INTERNAL_H
# define SEED_FILESYSTEM_INTERNAL_H

# include "dependency.h"

# define FILE_ALLOC 32
# define DIR_ALLOC 32

// +===----- Types -----===+ //

/* A file in the filesystem */
typedef struct	s_File
{
	char				*filename;	/* The filename */
	struct s_Directory	*parent;	/* The parent directory */
}	t_File;

/* A directory in the filesystem */
typedef struct	s_Directory
{
	char				*dirname;	/* The dirname */
	struct s_Directory	*parent;	/* The parent directory */

	t_File				**files;	/* The files his contains */
	size_t				files_count;	/* The files count */
	size_t				files_capacity;	/* The file capacity */

	struct s_Directory	**subdir;	/* The subdir his contains */
	size_t				subdir_count;	/* The subdir count */
	size_t				subdir_capacity;	/* The subdir capacity */
}	t_Directory;

// +===----- Path -----===+ //

/**
 * @brief Join two paths.
 * @param base The base path.
 * @param path The path that will be added to the end of the base.
 * @return The joined allocated path.
*/
char		*join_path(const char *base, const char *path);

/**
 * @brief Get the relative path of a dir.
 * @param dir The dir to find his relative path.
 * @return The relative path.
*/
char		*directory_get_relative_path(const t_Directory *dir);

/**
 * @brief Get the relative path of a file.
 * @param file The file to find his relative path.
 * @return The relative path.
*/
char		*file_get_relative_path(const t_File *file);

// +===----- Directory -----===+ //

/**
 * @brief Creates a new empty directory.
 * @param parent The parent directory.
 * @param dirname The name of the directory that will be created.
 * @return The directory that has just been created.
*/
t_Directory	*directory_create(t_Directory *parent, const char *dirname);

/**
 * @brief Destroys the given directory.
 * @param dir The directory that will be destroyed.
*/
void		directory_destroy(t_Directory *dir);

// +===----- Files -----===+ //

/**
 * @brief Creates a new empty file.
 * @param parent The parent directory.
 * @param filename The name of the file that will be created.
 * @return The file that has just been created.
*/
t_File		*file_create(t_Directory *parent, const char *filename);

/**
 * @brief Destroys the given file.
 * @param file The file that will be destroyed.
*/
void		file_destroy(t_File *file);

/**
 * @brief Add the given file to the directory.
 * @param dir The directory that contains files.
 * @param file The file that will be added. 
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_file_add(t_Directory *dir, t_File *file);

/**
 * @brief Remove the given file.
 * @param dir The directory that contains files.
 * @param file The file that will be removed.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_file_remove(t_Directory *dir, t_File *file);

/**
 * @brief Find a file by its name in the given directory.
 * @param parent The parent directory of the file that contains files and sub directory.
 * @param filename The name of the file.
 * @return The file or NULL if not found.
*/
t_File		*directory_find_file(t_Directory *parent, const char *filename);

/**
 * @brief Resolve a relative path.
 * @param root The root directory.
 * @param path The relative path of the file.
 * @return The file if it was find or NULL.
*/
t_File		*file_resolve(t_Directory *root, const char *path);

/**
 * @brief Move a file in src to dst directory.
 * @param dst The destination directory.
 * @param src The source directory.
 * @param file The file that will be moved.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_file_move(t_Directory *dst, t_Directory *src, t_File *file);

/**
 * @brief Rename a file.
 * @param subdir The file that will be renamed.
 * @param filename The new filename
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_file_rename(t_File *file, const char *filename);

/**
 * @brief Check if the directory contains a specific file.
 * @param dir The directory that contains files and sub directory.
 * @param file The file.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_contains_file(t_Directory *dir, t_File *file);

// +===----- Sub directory -----===+ //

/**
 * @brief Add the given sub directory to the directory.
 * @param dir The directory that contains files.
 * @param subdir The sub directory that will be added.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_add(t_Directory *dir, t_Directory *subdir);

/**
 * @brief Remove the given sub directory.
 * @param dir The directory that contains files.
 * @param file The subdirectory that will be removed.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_remove(t_Directory *dir, t_Directory *subdir);

/**
 * @brief Find a sub directory by its name in the given directory.
 * @param parent The parent directory of the subdir that contains files and sub directory.
 * @param dirname The name of the sub directory.
 * @return The subdir or NULL if not found.
*/
t_Directory	*directory_find_subdir(t_Directory *parent, const char *dirname);

/**
 * @brief Resolve a relative path.
 * @param root The root directory.
 * @param path The relative path of the dir.
 * @return The dir if it was find or NULL.
*/
t_Directory	*directory_resolve(t_Directory *root, const char *path);

/**
 * @brief Move a sub directory to src in dst directory.
 * @param dst The destination directory.
 * @param src The source directory.
 * @param subdir The sub directory that will be moved.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_move(t_Directory *dst, t_Directory *src, t_Directory *subdir);

/**
 * @brief Rename a sub directory.
 * @param subdir The sub directory that will be renamed.
 * @param dirname The new dirname
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_rename(t_Directory *dir, const char *dirname);

/**
 * @brief Check if the directory contains a specific sub directory.
 * @param dir The directory that contains files and sub directory.
 * @param subdir The subdir.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_contains_subdir(t_Directory *dir, t_Directory *subdir);

#endif