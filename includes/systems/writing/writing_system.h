/**
 * @file "systems/writing/writing_system.h"
 * 
 * @brief The main of the writing system. He registers these
 * 		commands with the dispatcher and manage the system
 * 		(initialization / cleanup)
 * 
 * @ingroup "writing system"
*/

#ifndef SEED_WRITING_SYSTEM_H
# define SEED_WRITING_SYSTEM_H

# include "core/manager.h"
# include "systems/writing/writing_internal.h"

/* The writing context of the seed core */
typedef struct s_WritingCtx
{
	t_Buffer	**buffers;	/* Buffers in the writing context */
	size_t		count;		/* Buffers count */
	size_t		capacity;	/* Buffers capacity */
}	t_WritingCtx;

// +===----- Commands -----===+ //

# define WRITING_COMMANDS_COUNT 9

extern const t_CommandEntry	writing_commands[];

// +===----- Functions -----===+ //

/**
 * @brief Initialize the writing system.
 * 
 * @param manager The manager must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `manager` is NULL or an error occurred.
*/
bool	writing_init(t_Manager	*manager);

/**
 * @brief Clean the writing context.
 * 
 * @param ctx The writing context must not be NULL.
*/
void	writing_clean(t_WritingCtx *ctx);

#endif