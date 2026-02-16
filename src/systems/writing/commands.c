#include <stdlib.h>
#include "core/manager.h"
#include "core/dispatcher.h"
#include "systems/writing/_internal.h"
#include "systems/writing/commands.h"
#include "systems/writing/system.h"

#define DATA_ALLOC 16

// +===----- Buffer -----===+ //

/**
 * @brief Manage the capacity of the writing context.
 * @param ctx The context of the system.
 * @param id The id of the buffer.
 * @return TRUE for success or FALSE if an error occured.
*/
static bool	manage_capacity(t_WritingCtx *ctx, size_t id)
{
	t_Buffer			**_tmp;
	size_t				_old_cap;
	size_t				_i;

	if (NULL == ctx->buffers)
	{
		ctx->buffers = malloc(DATA_ALLOC * sizeof(t_Buffer *));
		if (NULL == ctx->buffers)
			return (false);
		ctx->capacity = DATA_ALLOC;
		for (_i = 0; _i < ctx->capacity; _i++)
			ctx->buffers[_i] = NULL;
	}
	if (id >= ctx->capacity)
	{
		_old_cap = ctx->capacity;
		_tmp = realloc(
			ctx->buffers,
			(ctx->capacity + DATA_ALLOC) * sizeof(t_Buffer *)
		);
		if (NULL == _tmp)
			return (false);
		ctx->buffers = _tmp;
		ctx->capacity += DATA_ALLOC;
		for (_i = _old_cap; _i < ctx->capacity; _i++)
			ctx->buffers[_i] = NULL;
	}
	return (true);
}

bool	cmd_buffer_create(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_Buffer			*_buffer;
	t_CmdCreateBuffer	*_payload;
	size_t				_i;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdCreateBuffer))
		return (false);
	_payload = cmd->payload;
	_i = 0;
	while (_i < _ctx->capacity && _ctx->buffers[_i])
		_i++;
	_buffer = buffer_create();
	if (NULL == _buffer)
		return (false);
	if (false == manage_capacity(_ctx, _i))
		return (buffer_destroy(_buffer), false);

	_ctx->buffers[_i] = _buffer;
	_payload->out_buffer_id = _i;
	_ctx->count++;
	return (true);
}

bool	cmd_buffer_destroy(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdDestroyBuffer	*_payload;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdDestroyBuffer))
		return (false);
	_payload = cmd->payload;
	if (_payload->buffer_id >= _ctx->capacity)
		return (false);
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	buffer_destroy(_ctx->buffers[_payload->buffer_id]);
	_ctx->buffers[_payload->buffer_id] = NULL;
	_ctx->count--;
	return (true);
}

// +===----- Lines -----===+ //

bool	cmd_buffer_line_insert(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdInsertLine		*_payload;
	t_Line				*_line;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdInsertLine))
		return (false);
	_payload = cmd->payload;
	_line = line_create();
	if (NULL == _line)
		return (false);
	if (NULL == _ctx->buffers[_payload->buffer_id]
		|| (ssize_t)_payload->line > _ctx->buffers[_payload->buffer_id]->size)
		return (free(_line), false);
	if (false == buffer_line_insert(
		_ctx->buffers[_payload->buffer_id],
		_line,
		_payload->line
	))
		return (free(_line), false);
	return (true);
}

bool	cmd_buffer_line_destroy(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdDestroyLine	*_payload;
	t_Line				*_line;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdDestroyLine))
		return (false);
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (false);
	buffer_line_destroy(_ctx->buffers[_payload->buffer_id], _line);
	return (true);
}

bool	cmd_buffer_line_split(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdSplitLine		*_payload;
	t_Line				*_line;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdSplitLine))
		return (false);
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (false);
	if (NULL == buffer_line_split(_ctx->buffers[_payload->buffer_id], _line, _payload->index))
		return (false);
	return (true);
}

bool	cmd_buffer_line_join(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdJoinLine		*_payload;
	t_Line				*_dst;
	t_Line				*_src;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdJoinLine))
		return (false);
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	_dst = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->dst);
	_src = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->src);
	if (_src == _dst || _src->prev != _dst)
		return (false);
	if (NULL == _dst || NULL == _src)
		return (false);
	if (NULL == buffer_line_join(_ctx->buffers[_payload->buffer_id], _dst, _src))
		return (false);
	return (true);
}

bool	cmd_buffer_get_line(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdGetLine		*_payload;
	t_Line				*_line;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdGetLine))
		return (false);
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (false);
	_payload->out_data = _line->data;
	_payload->out_len = _line->len;
	return (true);
}

// +===----- Data -----===+ //

bool	cmd_line_add_data(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdAddData		*_payload;
	t_Line				*_line;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdAddData))
		return (false);
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (false);
	if (false == line_add_data(_line, _payload->column, _payload->size, _payload->data))
		return (false);
	return (true);
}

bool	cmd_line_delete_data(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdDeleteData		*_payload;
	t_Line				*_line;

	if (NULL == manager || NULL == manager->writing_ctx || NULL == cmd)
		return (false);
	_ctx = manager->writing_ctx;
	if (cmd->payload_size && cmd->payload_size != sizeof(t_CmdDeleteData))
		return (false);
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (false);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (false);
	if (false == line_delete_data(_line, _payload->column, _payload->size))
		return (false);
	return (true);
}
