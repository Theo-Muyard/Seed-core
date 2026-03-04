/**
 * @file "systems/filesystem/fs_system.h"
 * 
 * @brief The main of the filesystem. He registers these
 * 		commands with the dispatcher and manage the system
 * 		(initialization / cleanup)
 * 
 * @ingroup "writing system"
*/

#ifndef SEED_FILESYSTEM_SYSTEM_H
# define SEED_FILESYSTEM_SYSTEM_H

# include "dependency.h"
# include "core/manager.h"
# include "systems/filesystem/fs_vfs.h"

/* The filesystem context of the seed core */
typedef struct s_FileSystemCtx
{
	t_Directory	*root;	/* The root directory */
	char		*root_path;	/* The absolute root path */
	size_t	path_len;	/* The path length */
}	t_FileSystemCtx;

// +===----- Commands -----===+ //

# define FS_COMMANDS_COUNT 10

extern const t_CommandEntry	fs_commands[];

// +===----- Functions -----===+ //

/**
 * @brief Initialize the filesystem.
 * 
 * @param manager The manager must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `manager` is NULL or an error occurred.
*/
bool	fs_init(t_Manager	*manager);

/**
 * @brief Clean the filesystem context.
 * 
 * @param ctx The filesystem context must not be NULL.
*/
void	fs_clean(t_FileSystemCtx *ctx);

#endif