#ifndef SEED_WRITING_SYSTEM_H
# define SEED_WRITING_SYSTEM_H

# include <stdbool.h>
# include "systems/writing/commands.h"

// +===----- Types -----===+ //

typedef struct s_CommandEntry	t_CommandEntry;
typedef struct s_Manager		t_Manager;
typedef struct s_Buffer			t_Buffer;

typedef struct s_WritingCtx
{
	t_Buffer	**buffers;
	size_t		count;
	size_t		capacity;
}	t_WritingCtx;

// +===----- Commands -----===+ //

#define WRITING_COMMANDS_COUNT 9

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