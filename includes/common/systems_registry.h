/**
 * @file systems_registry.h
 * 
 * @brief Systems helper used across seed-core.
 * 	Provides systems helpers.
 * 
 * @ingroup common
*/

#ifndef SEED_COMMON_SYSTEMS_REGISTRY_H
# define SEED_COMMON_SYSTEMS_REGISTRY_H

# include "core/dispatcher.h"

/**
 * @brief Registers the commands into the dispatcher.
 * 
 * @param dispatcher The dispatcher must not be NULL.
 * @param commands The commands must not be NULL.
 * @param count The number of commands.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dispatcher` or `commands` is NULL
 * 	or an error occurred.
*/
bool	register_commands(
	t_Dispatcher *dispatcher,
	const t_CommandEntry *commands, size_t count
);

#endif