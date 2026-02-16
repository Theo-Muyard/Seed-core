#include <stdlib.h>
#include "seed.h"
#include "core/manager.h"
#include "core/dispatcher.h"

/**
 * @brief Search for the function associated with the given ID.
 * @param dispatcher The dispatcher that will contains commands.
 * @param id The id of the command.
*/
static t_Fn	search_function(t_Dispatcher *dispatcher, t_CommandId id)
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
			return (dispatcher->commands[_i].fn);
		_i++;
	}
	return (NULL);
}

bool	dispatcher_init(t_Manager *manager, size_t capacity)
{
	t_Dispatcher	*_dispatcher;

	_dispatcher = malloc(sizeof(t_Dispatcher));
	if (NULL == _dispatcher)
		return (false);
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

bool	dispatcher_register(t_Dispatcher *dispatcher, t_CommandId id, t_Fn fn)
{
	t_CommandEntry	_entry;
	size_t			_count;

	if (NULL == dispatcher || NULL == fn)
		return (false);
	if (NULL == dispatcher->commands)
		return (false);
	_count = dispatcher->count;
	if (_count >= dispatcher->capacity)
		return (false);
	_entry.id = id;
	_entry.fn = fn;
	dispatcher->commands[_count] = _entry;
	dispatcher->count++;
	return (true);
}

bool	dispatcher_exec(t_Manager *manager, const t_Command *cmd)
{
	t_Fn		_fn;
	bool		state;

	if (NULL == manager || NULL == manager->dispatcher|| NULL == cmd)
		return (false);
	_fn = search_function(manager->dispatcher, cmd->id);
	if (NULL == _fn)
		return (false);
	state = _fn(manager, cmd);
	return (state);
}
