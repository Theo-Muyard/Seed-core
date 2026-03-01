#include "core/dispatcher.h"
#include "systems/writing/_internal.h"
#include "systems/writing/commands.h"
#include "systems/writing/system.h"

#define BUFFER_ALLOC 32

/**
 * @brief Converts the index given to the actual index of the first byte of the character.
 * @param str The data content.
 * @param index The index of the character.
 * @return The real index to the first byte of the character. 
*/
static size_t	utf_char_to_byte(const char *str, size_t index)
{
	size_t	_i;
	size_t	_count;

	TEST_NULL(str, 0);
	_i = 0;
	_count = 0;
	while (str[_i])
	{
		if ((str[_i] & 0xC0) != 0x80)
		{
			if (_count == index)
				return (_i);
			_count++;
		}
		_i++;
	}
	return (_i);
}

// +===----- Buffer -----===+ //

/**
 * @brief Manage the capacity of the writing context.
 * @param ctx The context of the system.
 * @param id The id of the buffer.
 * @return TRUE for success or FALSE if an error occured.
*/
static t_ErrorCode	manage_capacity(t_WritingCtx *ctx, size_t id)
{
	t_Buffer			**_tmp;
	size_t				_old_cap;
	size_t				_i;

	if (NULL == ctx->buffers)
	{
		ctx->buffers = malloc(BUFFER_ALLOC * sizeof(t_Buffer *));
		if (NULL == ctx->buffers)
			return (ERR_INTERNAL_MEMORY);
		ctx->capacity = BUFFER_ALLOC;
		for (_i = 0; _i < ctx->capacity; _i++)
			ctx->buffers[_i] = NULL;
	}
	if (id >= ctx->capacity)
	{
		_old_cap = ctx->capacity;
		_tmp = realloc(
			ctx->buffers,
			(ctx->capacity + BUFFER_ALLOC) * sizeof(t_Buffer *)
		);
		if (NULL == _tmp)
			return (ERR_INTERNAL_MEMORY);
		ctx->buffers = _tmp;
		ctx->capacity += BUFFER_ALLOC;
		for (_i = _old_cap; _i < ctx->capacity; _i++)
			ctx->buffers[_i] = NULL;
	}
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_buffer_create(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_Buffer			*_buffer;
	t_CmdCreateBuffer	*_payload;
	size_t				_i;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	_i = 0;
	while (_i < _ctx->capacity && _ctx->buffers[_i])
		_i++;
	_buffer = buffer_create();
	if (NULL == _buffer)
		return (ERR_INTERNAL_MEMORY);
	if (manage_capacity(_ctx, _i))
		return (buffer_destroy(_buffer), ERR_INTERNAL_MEMORY);

	_ctx->buffers[_i] = _buffer;
	_payload->out_buffer_id = _i;
	_ctx->count++;
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_buffer_destroy(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdDestroyBuffer	*_payload;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (_payload->buffer_id >= _ctx->capacity)
		return (ERR_BUFFER_NOT_FOUND);
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	buffer_destroy(_ctx->buffers[_payload->buffer_id]);
	_ctx->buffers[_payload->buffer_id] = NULL;
	_ctx->count--;
	return (ERR_SUCCESS);
}

// +===----- Lines -----===+ //

t_ErrorCode	cmd_buffer_line_insert(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdInsertLine		*_payload;
	t_Line				*_line;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	if (_payload->line == -1)
		_payload->line = _ctx->buffers[_payload->buffer_id]->size - 1;
	if((size_t)_payload->line > _ctx->buffers[_payload->buffer_id]->size)
		return (ERR_LINE_NOT_FOUND);
	_line = line_create();
	if (NULL == _line)
		return (ERR_INTERNAL_MEMORY);
	if (false == buffer_line_insert(
		_ctx->buffers[_payload->buffer_id],
		_line,
		_payload->line
	))
		return (free(_line), ERR_OPERATION_FAILED);
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_buffer_line_delete(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdDeleteLine		*_payload;
	t_Line				*_line;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (ERR_LINE_NOT_FOUND);
	buffer_line_destroy(_ctx->buffers[_payload->buffer_id], _line);
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_buffer_line_split(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdSplitLine		*_payload;
	t_Line				*_line;
	size_t				_byte_offset;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (ERR_LINE_NOT_FOUND);
	_byte_offset = utf_char_to_byte(_line->data, _payload->index);
	if (NULL == buffer_line_split(_ctx->buffers[_payload->buffer_id], _line, _byte_offset))
		return (ERR_OPERATION_FAILED);
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_buffer_line_join(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdJoinLine		*_payload;
	t_Line				*_dst;
	t_Line				*_src;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	_dst = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->dst);
	_src = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->src);
	if (_src == _dst || _src->prev != _dst)
		return (ERR_INVALID_PAYLOAD);
	if (NULL == _dst || NULL == _src)
		return (ERR_LINE_NOT_FOUND);
	if (NULL == buffer_line_join(_ctx->buffers[_payload->buffer_id], _dst, _src))
		return (ERR_OPERATION_FAILED);
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_buffer_get_line(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdGetLine		*_payload;
	t_Line				*_line;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (ERR_LINE_NOT_FOUND);
	_payload->out_data = _line->data;
	_payload->out_size = _line->size;
	return (ERR_SUCCESS);
}

// +===----- Data -----===+ //

t_ErrorCode	cmd_line_insert_data(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdInsertData		*_payload;
	t_Line				*_line;
	size_t				_byte_offset;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (ERR_LINE_NOT_FOUND);
	_byte_offset = utf_char_to_byte(_line->data, _payload->index);
	if (false == line_insert_data(_line, _byte_offset, _payload->size, _payload->data))
		return (ERR_OPERATION_FAILED);
	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_line_delete_data(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx		*_ctx;
	t_CmdDeleteData		*_payload;
	t_Line				*_line;
	size_t				_byte_start;
	size_t				_byte_end;

	_ctx = manager->writing_ctx;
	_payload = cmd->payload;
	if (NULL == _ctx->buffers[_payload->buffer_id])
		return (ERR_BUFFER_NOT_FOUND);
	_line = buffer_get_line(_ctx->buffers[_payload->buffer_id], _payload->line);
	if (NULL == _line)
		return (ERR_LINE_NOT_FOUND);
	_byte_start = utf_char_to_byte(_line->data, _payload->index);
	_byte_end = utf_char_to_byte(_line->data, _payload->index + _payload->size);
	if (false == line_delete_data(_line, _byte_start, _byte_end - _byte_start))
		return (ERR_OPERATION_FAILED);
	return (ERR_SUCCESS);
}
