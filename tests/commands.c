#include "tests/commands.h"

t_ErrorCode	test_buffer_create(t_Manager *manager, t_CmdCreateBuffer *payload)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(payload, ERR_INVALID_PAYLOAD);

	t_Command	_cmd = {
		.id = CMD_WRITING_CREATE_BUFFER,
		.payload = payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_buffer_destroy(t_Manager *manager, size_t id)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdDestroyBuffer	_payload = {
		.buffer_id = id
	};

	t_Command			_cmd = {
		.id = CMD_WRITING_DELETE_BUFFER,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_line_insert(t_Manager *manager, size_t buffer_id, ssize_t line)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdInsertLine	_payload = {
		.buffer_id = buffer_id,
		.line = line
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_INSERT_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_line_delete(t_Manager *manager, size_t buffer_id, ssize_t line)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdDeleteLine	_payload = {
		.buffer_id = buffer_id,
		.line = line
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_DELETE_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_get_line(t_Manager *manager, t_CmdGetLine *payload)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(payload, ERR_INVALID_PAYLOAD);

	t_Command	_cmd = {
		.id = CMD_WRITING_GET_LINE,
		.payload = payload
	};

	return (manager_exec(manager, &_cmd));
}


t_ErrorCode	test_line_split(t_Manager *manager, size_t buffer_id, ssize_t line, size_t index)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdSplitLine	_payload = {
		.buffer_id = buffer_id,
		.line = line,
		.index = index
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_SPLIT_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_line_join(t_Manager *manager, size_t buffer_id, size_t dst, size_t src)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdJoinLine	_payload = {
		.buffer_id = buffer_id,
		.dst = dst,
		.src = src
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_JOIN_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_insert_data(
	t_Manager *manager,
	size_t buffer_id, size_t line,
	size_t index, size_t size, const char *data
)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdInsertData	_payload = {
		.buffer_id = buffer_id,
		.line = line,
		.index = index,
		.size = size,
		.data = data
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_INSERT_TEXT,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_delete_data(
	t_Manager *manager, size_t buffer_id,
	size_t line, size_t index, size_t size
)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdDeleteData	_payload = {
		.buffer_id = buffer_id,
		.line = line,
		.index = index,
		.size = size
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_DELETE_TEXT,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}
