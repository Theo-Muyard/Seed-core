#include "core/dispatcher.h"
#include "tools/systems.h"
#include "systems/writing/_internal.h"
#include "systems/writing/commands.h"
#include "systems/writing/system.h"

// +===----- Commands Definition -----===+ //

const t_CommandEntry	writing_commands[] = {
	{ CMD_WRITING_CREATE_BUFFER,	sizeof(t_CmdCreateBuffer),	cmd_buffer_create},
	{ CMD_WRITING_DELETE_BUFFER,	sizeof(t_CmdDestroyBuffer),	cmd_buffer_destroy},
	
	{ CMD_WRITING_INSERT_LINE,		sizeof(t_CmdInsertLine),	cmd_buffer_line_insert},
	{ CMD_WRITING_DELETE_LINE,		sizeof(t_CmdDeleteLine),	cmd_buffer_line_delete},
	{ CMD_WRITING_SPLIT_LINE,		sizeof(t_CmdSplitLine),		cmd_buffer_line_split},
	{ CMD_WRITING_JOIN_LINE,		sizeof(t_CmdJoinLine),		cmd_buffer_line_join},
	{ CMD_WRITING_GET_LINE,			sizeof(t_CmdGetLine),		cmd_buffer_get_line},
	
	{ CMD_WRITING_INSERT_TEXT,		sizeof(t_CmdInsertData),	cmd_line_insert_data},
	{ CMD_WRITING_DELETE_TEXT,		sizeof(t_CmdDeleteData),	cmd_line_delete_data}
};

// +===----- Functions -----===+ //

bool	writing_init(t_Manager	*manager)
{
	t_WritingCtx		*_ctx;

	if (NULL == manager)
		return (false);
	if (NULL == manager->dispatcher)
		return (false);
	_ctx = malloc(sizeof(t_WritingCtx));
	if (NULL == _ctx)
		return (false);
	_ctx->buffers = NULL;
	_ctx->count = 0;
	_ctx->capacity = 0;
	if (false == register_commands(
		manager->dispatcher,
		writing_commands,
		WRITING_COMMANDS_COUNT
	))
		return (free(_ctx), false);
	manager->writing_ctx = _ctx;
	return (true);
}

void	writing_clean(t_WritingCtx *ctx)
{
	size_t	_i;

	if (NULL == ctx)
		return ;
	_i = 0;
	while (_i < ctx->count)
	{
		buffer_destroy(ctx->buffers[_i]);
		_i++;
	}
	free(ctx->buffers);
	ctx->buffers = NULL;
	ctx->count = 0;
	ctx->capacity = 0;
	free(ctx);
}
