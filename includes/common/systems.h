#ifndef SEED_TOOLS_SYSTEMS_H
# define SEED_TOOLS_SYSTEMS_H

#include <sys/types.h>

// +===----- Types -----===+ //

typedef struct s_Dispatcher	t_Dispatcher;
typedef struct s_CommandEntry	t_CommandEntry;

// +===----- Functions -----===+ //

/**
 * @brief Registers the commands into the dispatcher.
 * @param dispatcher The dispatcher that will contains commands.
 * @param commands The container for all write command entries.
 * @param count The count of commands.
*/
bool	register_commands(t_Dispatcher *dispatcher, const t_CommandEntry *commands, size_t count);

#endif