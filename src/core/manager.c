#include "core/manager.h"

t_Manager	*manager_init(void)
{
	t_Manager	*manager = malloc(sizeof(t_Manager));
	RETURN_IF_NULL(manager, NULL);

	size_t	_size = WRITING_COMMANDS_COUNT + FS_COMMANDS_COUNT;

	manager->dispatcher = NULL;
	manager->writing_ctx = NULL;
	manager->fs_ctx = NULL;

	GOTO_IF_FALSE(dispatcher_init(manager, _size), exit_manager_clean);
	GOTO_IF_FALSE(writing_init(manager), exit_manager_clean);
	GOTO_IF_FALSE(fs_init(manager), exit_manager_clean);

	return (manager);

	/* GOTO EXIT */
	exit_manager_clean:
		return (manager_clean(manager), NULL);
}

void		manager_clean(t_Manager *manager)
{
	if (NULL == manager)
		return ;

	dispatcher_clean(manager->dispatcher);
	writing_clean(manager->writing_ctx);
	fs_clean(manager->fs_ctx);
	free(manager);
}

t_ErrorCode	manager_exec(t_Manager *manager, t_Command *cmd)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(cmd, ERR_INVALID_COMMAND);

	return (dispatcher_exec(manager, cmd));
}
