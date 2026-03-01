#ifndef SEED_FILESYSTEM_SYSTEM_H
# define SEED_FILESYSTEM_SYSTEM_H

# include "dependency.h"
# include "core/dispatcher.h"

// +===----- Types -----===+ //

typedef struct s_Manager		t_Manager;
typedef struct s_Directory		t_Directory;

/* The filesystem of the seed core */
typedef struct s_FileSystemCtx
{
	t_Directory	*root;	/* The root directory */
	char		*root_path;	/* The absolute root path directory */
	size_t		path_len;	/* The length of the root path */
}	t_FileSystemCtx;

// +===----- Commands -----===+ //

# define FS_COMMANDS_COUNT 10

extern const t_CommandEntry	fs_commands[];

// +===----- Functions -----===+ //

/**
 * @brief Initialize the filesystem system.
 * @param manager The seed core manager.
 * @return TRUE for success or FALSE if an error occured.
*/
bool	fs_init(t_Manager	*manager);

/**
 * @brief Clean the writing system.
 * @param ctx The filesystem context.
*/
void	fs_clean(t_FileSystemCtx *ctx);

#endif