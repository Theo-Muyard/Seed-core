#include "systems/writing/writing_commands.h"
#include "systems/writing/writing_system.h"

// +===----- Static functions -----===+ //

/**
 * @brief Converts a UTF-8 character index to a byte offset.
 *
 * @param str The string must not be NULL.
 * @param index The codepoint index.
 *
 * @retval Byte offset of the requested character.
 * @retval -1 if `str` is NULL or an error occurred.
 */
static size_t utf8_codepoint_to_byte_offset(const char *str, size_t index)
{
	RETURN_IF_NULL(str, -1);

	size_t	_i = 0;
	size_t	_count = 0;

	if (strlen(str) == index)
		return (index);

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
	return (-1);
}

/**
 * @brief Manage the specified buffer capacity allocation.
 *
 * @param ctx The context must not be NULL.
 * @param count The buffers count.
 *
 * @return A specific `ErrorCode`.
 */
static t_ErrorCode manage_buffers_capacity(t_WritingCtx *ctx, size_t count)
{
	RETURN_IF_NULL(ctx, ERR_OPERATION_FAILED);

	if (NULL == ctx->buffers)
	{
		ctx->buffers = malloc(BUFFER_ALLOC * sizeof(t_Buffer *));
		RETURN_IF_NULL(ctx->buffers, ERR_INTERNAL_MEMORY);

		ctx->capacity = BUFFER_ALLOC;
		for (size_t _i = 0; _i < BUFFER_ALLOC; _i++)
			ctx->buffers[_i] = NULL;
	}
	if (count >= ctx->capacity)
	{
		size_t	_old_cap = ctx->capacity;
		t_Buffer	**_tmp = realloc(
			ctx->buffers,
			(ctx->capacity + BUFFER_ALLOC) * sizeof(t_Buffer *)
		);
		RETURN_IF_NULL(_tmp, ERR_INTERNAL_MEMORY);

		ctx->buffers = _tmp;
		ctx->capacity += BUFFER_ALLOC;
		for (size_t _i = _old_cap; _i < ctx->capacity; _i++)
			ctx->buffers[_i] = NULL;
	}
	return (ERR_SUCCESS);
}

// +===----- Buffer functions -----===+ //

t_ErrorCode cmd_buffer_create(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdCreateBuffer *_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	size_t	_i = 0;
	while (_i < _ctx->capacity && _ctx->buffers && _ctx->buffers[_i])
		_i++;

	t_Buffer	*_buffer = buffer_create();
	RETURN_IF_NULL(_buffer, ERR_INTERNAL_MEMORY);

	t_ErrorCode	_code = manage_buffers_capacity(_ctx, _i);
	if (_code)
		return (buffer_destroy(_buffer), _code);

	_ctx->buffers[_i] = _buffer;
	_ctx->count++;
	_payload->out_buffer_id = _i;

	return (ERR_SUCCESS);
}

t_ErrorCode cmd_buffer_destroy(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx 		*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdDestroyBuffer	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	if (_payload->buffer_id >= _ctx->capacity)
		return (ERR_BUFFER_NOT_FOUND);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer	*_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	buffer_destroy(_buffer);
	_ctx->buffers[_payload->buffer_id] = NULL;
	_ctx->count--;

	return (ERR_SUCCESS);
}

// +===----- Line functions -----===+ //

t_ErrorCode cmd_buffer_line_insert(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdInsertLine	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer	*_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	if (_payload->line <= -1)
		_payload->line = _buffer->count ? _buffer->count - 1 : 0;

	if ((size_t)_payload->line > _buffer->count)
		return (ERR_LINE_NOT_FOUND);

	t_Line	*_line = line_create();
	RETURN_IF_NULL(_line, ERR_INTERNAL_MEMORY);

	GOTO_IF_FALSE(buffer_line_insert(_buffer, _line, _payload->line), exit_free_line);
	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_line:
		return (free(_line), ERR_OPERATION_FAILED);
}

t_ErrorCode cmd_buffer_line_delete(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdDeleteLine	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer	*_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	t_Line	*_line = buffer_get_line(_buffer, _payload->line);
	RETURN_IF_NULL(_line, ERR_LINE_NOT_FOUND);

	buffer_line_destroy(_buffer, _line);
	return (ERR_SUCCESS);
}

t_ErrorCode cmd_buffer_line_split(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdSplitLine	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer	*_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	t_Line	*_line = buffer_get_line(_buffer, _payload->line);
	RETURN_IF_NULL(_line, ERR_LINE_NOT_FOUND);

	size_t _byte_offset = utf8_codepoint_to_byte_offset(_line->data, _payload->index);
	if (_byte_offset <= 0 || _byte_offset >= _line->size)
		return (ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(
		buffer_line_split(_buffer, _line, _byte_offset),
		ERR_OPERATION_FAILED);

	return (ERR_SUCCESS);
}

t_ErrorCode cmd_buffer_line_join(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdJoinLine	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer	*_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	t_Line	*_dst = buffer_get_line(_buffer, _payload->dst);
	RETURN_IF_NULL(_dst, ERR_LINE_NOT_FOUND);

	t_Line	*_src = buffer_get_line(_buffer, _payload->src);
	RETURN_IF_NULL(_src, ERR_LINE_NOT_FOUND);

	if (_src == _dst || _src->prev != _dst)
		return (ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(buffer_line_join(_buffer, _dst, _src), ERR_OPERATION_FAILED);
	return (ERR_SUCCESS);
}

t_ErrorCode cmd_buffer_get_line(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdGetLine	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer	*_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	t_Line	*_line = buffer_get_line(_buffer, _payload->line);
	RETURN_IF_NULL(_line, ERR_LINE_NOT_FOUND);

	_payload->out_data = _line->data;
	_payload->out_size = _line->size;

	return (ERR_SUCCESS);
}

// +===----- Data functions -----===+ //

t_ErrorCode cmd_line_insert_data(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdInsertData	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer *_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	t_Line *_line = buffer_get_line(_buffer, _payload->line);
	RETURN_IF_NULL(_line, ERR_LINE_NOT_FOUND);

	if (_payload->index <= -1)
		_payload->index = _line->size ? _line->size : 0;

	size_t	_byte_offset = 0;
	if (_line->size > 0 && _payload->index != 0)
	{
		_byte_offset = utf8_codepoint_to_byte_offset(_line->data, _payload->index);
		if (_byte_offset <= 0 || _byte_offset > _line->size)
			return (ERR_INVALID_PAYLOAD);
	}

	RETURN_IF_FALSE(
		line_insert_data(_line, _byte_offset, _payload->size, _payload->data),
		ERR_OPERATION_FAILED
	);
	
	return (ERR_SUCCESS);
}

t_ErrorCode cmd_line_delete_data(t_Manager *manager, const t_Command *cmd)
{
	t_WritingCtx	*_ctx = manager->writing_ctx;
	RETURN_IF_NULL(_ctx, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_CmdDeleteData	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_ctx->buffers, ERR_WRITING_CONTEXT_NOT_INITIALIZED);
	t_Buffer *_buffer = _ctx->buffers[_payload->buffer_id];
	RETURN_IF_NULL(_buffer, ERR_BUFFER_NOT_FOUND);

	t_Line *_line = buffer_get_line(_buffer, _payload->line);
	RETURN_IF_NULL(_line, ERR_LINE_NOT_FOUND);

	size_t	_byte_offset = utf8_codepoint_to_byte_offset(_line->data, _payload->index);
	if (_byte_offset <= 0 || _byte_offset >= _line->size)
		return (ERR_INVALID_PAYLOAD);

	RETURN_IF_FALSE(
		line_delete_data(_line, _byte_offset, _payload->size),
		ERR_OPERATION_FAILED
	);

	return (ERR_SUCCESS);
}
