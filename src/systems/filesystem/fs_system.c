#include "core/dispatcher.h"
#include "tools/memory.h"
#include "tools/systems.h"
#include "systems/filesystem/vfs/_internal.h"
#include "systems/filesystem/_os.h"
#include "systems/filesystem/commands.h"
#include "systems/filesystem/system.h"

// +===----- Commands Definition -----===+ //

const t_CommandEntry	fs_commands[] = {
	{ CMD_FS_OPEN_ROOT,		sizeof(t_CmdOpenRoot),		cmd_root_open },
	{ CMD_FS_CLOSE_ROOT,	sizeof(NULL),				cmd_root_close },

	{ CMD_FS_CREATE_DIR,	sizeof(t_CmdCreateDir),		cmd_directory_create },
	{ CMD_FS_DELETE_DIR,	sizeof(t_CmdDeleteDir),		cmd_directory_delete },
	{ CMD_FS_MOVE_DIR,		sizeof(t_CmdMoveDir),		cmd_directory_move },

	{ CMD_FS_CREATE_FILE,	sizeof(t_CmdCreateFile),	cmd_file_create },
	{ CMD_FS_DELETE_FILE,	sizeof(t_CmdDeleteFile),	cmd_file_delete },
	{ CMD_FS_MOVE_FILE,		sizeof(t_CmdMoveFile),		cmd_file_move },
	{ CMD_FS_READ_FILE,		sizeof(t_CmdReadFile),		cmd_file_read },
	{ CMD_FS_WRITE_FILE,	sizeof(t_CmdWriteFile),		cmd_file_write }
};

bool	fs_init(t_Manager *manager)
{
	t_FileSystemCtx		*_ctx;

	TEST_NULL(manager, false);
	TEST_NULL(manager->dispatcher, false);
	_ctx = malloc(sizeof(t_FileSystemCtx));
	TEST_NULL(_ctx, false);
	if (false == register_commands(
		manager->dispatcher,
		fs_commands,
		FS_COMMANDS_COUNT
	))
		return (free(_ctx->root), free(_ctx), false);
	_ctx->root = NULL;
	_ctx->root_path = NULL;
	_ctx->path_len = 0;
	manager->fs_ctx = _ctx;
	return (true);
}

void	fs_clean(t_FileSystemCtx *ctx)
{
	if (NULL == ctx)
		return ;
	directory_destroy(ctx->root);
	free(ctx->root_path);
	free(ctx);
}
