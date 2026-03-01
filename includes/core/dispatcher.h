/**
 * @file dispatcher.h
 * 
 * @brief Dispatcher system, the backbone of the control system.
 * 	Manages customer order redirects.
 * 	Under the direct supervision of the manager.
 * 
 * @ingroup core
*/

#ifndef SEED_DISPATCHER_H
# define SEED_DISPATCHER_H

# include "seed.h"
# include "dependency.h"

// +===----- Types -----===+ //

/* A command function */
typedef t_ErrorCode		(*t_Fn)(t_Manager *manager, const t_Command *cmd);

/* An entry command */
typedef struct	s_CommandEntry
{
	t_CommandId	id;	/* The command ID */
	t_Fn		fn;	/* The function to execute */
}	t_CommandEntry;

/* The dispatcher */
typedef struct	s_Dispatcher
{
	t_CommandEntry	*commands;	/* The commands */
	size_t		count;	/* The count of commands */
	size_t		capacity;	/* The capacity of commands */
}	t_Dispatcher;

// +===----- Functions -----===+ //

/**
 * @brief Initialize the dispatcher.
 * 
 * @param manager The manager must not be NULL.
 * @param capacity The capacity of the dispatcher.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `manager` is NULL or an error occurred.
*/
bool	dispatcher_init(t_Manager *manager, size_t capacity);

/**
 * @brief Clean the dispatcher.
 * 
 * @param dispatcher The dispatcher must not be NULL.
*/
void	dispatcher_clean(t_Dispatcher *dispatcher);

/**
 * @brief Place a single order with the dispatcher.
 * 
 * @details The command is registered in the dispatcher.
 * 	Once registered, it becomes part of the commands
 * 	that the dispatcher can execute.
 * 
 * @param dispatcher The dispatcher must not be NULL.
 * @param id The ID of command.
 * @param fn The function must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dispatcher` or `fn` is NULL
 * 	or an error occured.
 * 
*/
bool	dispatcher_register(
	t_Dispatcher *dispatcher,
	t_CommandId id, t_Fn fn
);

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
t_ErrorCode	dispatcher_exec(t_Manager *manager, const t_Command *cmd);

#endif