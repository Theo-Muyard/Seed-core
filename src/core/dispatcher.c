#include <stdlib.h>
#include "seed.h"
#include "core/manager.h"
#include "core/dispatcher.h"

/**
 * @brief Search for the entry associated with the given ID.
 * @param dispatcher The dispatcher that will contains commands.
 * @param id The id of the command.
 * @return The command entry associated with the given ID, or NULL if not found.
*/
static t_CommandEntry	*search_command_entry(t_Dispatcher *dispatcher, t_CommandId id)
{
	size_t	_count;
	size_t	_i;

	if (NULL == dispatcher)
		return (NULL);
	_i = 0;
	_count = dispatcher->count;
	while (_i < _count)
	{
		if (dispatcher->commands[_i].id == id)
			return (&dispatcher->commands[_i]);
		_i++;
	}
	return (NULL);
}

bool	dispatcher_init(t_Manager *manager, size_t capacity)
{
	t_Dispatcher	*_dispatcher;

	_dispatcher = malloc(sizeof(t_Dispatcher));
	TEST_NULL(_dispatcher, false);
	_dispatcher->count = 0;
	_dispatcher->capacity = capacity;
	_dispatcher->commands = malloc(capacity * sizeof(t_CommandEntry));
	if (NULL == _dispatcher->commands)
		return (free(_dispatcher), false);
	manager->dispatcher = _dispatcher;
	return (true);
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
	t_CommandId id,
	t_Fn fn
)
{
	t_CommandEntry	_entry;
	size_t		_count;

	TEST_NULL(dispatcher, false);
	TEST_NULL(fn, false);
	TEST_NULL(dispatcher->commands, false);
	_count = dispatcher->count;
	if (_count >= dispatcher->capacity)
		return (false);
	_entry.id = id;
	_entry.fn = fn;
	dispatcher->commands[_count] = _entry;
	dispatcher->count++;
	return (true);
}

t_ErrorCode	dispatcher_exec(t_Manager *manager, const t_Command *cmd)
{
	t_CommandEntry	*_cmd_entry;

	TEST_NULL(manager, ERR_INVALID_MANAGER);
	TEST_NULL(cmd, ERR_INVALID_COMMAND);
	TEST_NULL(manager->dispatcher, ERR_DISPATCHER_NOT_INITIALIZED);

	_cmd_entry = search_command_entry(manager->dispatcher, cmd->id);
	TEST_NULL(_cmd_entry, ERR_INVALID_COMMAND_ID);
	return (_cmd_entry->fn(manager, cmd));
}
