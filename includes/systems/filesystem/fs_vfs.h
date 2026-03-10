/**
 * @file "systems/filesystem/fs_vfs.h"
 * 
 * @brief Managing virtual file system. The virtual file system
 * 		internally replicates the operating system's file system.
 * 
 * @ingroup "filesystem"
*/

#ifndef SEED_FILESYSTEM_VFS_H
# define SEED_FILESYSTEM_VFS_H

# include "dependency.h"

# define FILE_ALLOC 32
# define DIR_ALLOC 32

// +===----- Types -----===+ //

/* A file in the filesystem */
typedef struct	s_File
{
	char			*filename;	/* The filename */
	struct s_Directory	*parent;	/* The parent directory */
}	t_File;

/* A directory in the filesystem */
typedef struct	s_Directory
{
	char			*dirname;		/* The dirname */
	struct s_Directory	*parent;		/* The parent directory */

	t_File			**files;		/* The files his contains */
	size_t			files_count;		/* The files count */
	size_t			files_capacity;		/* The file capacity */

	struct s_Directory	**subdirs;		/* The subdir his contains */
	size_t			subdirs_count;		/* The subdir count */
	size_t			subdirs_capacity;	/* The subdir capacity */
}	t_Directory;

// +===----- Resolving functions -----===+ //

/**
 * @brief Retrieve the relative path of the folder,
 * 		relative to the root.
 * 
 * @param dir The folder must not be NULL.
 * 
 * @retval The newly allocated relative path.
 * @retval NULL if `dir` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char		*vfs_get_dir_relative_path(const t_Directory *dir);

/**
 * @brief Retrieve the relative path of the file,
 * 		relative to the root.
 * 
 * @param file The file must not be NULL.
 * 
 * @retval The newly allocated relative path.
 * @retval NULL if `file` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char		*vfs_get_file_relative_path(const t_File *file);

// +===----- Directory functions -----===+ //

/**
 * @brief Creates a new empty directory.
 * 
 * @param parent The parent can be NULL.
 * @param dirname The dirname must not be NULL.
 * 
 * @retval The newly allocated directory.
 * @retval NULL if `dirname` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `vfs_dir_destroy()`.
*/
t_Directory	*vfs_dir_create(t_Directory *parent, const char *dirname);

/**
 * @brief Destroys the directory.
 * 
 * @param dir The directory must not be NULL.
*/
void		vfs_dir_destroy(t_Directory *dir);

/**
 * @brief Rename a folder.
 * 
 * @param dir The dir must not be NULL.
 * @param dirname The dirname must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `dirname` is NULL, if the dirname
 * 	is same as the current one or an error occurred.
*/
bool		vfs_dir_rename(t_Directory *dir, const char *dirname);

// +===----- File functions -----===+ //

/**
 * @brief Creates a new empty file.
 * 
 * @param parent The parent can be NULL.
 * @param filename The filename must not be NULL.
 * 
 * @retval The newly allocated file.
 * @retval NULL if `filename` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `vfs_file_destroy()`.
*/
t_File	*vfs_file_create(t_Directory *parent, const char *filename);

/**
 * @brief Destroys the file.
 * 
 * @param file The file must not be NULL.
*/
void		vfs_file_destroy(t_File *file);

/**
 * @brief Add the file to the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param file The file must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `file` is NULL or an error occurred.
*/
bool		vfs_add_file_to_dir(t_Directory *dir, t_File *file);

/**
 * @brief Remove the file to the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param file The file must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `file` is NULL or an error occurred.
*/
bool		vfs_remove_file_to_dir(t_Directory *dir, t_File *file);

/**
 * @brief Search for a file by its filename ONLY in its parent directory.
 * 
 * @param parent The parent must not be NULL.
 * @param filename The filename must not be NULL.
 * 
 * @retval The file.
 * @retval NULL if `parent` or `filename` is NULL, if the file was not found
 * 		or an error occurred.
*/
t_File	*vfs_file_find(t_Directory *parent, const char *filename);

/**
 * @brief Resolves a relative file path.
 * 
 * @param root The root must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval The file.
 * @retval NULL if `root` or `path` is NULL, if the file was not found
 * 		or an error occurred.
*/
t_File	*vfs_file_resolve(t_Directory *root, const char *path);

/**
 * @brief Move a file from the source folder to the destination folder.
 * 
 * @param dst The destination folder must not be NULL.
 * @param src The source folder must not be NULL.
 * @param file The file must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dst`, `src` or `file` is NULL or an error occurred.
*/
bool		vfs_file_move(t_Directory *dst, t_Directory *src, t_File *file);

/**
 * @brief Rename a file.
 * 
 * @param file The file must not be NULL.
 * @param filename The filename must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `file` or `filename` is NULL or an error occurred.
*/
bool		vfs_file_rename(t_File *file, const char *filename);

/**
 * @brief Check if the file is contained in the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param file The file must not be NULL.
 * 
 * @retval TRUE if the file is in folder.
 * @retval FALSE if `dir` or `file` is NULL, if the file is not in folder 
 * 		or an error occurred.
*/
bool		vfs_file_is_in_dir(t_Directory *dir, t_File *file);

// +===----- Subdir functions -----===+ //

/**
 * @brief Add the sub folder to the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param subdir The sub directory must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `subdir` is NULL or an error occurred.
*/
bool		vfs_add_subdir_to_dir(t_Directory *dir, t_Directory *subdir);

/**
 * @brief Remove the sub folder to the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param subdir The sub directory must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `subdir` is NULL or an error occurred.
*/
bool		vfs_remove_subdir_to_dir(t_Directory *dir, t_Directory *subdir);

/**
 * @brief Search for a sub folder by its dirname ONLY in its parent directory.
 * 
 * @param parent The parent must not be NULL.
 * @param dirname The dirname must not be NULL.
 * 
 * @retval The subdir.
 * @retval NULL if `parent` or `dirname` is NULL, if the subdir was not found
 * 		or an error occurred.
*/
t_Directory	*vfs_subdir_find(t_Directory *parent, const char *dirname);

/**
 * @brief Resolves a relative sub folder path.
 * 
 * @param root The root must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval The subdir.
 * @retval NULL if `root` or `path` is NULL, if the subdir was not found
 * 		or an error occurred.
*/
t_Directory	*vfs_subdir_resolve(t_Directory *root, const char *path);

/**
 * @brief Move a sub folder from the source folder to the destination folder.
 * 
 * @param dst The destination folder must not be NULL.
 * @param src The source folder must not be NULL.
 * @param subdir The sub folder must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dst`, `src` or `subdir` is NULL or an error occurred.
*/
bool		vfs_subdir_move(t_Directory *dst, t_Directory *src, t_Directory *subdir);

/**
 * @brief Check if the sub folder is contained in the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param subdir The sub folder must not be NULL.
 * 
 * @retval TRUE if the sub folder is in folder.
 * @retval FALSE if `dir` or `subdir` is NULL, if the sub folder is not in folder 
 * 		or an error occurred.
*/
bool		vfs_subdir_is_in_dir(t_Directory *dir, t_Directory *subdir);

#endif