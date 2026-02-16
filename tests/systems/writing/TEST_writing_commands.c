#include "tools.h"
#include "seed.h"

static int	create_buffer(t_Manager *manager, size_t *buffer_id)
{
	t_Command			cmd;
	t_CmdCreateBuffer	payload;
	t_ErrorCode			err;

	payload.out_buffer_id = 0;
	cmd.id = CMD_WRITING_CREATE_BUFFER;
	cmd.payload = &payload;
	err = manager_exec(manager, &cmd);
	if (ERR_SUCCESS != err)
		return (assert_error_code(err, ERR_SUCCESS, "Create buffer"));
	*buffer_id = payload.out_buffer_id;
	return (0);
}

static int	insert_line(t_Manager *manager, size_t buffer_id, ssize_t line)
{
	t_Command		cmd;
	t_CmdInsertLine	payload;

	payload.buffer_id = buffer_id;
	payload.line = line;
	cmd.id = CMD_WRITING_INSERT_LINE;
	cmd.payload = &payload;
	return (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Insert line"));
}

static int	insert_text(t_Manager *manager, size_t buffer_id, ssize_t line, ssize_t col, char *txt)
{
	t_Command		cmd;
	t_CmdInsertData	payload;

	payload.buffer_id = buffer_id;
	payload.line = line;
	payload.index = col;
	payload.size = strlen(txt);
	payload.data = txt;
	cmd.id = CMD_WRITING_INSERT_TEXT;
	cmd.payload = &payload;
	return (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Insert text"));
}

static int	test_buffer_commands(void)
{
	t_Manager			*manager;
	t_Command			cmd;
	t_CmdCreateBuffer	create_payload;
	t_CmdDestroyBuffer	destroy_payload;
	t_ErrorCode			err;

	print_section("WRITING BUFFER COMMANDS");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	create_payload.out_buffer_id = 0;
	cmd.id = CMD_WRITING_CREATE_BUFFER;
	cmd.payload = &create_payload;
	err = manager_exec(manager, &cmd);
	if (assert_error_code(err, ERR_SUCCESS, "Create first buffer"))
		return (manager_clean(manager), 1);
	destroy_payload.buffer_id = create_payload.out_buffer_id;
	cmd.id = CMD_WRITING_DELETE_BUFFER;
	cmd.payload = &destroy_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Delete existing buffer"))
		return (manager_clean(manager), 1);
	if (assert_error_code(manager_exec(manager, &cmd), ERR_BUFFER_NOT_FOUND, "Delete missing buffer rejected"))
		return (manager_clean(manager), 1);
	destroy_payload.buffer_id = 9999;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_BUFFER_NOT_FOUND, "Delete out-of-range buffer rejected"))
		return (manager_clean(manager), 1);
	manager_clean(manager);
	return (0);
}

static int	test_line_commands(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdDeleteLine	del_payload;
	t_CmdGetLine	get_payload;
	size_t			buffer_id;

	print_section("WRITING LINE COMMANDS");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	if (create_buffer(manager, &buffer_id))
		return (manager_clean(manager), 1);
	if (insert_line(manager, buffer_id, 0))
		return (manager_clean(manager), 1);
	if (insert_line(manager, buffer_id, -1))
		return (manager_clean(manager), 1);
	del_payload.buffer_id = buffer_id;
	del_payload.line = 1;
	cmd.id = CMD_WRITING_DELETE_LINE;
	cmd.payload = &del_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Delete existing line"))
		return (manager_clean(manager), 1);
	del_payload.line = 5;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_LINE_NOT_FOUND, "Delete missing line rejected"))
		return (manager_clean(manager), 1);
	get_payload.buffer_id = buffer_id;
	get_payload.line = 4;
	get_payload.out_data = NULL;
	get_payload.out_size = 0;
	cmd.id = CMD_WRITING_GET_LINE;
	cmd.payload = &get_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_LINE_NOT_FOUND, "Get missing line rejected"))
		return (manager_clean(manager), 1);
	manager_clean(manager);
	return (0);
}

static int	test_text_commands(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdGetLine	get_payload;
	t_CmdDeleteData	del_payload;
	t_CmdInsertData	ins_payload;
	size_t			buffer_id;
	char			msg[] = "HelloWorld";

	print_section("WRITING TEXT COMMANDS");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	if (create_buffer(manager, &buffer_id))
		return (manager_clean(manager), 1);
	if (insert_line(manager, buffer_id, 0))
		return (manager_clean(manager), 1);
	if (insert_text(manager, buffer_id, 0, 0, msg))
		return (manager_clean(manager), 1);

	del_payload.buffer_id = buffer_id;
	del_payload.line = 0;
	del_payload.index = 5;
	del_payload.size = 5;
	cmd.id = CMD_WRITING_DELETE_TEXT;
	cmd.payload = &del_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Delete text in line"))
		return (manager_clean(manager), 1);
	get_payload.buffer_id = buffer_id;
	get_payload.line = 0;
	get_payload.out_data = NULL;
	get_payload.out_size = 0;
	cmd.id = CMD_WRITING_GET_LINE;
	cmd.payload = &get_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Get modified line"))
		return (manager_clean(manager), 1);
	if (get_payload.out_size != 5 || 0 != strcmp(get_payload.out_data, "Hello"))
		return (manager_clean(manager), print_error("Line content mismatch after delete"), 1);
	print_success("Line content is correct after delete");
	ins_payload.buffer_id = buffer_id;
	ins_payload.line = 3;
	ins_payload.index = 0;
	ins_payload.size = strlen(msg);
	ins_payload.data = msg;
	cmd.id = CMD_WRITING_INSERT_TEXT;
	cmd.payload = &ins_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_LINE_NOT_FOUND, "Insert text rejected on missing line"))
		return (manager_clean(manager), 1);
	del_payload.line = 0;
	del_payload.index = 999;
	del_payload.size = 1;
	cmd.id = CMD_WRITING_DELETE_TEXT;
	cmd.payload = &del_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_OPERATION_FAILED, "Delete text rejected on invalid index"))
		return (manager_clean(manager), 1);
	manager_clean(manager);
	return (0);
}

static int	test_split_and_join_commands(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdSplitLine	split_payload;
	t_CmdJoinLine	join_payload;
	t_CmdGetLine	get_payload;
	size_t			buffer_id;
	char			msg[] = "ABCD1234";

	print_section("WRITING SPLIT/JOIN COMMANDS");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	if (create_buffer(manager, &buffer_id))
		return (manager_clean(manager), 1);
	if (insert_line(manager, buffer_id, 0))
		return (manager_clean(manager), 1);
	if (insert_line(manager, buffer_id, 1))
		return (manager_clean(manager), 1);
	if (insert_text(manager, buffer_id, 0, 0, msg))
		return (manager_clean(manager), 1);

	split_payload.buffer_id = buffer_id;
	split_payload.line = 0;
	split_payload.index = 4;
	cmd.id = CMD_WRITING_SPLIT_LINE;
	cmd.payload = &split_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Split line"))
		return (manager_clean(manager), 1);
	split_payload.index = 999;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_OPERATION_FAILED, "Split rejected with invalid index"))
		return (manager_clean(manager), 1);

	join_payload.buffer_id = buffer_id;
	join_payload.dst = 1;
	join_payload.src = 0;
	cmd.id = CMD_WRITING_JOIN_LINE;
	cmd.payload = &join_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_INVALID_PAYLOAD, "Join rejected for non-adjacent order"))
		return (manager_clean(manager), 1);
	join_payload.dst = 0;
	join_payload.src = 1;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Join adjacent lines"))
		return (manager_clean(manager), 1);

	get_payload.buffer_id = buffer_id;
	get_payload.line = 0;
	get_payload.out_data = NULL;
	get_payload.out_size = 0;
	cmd.id = CMD_WRITING_GET_LINE;
	cmd.payload = &get_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Get joined line"))
		return (manager_clean(manager), 1);
	if (0 != strcmp(get_payload.out_data, "ABCD1234"))
		return (manager_clean(manager), print_error("Joined content mismatch"), 1);
	print_success("Joined content is correct");
	manager_clean(manager);
	return (0);
}

int	test_commands_main(void)
{
	int	status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║     WRITING COMMANDS TEST SUITE    ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);
	status = 0;
	status |= test_buffer_commands();
	status |= test_line_commands();
	status |= test_text_commands();
	status |= test_split_and_join_commands();
	print_status(status);
	return (status);
}
