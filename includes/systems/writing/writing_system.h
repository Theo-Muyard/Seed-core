#ifndef SEED_WRITING_SYSTEM_H
# define SEED_WRITING_SYSTEM_H

# include "dependency.h"
# include "core/dispatcher.h"

// +===----- Types -----===+ //

typedef struct s_Manager		t_Manager;
typedef struct s_Buffer			t_Buffer;

/* The writing context of the seed core */
typedef struct s_WritingCtx
{
	t_Buffer	**buffers;	/* All buffers in the writing context */
	size_t		count;	/* The count of buffers */
	size_t		capacity;	/* The capacity of buffers */
}	t_WritingCtx;

// +===----- Commands -----===+ //

# define WRITING_COMMANDS_COUNT 9

extern const t_CommandEntry	writing_commands[];

// +===----- Functions -----===+ //

/**
 * @brief Initialize the writing system.
 * @param manager The seed core manager.
 * @return TRUE for success or FALSE if an error occured.
*/
bool	writing_init(t_Manager	*manager);

/**
 * @brief Clean the writing system.
 * @param ctx The writing context.
*/
void	writing_clean(t_WritingCtx *ctx);

#endif