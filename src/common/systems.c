#include "core/dispatcher.h"
#include "tools/systems.h"

bool	register_commands(t_Dispatcher *dispatcher, const t_CommandEntry *commands, size_t count)
{
	size_t	_i;

	_i = 0;
	while (_i < count)
	{
		if (false == dispatcher_register(
			dispatcher,
			commands[_i].id,
			commands[_i].size,
			commands[_i].fn)
		)
			return (false);
		_i++;
	}
	return (true);
}
