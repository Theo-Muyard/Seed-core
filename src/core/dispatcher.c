#include "core/dispatcher.h"
#include "core/manager.h"

// +===----- Static functions -----===+ //

/**
 * @brief Search for the entry associated with the ID.
 * 
 * @param dispatcher The dispatcher must not be NULL.
 * @param id The command ID.
 * 
 * @return The command entry associated with the given ID, or NULL if not found.
 * @retval The command entry.
 * @retval NULL if `dispatcher` is NULL, the ID is invalid or an error occurred.
*/
static t_CommandEntry	*search_command_entry(t_Dispatcher *dispatcher, t_CommandId id)
{
	RETURN_IF_NULL(dispatcher, NULL);

	size_t	_i = 0;
	while (_i < dispatcher->count)
	{
		if (dispatcher->commands[_i].id == id)
			return (&dispatcher->commands[_i]);
		_i++;
	}
	return (NULL);
}

// +===----- Functions -----===+ //

bool	dispatcher_init(t_Manager *manager, size_t capacity)
{
	RETURN_IF_NULL(manager, false);

	t_Dispatcher	*_dispatcher = malloc(sizeof(t_Dispatcher));
	RETURN_IF_NULL(_dispatcher, false);

	_dispatcher->count = 0;
	_dispatcher->capacity = capacity;

	_dispatcher->commands = malloc(capacity * sizeof(t_CommandEntry));
	GOTO_IF_NULL(_dispatcher->commands, exit_free_dispatcher);

	manager->dispatcher = _dispatcher;
	return (true);

	/* GOTO EXIT */
	exit_free_dispatcher:
		return (free(_dispatcher), false);
}

void	dispatcher_clean(t_Dispatcher *dispatcher)
{
	if (NULL == dispatcher)
		return ;
	free(dispatcher->commands);
	dispatcher->commands = NULL;
	dispatcher->count = 0;
	dispatcher->capacity = 0;
	free(dispatcher);
}

bool	dispatcher_register(
	t_Dispatcher *dispatcher,
	t_CommandId id, t_Fn fn
)
{
	RETURN_IF_NULL(dispatcher, false);
	RETURN_IF_NULL(dispatcher->commands, false);
	RETURN_IF_NULL(fn, false);

	if (dispatcher->count >= dispatcher->capacity)
		return (false);
	t_CommandEntry	_entry = {
		.id = id,
		.fn = fn
	};
	dispatcher->commands[dispatcher->count] = _entry;
	dispatcher->count++;
	return (true);
}

t_ErrorCode	dispatcher_exec(t_Manager *manager, const t_Command *cmd)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(cmd, ERR_INVALID_COMMAND);
	RETURN_IF_NULL(manager->dispatcher, ERR_DISPATCHER_NOT_INITIALIZED);

	t_CommandEntry	*_cmd_entry = search_command_entry(manager->dispatcher, cmd->id);
	RETURN_IF_NULL(_cmd_entry, ERR_INVALID_COMMAND_ID);

	return (_cmd_entry->fn(manager, cmd));
}
