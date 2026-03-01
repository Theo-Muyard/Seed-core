/**
 * @file manager.h
 * 
 * @brief The seed-core manager, all customer interactions
 * 	 go through the manager. 
 * 
 * @ingroup core
*/

#ifndef SEED_MANAGER_H
# define SEED_MANAGER_H

# include "dispatcher.h"
# include "systems/writing/writing_system.h"
# include "systems/filesystem/fs_system.h"

/* The seed API manager */
typedef struct	s_Manager
{
	t_Dispatcher	*dispatcher;	/* The dispatcher */
	t_WritingCtx	*writing_ctx;	/* The writing context */
	t_FileSystemCtx	*fs_ctx;	/* The filesysten context */
}	t_Manager;

/**
 * @brief Initialize the manager.
 * 
 * @return A newly allocated manager owned by caller.
 * 
 * @warning Caller must free the returned pointer with `manager_clean()`
*/
t_Manager	*manager_init(void);

/**
 * @brief Clean the manager.
 * 
 * @param manager The manager must not be NULL.
*/
void		manager_clean(t_Manager *manager);

/**
 * @brief Execute a specific command.
 * 
 * @details The command informations must be entered correctly.
 * 	The function for the command ID will be executed
 * 	with the parameters provided.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command must not be NULL.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	manager_exec(t_Manager *manager, t_Command *cmd);

#endif