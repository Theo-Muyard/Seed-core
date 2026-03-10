#include "seed.h"

#include "core/manager.h"
#include "systems/writing/writing_system.h"

#define RED	"\033[31m"
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
	t_Command	_cmd;		// Our command
	t_ErrorCode	_err_code;	// The error code returned by manager

	/* CREATE COMMAND */
	// In this first test, we will create 5 buffers
	t_CmdCreateBuffer	_buffer_create_payload = {0};	// The payload does not require any optins

	_cmd.id = CMD_WRITING_CREATE_BUFFER;	// The command ID
	_cmd.payload = &_buffer_create_payload;	// We provide the adress of our payload so that the manager
						// can modify it.

	for (size_t _i = 0; _i < 5; _i++)
	{
		_err_code = manager_exec(manager, &_cmd); // We retrieve the return code

		// If there is an error, we display it and stop the test here
		if (_err_code)
			return (handle_errors(_err_code));

		// If the buffer ID is incorrect, then the test fails
		if (_buffer_create_payload.out_buffer_id != _i)
			return (1);
	}

	print_success("Buffers created correctly.");

	/* DELETE COMMAND */
	t_CmdDestroyBuffer	_buffer_destroy_payload = {
		.buffer_id = 2 // The ID of the buffer that will be destroyed
	};

	_cmd.id = CMD_WRITING_DELETE_BUFFER;
	_cmd.payload = &_buffer_destroy_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	// We check whether the buffer has been deleted
	if (manager->writing_ctx->buffers[2])
	{
		print_error("Buffers has not been deleted.");
		return (1);
	}

	print_success("Buffer deleted correctly.");

	_buffer_destroy_payload.buffer_id = -4;	// Invalid buffer

	_err_code = manager_exec(manager, &_cmd);
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
	(void)manager;
	return (0);
}

int	test_data_command(t_Manager *manager)
{
	(void)manager;
	return (0);
}

int	main(void)
{
	t_Manager	*_manager = manager_init();
	RETURN_IF_NULL(_manager, 1);

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║     WRITING COMMANDS TEST SUITE    ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	print_section("BUFFERS COMMANDS");

	int	_status = 0;

	_status |= test_buffer_command(_manager);
	_status |= test_line_command(_manager);
	_status |= test_data_command(_manager);

	manager_clean(_manager);
	return (0);

	/* GOTO EXIT */
	exit_free_manager:
		return (manager_clean(_manager), 1);
}
