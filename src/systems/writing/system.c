#include <stdlib.h>
#include "core/manager.h"
#include "core/dispatcher.h"
#include "systems/writing/_internal.h"
#include "systems/writing/system.h"

// +===----- Commands Definition -----===+ //

const t_CommandEntry	writing_commands[] = {
	{ CMD_WRITING_CREATE_BUFFER,	cmd_buffer_create},
	{ CMD_WRITING_DELETE_BUFFER,	cmd_buffer_destroy},

	{ CMD_WRITING_INSERT_LINE,		cmd_buffer_line_insert},
	{ CMD_WRITING_DELETE_LINE,		cmd_buffer_line_destroy},
	{ CMD_WRITING_SPLIT_LINE,		cmd_buffer_line_split},
	{ CMD_WRITING_JOIN_LINE,		cmd_buffer_line_join},
	{ CMD_WRITING_GET_LINE,			cmd_buffer_get_line},

	{ CMD_WRITING_INSERT_TEXT,		cmd_line_add_data},
	{ CMD_WRITING_DELETE_TEXT,		cmd_line_delete_data},
};

static bool	register_commands(t_Dispatcher *dispatcher, const t_CommandEntry *commands, size_t size)
{
	size_t	_i;

	_i = 0;
	while (_i < size)
	{
		if (false == dispatcher_register(dispatcher, commands[_i].id, commands[_i].fn))
			return (false);
		_i++;
	}
	return (true);
}

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
