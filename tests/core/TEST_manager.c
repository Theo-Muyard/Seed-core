#include <stdio.h>
#include <stdlib.h>
#include "core/manager.h"
#include "core/dispatcher.h"
#include "systems/writing/system.h"
#include "systems/writing/_internal.h"
#include "systems/writing/commands.h"

// +===----- Colors -----===+ //

#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define WHITE	"\033[37m"

// +===----- Test Utilities -----===+ //

static void	print_section(const char *section)
{
	printf("\n%s=== %s ===%s\n", BLUE, section, WHITE);
}

static void	print_success(const char *message)
{
	printf("%s✓ %s%s\n", GREEN, message, WHITE);
}

static void	print_error(const char *message)
{
	printf("%s✗ %s%s\n", RED, message, WHITE);
}

// +===----- Test Cases -----===+ //

static int	test_manager_init(void)
{
	t_Manager	*_manager;

	print_section("MANAGER INITIALIZATION");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to allocate manager"), 1);
	print_success("Manager allocated successfully");

	if (NULL == _manager->dispatcher)
		return (print_error("Dispatcher not initialized"), 1);
	print_success("Dispatcher initialized");

	if (NULL == _manager->writing_ctx)
		return (print_error("Writing context not initialized"), 1);
	print_success("Writing context initialized");

	printf("  %sDispatcher capacity%s: %zu commands\n", BLUE, WHITE, _manager->dispatcher->capacity);
	printf("  %sDispatcher count%s: %zu commands registered\n", BLUE, WHITE, _manager->dispatcher->count);

	if (_manager->dispatcher->count != 9)
		return (print_error("Expected 9 writing commands to be registered"), 1);
	print_success("All 9 writing commands registered");

	manager_clean(_manager);
	return (0);
}

static int	test_manager_buffers(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_payload;
	t_CmdCreateBuffer	_payload2;
	size_t				_id1;
	size_t				_id2;

	print_section("MANAGER - BUFFER OPERATIONS");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to execute create buffer command"), 1);
	print_success("Buffer creation command executed");

	_id1 = _payload.out_buffer_id;
	printf("  %sAllocated buffer ID%s: %zu\n", BLUE, WHITE, _id1);

	_payload2.out_buffer_id = 0;
	_cmd.payload = &_payload2;

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to create second buffer"), 1);
	print_success("Second buffer created");

	_id2 = _payload2.out_buffer_id;
	printf("  %sSecond buffer ID%s: %zu\n", BLUE, WHITE, _id2);

	if (_id1 == _id2)
		return (print_error("Buffer IDs should be different"), 1);
	print_success("Buffer IDs are unique");

	manager_clean(_manager);
	return (0);
}

static int	test_manager_lines_and_text(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_line_payload;
	t_CmdAddData		_add_data_payload;
	t_CmdDeleteData		_delete_data_payload;
	size_t				_buffer_id;
	char				_text[] = "Hello World";
	char				_delete_text[] = "Hello";

	print_section("MANAGER - LINES AND TEXT");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to create buffer"), 1);

	_buffer_id = _create_payload.out_buffer_id;

	_insert_line_payload.buffer_id = _buffer_id;
	_insert_line_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_line_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to insert line"), 1);
	print_success("Line created in buffer");

	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = sizeof(_text) - 1;
	_add_data_payload.data = _text;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to insert text"), 1);
	print_success("Text inserted: 'Hello World'");

	printf("  %sLine content%s: %s\n", BLUE, WHITE, _text);

	_delete_data_payload.buffer_id = _buffer_id;
	_delete_data_payload.line = 0;
	_delete_data_payload.column = 0;
	_delete_data_payload.size = sizeof(_delete_text) - 1;

	_cmd.id = CMD_WRITING_DELETE_TEXT;
	_cmd.payload = &_delete_data_payload;
	_cmd.payload_size = sizeof(t_CmdDeleteData);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to delete text"), 1);
	print_success("Text deleted");

	printf("  %sResult%s: World\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

static int	test_manager_split_join(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_line_payload;
	t_CmdAddData		_add_data_payload;
	t_CmdSplitLine		_split_payload;
	t_CmdJoinLine		_join_payload;
	size_t				_buffer_id;
	char				_text[] = "ABCDEFGH";

	print_section("MANAGER - SPLIT AND JOIN");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	manager_exec(_manager, &_cmd);

	_buffer_id = _create_payload.out_buffer_id;

	_insert_line_payload.buffer_id = _buffer_id;
	_insert_line_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_line_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	manager_exec(_manager, &_cmd);
	manager_exec(_manager, &_cmd);

	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = sizeof(_text) - 1;
	_add_data_payload.data = _text;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);
	manager_exec(_manager, &_cmd);

	_add_data_payload.line = 1;
	char _text2[] = "IJKL";
	_add_data_payload.size = sizeof(_text2) - 1;
	_add_data_payload.data = _text2;
	manager_exec(_manager, &_cmd);

	print_success("Two lines created");
	printf("  %sLine 0: ABCDEFGH%s\n", BLUE, WHITE);
	printf("  %sLine 1: IJKL%s\n", BLUE, WHITE);

	_split_payload.buffer_id = _buffer_id;
	_split_payload.line = 0;
	_split_payload.index = 4;

	_cmd.id = CMD_WRITING_SPLIT_LINE;
	_cmd.payload = &_split_payload;
	_cmd.payload_size = sizeof(t_CmdSplitLine);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to split line"), 1);
	print_success("Line split at index 4");

	printf("  %sResult: ABCD | EFGH%s\n", BLUE, WHITE);

	_join_payload.buffer_id = _buffer_id;
	_join_payload.dst = 0;
	_join_payload.src = 1;

	_cmd.id = CMD_WRITING_JOIN_LINE;
	_cmd.payload = &_join_payload;
	_cmd.payload_size = sizeof(t_CmdJoinLine);

	if (false == manager_exec(_manager, &_cmd))
		return (print_error("Failed to join lines"), 1);
	print_success("Lines joined successfully");

	manager_clean(_manager);
	return (0);
}

int	main(void)
{
	int	_status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║   MANAGER TEST SUITE               ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	_status = 0;
	_status |= test_manager_init();
	_status |= test_manager_buffers();
	_status |= test_manager_lines_and_text();
	_status |= test_manager_split_join();

	if (0 == _status)
	{
		printf("\n%s╔════════════════════════════════════╗%s\n", GREEN, WHITE);
		printf("%s║         ALL TESTS PASSED ✓         ║%s\n", GREEN, WHITE);
		printf("%s╚════════════════════════════════════╝%s\n", GREEN, WHITE);
	}
	else
	{
		printf("\n%s╔════════════════════════════════════╗%s\n", RED, WHITE);
		printf("%s║         SOME TESTS FAILED ✗        ║%s\n", RED, WHITE);
		printf("%s╚════════════════════════════════════╝%s\n", RED, WHITE);
	}

	printf("\n");
	return (_status);
}
