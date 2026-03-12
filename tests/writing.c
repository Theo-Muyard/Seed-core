#include "tests/commands.h"

#include "core/manager.h"
#include "systems/writing/writing_system.h"

#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define WHITE	"\033[37m"
#define RESET	"\033[0m"

#define print_error_code(msg) \
	do { printf("%sError:%s %s\n", RED, RESET, msg); } while (0)

#define print_section(msg) \
	do { printf("\n%s=== %s ===%s\n", BLUE, msg, WHITE); } while (0)

#define print_success(msg) \
	do { printf("%s✓ %s%s\n", GREEN, msg, RESET); } while (0)

#define print_error(msg) \
	do { printf("%s✗ %s%s\n", RED, msg, RESET); } while (0)

#define RETURN_IF_ERR(code) \
	do { if (code) return (handle_errors(code)); } while (0)


static int	handle_errors(t_ErrorCode code)
{
	if (code == 0)
		return (0);

	if (code == 1)
		print_error_code("Internal memory.");
	else if (code == 2)
		print_error_code("Operation failed.");
	else if (code == 3)
		print_error_code("Permission denied.");
	else if (code == 4)
		print_error_code("Invalid manager.");
	else if (code == 5)
		print_error_code("Invalid payload.");
	else if (code == 6)
		print_error_code("Invalid command.");
	else if (code == 7)
		print_error_code("Invalid command ID.");
	else if (code == 8)
		print_error_code("Dispatcher not initialized.");
	else if (code == 9)
		print_error_code("Writing context not initialized.");
	else if (code == 10)
		print_error_code("Filesystem context not initialized.");
	else if (code == 11)
		print_error_code("Buffer not found.");
	else if (code == 12)
		print_error_code("Line not found.");
	else if (code == 13)
		print_error_code("Folder not found.");
	else if (code == 14)
		print_error_code("Folder access denied.");
	else if (code == 15)
		print_error_code("Folder already exists.");
	else if (code == 16)
		print_error_code("File not found.");
	else if (code == 17)
		print_error_code("File access denied.");
	else if (code == 18)
		print_error_code("File already exists.");

	return (1);
}

int	test_buffer_command(t_Manager *manager)
{
	// t_ErrorCode	_err_code;	// The error code returned by manager

	/* CREATE COMMAND */
	// In this first test, we will create 5 buffers
	t_CmdCreateBuffer	_buffer_create_payload = {0};	// The payload does not require any optins

	for (size_t _i = 0; _i < 5; _i++)
	{
		RETURN_IF_ERR(test_buffer_create(manager, &_buffer_create_payload));

		if (_buffer_create_payload.out_buffer_id != _i)
			return (1);
	}

	print_success("Buffers created correctly.");

	/* DELETE COMMAND */
	RETURN_IF_ERR(test_buffer_destroy(manager, 2));

	// We check whether the buffer has been deleted
	if (manager->writing_ctx->buffers[2])
	{
		print_error("Buffers has not been deleted.");
		return (1);
	}

	print_success("Buffer deleted correctly.");

	t_ErrorCode	_err_code = test_buffer_destroy(manager, -4);
	if (!_err_code)
	{
		print_error("Invalid buffer should not pass.");
		return (1);
	}

	print_success("Invalid buffer does not pass.");
	return (0);
}

int	test_line_command(t_Manager *manager)
{
	t_CmdCreateBuffer	_buffer_payload = {0};

	RETURN_IF_ERR(test_buffer_create(manager, &_buffer_payload));

	size_t	_buffer_id = _buffer_payload.out_buffer_id;

	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));
	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));
	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));

	
	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 0, -1, 12,
			"First line!"
		)
	);
	
	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 1, -1, 13,
			"Second line!"
		)
	);

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 2, -1, 12,
			"Third line!"
		)
	);

	RETURN_IF_ERR(test_line_join(manager, _buffer_id, 0, 1));

	t_CmdGetLine	_line_get_payload = {
		.buffer_id = _buffer_id,
		.line = 0
	};

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));
	printf("DATA: %s\n", _line_get_payload.out_data);

	RETURN_IF_ERR(test_line_split(manager, _buffer_id, 0, 11));

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));
	printf("DATA: %s\n", _line_get_payload.out_data);

	_line_get_payload.line = 1;
	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));
	printf("DATA: %s\n", _line_get_payload.out_data);

	print_success("Lines created correctly.");

	return (0);
}

int	test_data_command(t_Manager *manager)
{
	t_CmdCreateBuffer	_buffer_payload = {0};

	RETURN_IF_ERR(test_buffer_create(manager, &_buffer_payload));

	size_t	_buffer_id = _buffer_payload.out_buffer_id;

	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 0, -1, 20,
			"Hello,\tWorld ! 🙂"
		)
	);

	t_CmdGetLine	_line_get_payload = {
		.buffer_id = _buffer_id,
		.line = 0
	};

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"Hello,\tWorld ! 🙂",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The data has not been written correctly.");
		return (1);
	}

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 0, -1, 23,
			" Second insertion here."
		)
	);

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"Hello,\tWorld ! 🙂 Second insertion here.",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The data has not been written correctly.");
		return (1);
	}

	print_success("Data writted correctly.");

	RETURN_IF_ERR(test_delete_data(manager, _buffer_id, 0, 14, 5));
	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"Hello,\tWorld ! Second insertion here.",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The data has not been written correctly.");
		return (1);
	}

	print_success("Data deleted correctly.");

	return (0);
}

int	main(void)
{
	t_Manager	*_manager = manager_init();
	RETURN_IF_NULL(_manager, 1);

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║     WRITING COMMANDS TEST SUITE    ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	int	_status = 0;

	print_section("BUFFER COMMANDS");
	_status |= test_buffer_command(_manager);

	print_section("LINE COMMANDS");
	_status |= test_line_command(_manager);

	print_section("DATA COMMANDS");
	_status |= test_data_command(_manager);

	manager_clean(_manager);
	return (0);
}
