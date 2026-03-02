#include "common/systems_registry.h"

bool	register_commands(
	t_Dispatcher *dispatcher,
	const t_CommandEntry *commands, size_t count
)
{
	RETURN_IF_NULL(dispatcher, false);
	RETURN_IF_NULL(commands, false);

	size_t	_i = 0;
	while (_i < count)
	{
		RETURN_IF_FALSE(
			dispatcher_register(dispatcher, commands->id, commands->fn),
			false
		);
		_i++;
	}
	return (true);
}
