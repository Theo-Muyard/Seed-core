#include <stdlib.h>
#include "core/manager.h"
#include "core/dispatcher.h"
#include "systems/writing/system.h"

// +===----- Functions -----===+ //

t_Manager	*manager_init(void)
{
	t_Manager		*manager;
	size_t			_size;

	manager = malloc(sizeof(t_Manager));
	if (NULL == manager)
		return (NULL);
	_size = WRITING_COMMANDS_COUNT;
	if (false == dispatcher_init(manager, _size))
		return (manager_clean(manager), NULL);
	if (false == writing_init(manager))
		return (manager_clean(manager), NULL);
	return (manager);
}

void		manager_clean(t_Manager *manager)
{
	if (NULL == manager)
		return ;
	dispatcher_clean(manager->dispatcher);
	writing_clean(manager->writing_ctx);
	free(manager);
}

bool		manager_exec(t_Manager *manager, t_Command *cmd)
{
	if (NULL == manager || NULL == manager->dispatcher || NULL == cmd)
		return (false);
	if (false == dispatcher_exec(manager, cmd))
		return (false);
	return (true);
}
