#include "systems/filesystem/fs_system.h"
#include "systems/filesystem/fs_commands.h"

// +===----- Commands Definition -----===+ //

const t_CommandEntry	fs_commands[] = {
	{ CMD_FS_OPEN_ROOT,	cmd_root_open },
	{ CMD_FS_CLOSE_ROOT,	cmd_root_close },

	{ CMD_FS_CREATE_DIR,	cmd_directory_create },
	{ CMD_FS_DELETE_DIR,	cmd_directory_delete },
	{ CMD_FS_MOVE_DIR,	cmd_directory_move },

	{ CMD_FS_CREATE_FILE,	cmd_file_create },
	{ CMD_FS_DELETE_FILE,	cmd_file_delete },
	{ CMD_FS_MOVE_FILE,	cmd_file_move },
	{ CMD_FS_READ_FILE,	cmd_file_read },
	{ CMD_FS_WRITE_FILE,	cmd_file_write }
};

bool	fs_init(t_Manager *manager)
{
	RETURN_IF_NULL(manager, false);
	RETURN_IF_NULL(manager->dispatcher, false);

	t_FileSystemCtx	*_ctx = malloc(sizeof(t_FileSystemCtx));
	RETURN_IF_NULL(_ctx, false);

	_ctx->root = NULL;
	_ctx->root_path = NULL;
	_ctx->path_len = 0;

	GOTO_IF_FALSE(
		register_commands(
			manager->dispatcher,
			fs_commands, FS_COMMANDS_COUNT
		),
		exit_free_ctx
	);
	
	manager->fs_ctx = _ctx;
	return (true);

	/* GOTO EXIT */
	exit_free_ctx:
		return (free(_ctx), false);
}

void	fs_clean(t_FileSystemCtx *ctx)
{
	if (NULL == ctx)
		return ;

	vfs_dir_destroy(ctx->root);
	ctx->root = NULL;
	free(ctx->root_path);
	ctx->root_path = NULL;
	free(ctx);
}
