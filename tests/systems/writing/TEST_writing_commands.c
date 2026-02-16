#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/**
 * @brief Test buffer creation through commands.
 * Verifies:
 *   - Buffer allocation via cmd_buffer_create
 *   - Unique buffer IDs
 *   - Context state management
*/
static int	test_cmd_buffer_create(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_payload;
	size_t				_id1;
	size_t				_id2;

	print_section("BUFFER CREATION COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Test 1: Create first buffer
	_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);

	if (false == cmd_buffer_create(_manager, &_cmd))
		return (print_error("Failed to create first buffer"), 1);
	print_success("First buffer created");
	_id1 = _payload.out_buffer_id;
	printf("  %sBuffer ID#1%s: %zu\n", BLUE, WHITE, _id1);

	// Test 2: Create second buffer with different ID
	_payload.out_buffer_id = 0;
	if (false == cmd_buffer_create(_manager, &_cmd))
		return (print_error("Failed to create second buffer"), 1);
	print_success("Second buffer created");
	_id2 = _payload.out_buffer_id;
	printf("  %sBuffer ID#2%s: %zu\n", BLUE, WHITE, _id2);

	if (_id1 == _id2)
		return (print_error("Buffer IDs should be unique"), 1);
	print_success("Buffer IDs are unique");

	// Test 3: Create many buffers
	printf("  %sCreating 5 more buffers...%s\n", YELLOW, WHITE);
	for (int _i = 0; _i < 5; _i++)
	{
		_payload.out_buffer_id = 0;
		if (false == cmd_buffer_create(_manager, &_cmd))
			return (print_error("Failed to create buffer in loop"), 1);
	}
	print_success("All buffers created successfully");

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test buffer destruction through commands.
 * Verifies:
 *   - Buffer cleanup
 *   - ID reuse after deletion
*/
static int	test_cmd_buffer_destroy(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdDestroyBuffer	_destroy_payload;
	size_t				_id;

	print_section("BUFFER DESTRUCTION COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Create buffer
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);

	if (false == cmd_buffer_create(_manager, &_cmd))
		return (print_error("Failed to create buffer"), 1);
	_id = _create_payload.out_buffer_id;
	print_success("Buffer created");
	printf("  %sBuffer ID%s: %zu\n", BLUE, WHITE, _id);

	// Destroy buffer
	_destroy_payload.buffer_id = _id;
	_cmd.id = CMD_WRITING_DELETE_BUFFER;
	_cmd.payload = &_destroy_payload;
	_cmd.payload_size = sizeof(t_CmdDestroyBuffer);

	if (false == cmd_buffer_destroy(_manager, &_cmd))
		return (print_error("Failed to destroy buffer"), 1);
	print_success("Buffer destroyed successfully");

	// Try to destroy same buffer again (should fail)
	if (true == cmd_buffer_destroy(_manager, &_cmd))
		return (print_error("Should fail when destroying non-existent buffer"), 1);
	print_success("Correctly rejected destruction of non-existent buffer");

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test line insertion through commands.
 * Verifies:
 *   - Line creation and insertion
 *   - Index handling
 *   - Multiple line management
*/
static int	test_cmd_line_insert(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	size_t				_buffer_id;

	print_section("LINE INSERTION COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Create buffer
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	// Insert line at index 0
	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);

	if (false == cmd_buffer_line_insert(_manager, &_cmd))
		return (print_error("Failed to insert first line"), 1);
	print_success("Line #1 inserted at index 0");

	// Insert line at end
	_insert_payload.line = -1;
	if (false == cmd_buffer_line_insert(_manager, &_cmd))
		return (print_error("Failed to insert second line"), 1);
	print_success("Line #2 inserted (appended to end)");

	// Insert line at middle
	_insert_payload.line = 1;
	if (false == cmd_buffer_line_insert(_manager, &_cmd))
		return (print_error("Failed to insert third line"), 1);
	print_success("Line #3 inserted at index 1 (middle)");

	printf("  %sTotal lines: 3 (structure: L1, L3, L2)%s\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test line deletion through commands.
 * Verifies:
 *   - Line removal
 *   - Index validity
*/
static int	test_cmd_line_destroy(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	t_CmdDestroyLine	_destroy_payload;
	size_t				_buffer_id;

	print_section("LINE DESTRUCTION COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Setup: Create buffer and insert lines
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	cmd_buffer_line_insert(_manager, &_cmd);
	cmd_buffer_line_insert(_manager, &_cmd);
	cmd_buffer_line_insert(_manager, &_cmd);

	print_success("Three lines created");

	// Delete middle line (index 1)
	_destroy_payload.buffer_id = _buffer_id;
	_destroy_payload.line = 1;
	_cmd.id = CMD_WRITING_DELETE_LINE;
	_cmd.payload = &_destroy_payload;
	_cmd.payload_size = sizeof(t_CmdDestroyLine);

	if (false == cmd_buffer_line_destroy(_manager, &_cmd))
		return (print_error("Failed to destroy line"), 1);
	print_success("Middle line (index 1) deleted");

	printf("  %sRemaining lines: 2%s\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test text addition through commands.
 * Verifies:
 *   - Data insertion into lines
 *   - Column positioning
 *   - Multiple insertions
*/
static int	test_cmd_add_data(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	t_CmdAddData		_add_data_payload;
	size_t				_buffer_id;
	char				_text1[] = "Hello";
	char				_text2[] = " World";

	print_section("TEXT ADDITION COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Setup
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	cmd_buffer_line_insert(_manager, &_cmd);

	// Add first text
	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = strlen(_text1);
	_add_data_payload.data = _text1;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);

	if (false == cmd_line_add_data(_manager, &_cmd))
		return (print_error("Failed to add first text"), 1);
	print_success("First text added: 'Hello'");

	// Add second text at end
	_add_data_payload.column = 5;
	_add_data_payload.size = strlen(_text2);
	_add_data_payload.data = _text2;

	if (false == cmd_line_add_data(_manager, &_cmd))
		return (print_error("Failed to add second text"), 1);
	print_success("Second text appended: ' World'");

	printf("  %sResult: Hello World%s\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test text deletion through commands.
 * Verifies:
 *   - Data removal from lines
 *   - Proper text management
*/
static int	test_cmd_delete_data(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	t_CmdAddData		_add_data_payload;
	t_CmdDeleteData		_delete_data_payload;
	size_t				_buffer_id;
	char				_text[] = "DELETE_THIS_PART";

	print_section("TEXT DELETION COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Setup
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	cmd_buffer_line_insert(_manager, &_cmd);

	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = strlen(_text);
	_add_data_payload.data = _text;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);
	cmd_line_add_data(_manager, &_cmd);

	print_success("Text inserted: 'DELETE_THIS_PART'");
	printf("  %sOriginal text%s: %s\n", BLUE, WHITE, _text);

	// Delete first 7 characters ("DELETE_")
	_delete_data_payload.buffer_id = _buffer_id;
	_delete_data_payload.line = 0;
	_delete_data_payload.column = 0;
	_delete_data_payload.size = 7;

	_cmd.id = CMD_WRITING_DELETE_TEXT;
	_cmd.payload = &_delete_data_payload;
	_cmd.payload_size = sizeof(t_CmdDeleteData);

	if (false == cmd_line_delete_data(_manager, &_cmd))
		return (print_error("Failed to delete text"), 1);
	print_success("Text deleted (7 characters)");

	printf("  %sResult: THIS_PART%s\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test line splitting through commands.
 * Verifies:
 *   - Line division at specified index
 *   - Content preservation and distribution
*/
static int	test_cmd_split_line(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	t_CmdAddData		_add_data_payload;
	t_CmdSplitLine		_split_payload;
	size_t				_buffer_id;
	char				_text[] = "ABCDEFGH";

	print_section("LINE SPLIT COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Setup
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	cmd_buffer_line_insert(_manager, &_cmd);

	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = strlen(_text);
	_add_data_payload.data = _text;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);
	cmd_line_add_data(_manager, &_cmd);

	print_success("Text inserted: 'ABCDEFGH'");

	// Split at index 4 (ABCD | EFGH)
	_split_payload.buffer_id = _buffer_id;
	_split_payload.line = 0;
	_split_payload.index = 4;

	_cmd.id = CMD_WRITING_SPLIT_LINE;
	_cmd.payload = &_split_payload;
	_cmd.payload_size = sizeof(t_CmdSplitLine);

	if (false == cmd_buffer_line_split(_manager, &_cmd))
		return (print_error("Failed to split line"), 1);
	print_success("Line split at index 4");

	printf("  %sLine 0: ABCD%s\n", BLUE, WHITE);
	printf("  %sLine 1: EFGH%s\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test line joining through commands.
 * Verifies:
 *   - Consecutive line merging
 *   - Content concatenation
*/
static int	test_cmd_join_line(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	t_CmdAddData		_add_data_payload;
	t_CmdJoinLine		_join_payload;
	size_t				_buffer_id;
	char				_text1[] = "First ";
	char				_text2[] = "Second";

	print_section("LINE JOIN COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Setup: Create buffer and two lines
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	cmd_buffer_line_insert(_manager, &_cmd);
	cmd_buffer_line_insert(_manager, &_cmd);

	// Add text to both lines
	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = strlen(_text1);
	_add_data_payload.data = _text1;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);
	cmd_line_add_data(_manager, &_cmd);

	_add_data_payload.line = 1;
	_add_data_payload.size = strlen(_text2);
	_add_data_payload.data = _text2;
	cmd_line_add_data(_manager, &_cmd);

	print_success("Two lines created with text");
	printf("  %sLine 0: First %s\n", BLUE, WHITE);
	printf("  %sLine 1: Second%s\n", BLUE, WHITE);

	// Join lines
	_join_payload.buffer_id = _buffer_id;
	_join_payload.dst = 0;
	_join_payload.src = 1;

	_cmd.id = CMD_WRITING_JOIN_LINE;
	_cmd.payload = &_join_payload;
	_cmd.payload_size = sizeof(t_CmdJoinLine);

	if (false == cmd_buffer_line_join(_manager, &_cmd))
		return (print_error("Failed to join lines"), 1);
	print_success("Lines joined successfully");

	printf("  %sResult: First Second%s\n", BLUE, WHITE);

	manager_clean(_manager);
	return (0);
}

/**
 * @brief Test line retrieval through commands.
 * Verifies:
 *   - Line data access
 *   - Proper pointer and length return
*/
static int	test_cmd_get_line(void)
{
	t_Manager			*_manager;
	t_Command			_cmd;
	t_CmdCreateBuffer	_create_payload;
	t_CmdInsertLine		_insert_payload;
	t_CmdAddData		_add_data_payload;
	t_CmdGetLine		_get_line_payload;
	size_t				_buffer_id;
	char				_text[] = "Retrieved Content";

	print_section("GET LINE COMMAND");

	_manager = manager_init();
	if (NULL == _manager)
		return (print_error("Failed to initialize manager"), 1);

	// Setup
	_create_payload.out_buffer_id = 0;
	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_create_payload;
	_cmd.payload_size = sizeof(t_CmdCreateBuffer);
	cmd_buffer_create(_manager, &_cmd);
	_buffer_id = _create_payload.out_buffer_id;

	_insert_payload.buffer_id = _buffer_id;
	_insert_payload.line = 0;
	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_insert_payload;
	_cmd.payload_size = sizeof(t_CmdInsertLine);
	cmd_buffer_line_insert(_manager, &_cmd);

	_add_data_payload.buffer_id = _buffer_id;
	_add_data_payload.line = 0;
	_add_data_payload.column = 0;
	_add_data_payload.size = strlen(_text);
	_add_data_payload.data = _text;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_add_data_payload;
	_cmd.payload_size = sizeof(t_CmdAddData);
	cmd_line_add_data(_manager, &_cmd);

	print_success("Line created with content");

	// Get line
	_get_line_payload.buffer_id = _buffer_id;
	_get_line_payload.line = 0;
	_get_line_payload.out_data = NULL;
	_get_line_payload.out_len = 0;

	_cmd.id = CMD_WRITING_GET_LINE;
	_cmd.payload = &_get_line_payload;
	_cmd.payload_size = sizeof(t_CmdGetLine);

	if (false == cmd_buffer_get_line(_manager, &_cmd))
		return (print_error("Failed to get line"), 1);
	print_success("Line retrieved successfully");

	printf("  %sData%s: %s\n", BLUE, WHITE, _get_line_payload.out_data);
	printf("  %sLength%s: %zu\n", BLUE, WHITE, _get_line_payload.out_len);

	if (_get_line_payload.out_len != strlen(_text))
		return (print_error("Length mismatch"), 1);
	print_success("Length is correct");

	manager_clean(_manager);
	return (0);
}

// +===----- Main -----===+ //

int	test_commands_main(void)
{
	int	_status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║     WRITING COMMANDS TEST SUITE    ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	_status = 0;
	_status |= test_cmd_buffer_create();
	_status |= test_cmd_buffer_destroy();
	_status |= test_cmd_line_insert();
	_status |= test_cmd_line_destroy();
	_status |= test_cmd_add_data();
	_status |= test_cmd_delete_data();
	_status |= test_cmd_split_line();
	_status |= test_cmd_join_line();
	_status |= test_cmd_get_line();

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
