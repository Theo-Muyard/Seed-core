#include <stdio.h>
#include <stdlib.h>
#include "core/manager.h"
#include "core/dispatcher.h"
#include "systems/writing/system.h"

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

// +===----- Test Command Handlers -----===+ //

static bool	test_handler_success(t_Manager *manager, const t_Command *cmd)
{
	(void)manager;
	(void)cmd;
	return (true);
}

static bool	test_handler_failure(t_Manager *manager, const t_Command *cmd)
{
	(void)manager;
	(void)cmd;
	return (false);
}

// +===----- Test Cases -----===+ //

static int	test_dispatcher_init(void)
{
	t_Manager		*_manager;
	t_Dispatcher	*_dispatcher;

	print_section("DISPATCHER INITIALIZATION");

	_manager = malloc(sizeof(t_Manager));
	if (NULL == _manager)
		return (print_error("Failed to allocate manager"), 1);

	if (false == dispatcher_init(_manager, 10))
		return (print_error("Failed to initialize dispatcher with capacity 10"), 1);
	print_success("Dispatcher initialized with capacity 10");

	_dispatcher = _manager->dispatcher;
	printf("  %sCount%s: %zu\n", BLUE, WHITE, _dispatcher->count);
	printf("  %sCapacity%s: %zu\n", BLUE, WHITE, _dispatcher->capacity);

	if (_dispatcher->count != 0)
		return (print_error("Initial count should be 0"), 1);
	print_success("Initial count is 0");

	if (_dispatcher->capacity != 10)
		return (print_error("Capacity should be 10"), 1);
	print_success("Capacity is correctly set to 10");

	if (NULL == _dispatcher->commands)
		return (print_error("Commands array should be allocated"), 1);
	print_success("Commands array allocated");

	dispatcher_clean(_dispatcher);
	free(_manager);
	return (0);
}

static int	test_dispatcher_register(void)
{
	t_Manager		*_manager;
	t_Dispatcher	*_dispatcher;

	print_section("COMMAND REGISTRATION");

	_manager = malloc(sizeof(t_Manager));
	if (NULL == _manager)
		return (print_error("Failed to allocate manager"), 1);

	if (false == dispatcher_init(_manager, 5))
		return (print_error("Failed to initialize dispatcher"), 1);

	_dispatcher = _manager->dispatcher;

	if (false == dispatcher_register(_dispatcher, CMD_WRITING_CREATE_BUFFER, test_handler_success))
		return (print_error("Failed to register first command"), 1);
	print_success("Registered CMD_WRITING_CREATE_BUFFER");

	if (_dispatcher->count != 1)
		return (print_error("Count should be 1 after registration"), 1);
	print_success("Count incremented to 1");

	if (false == dispatcher_register(_dispatcher, CMD_WRITING_DELETE_BUFFER, test_handler_success))
		return (print_error("Failed to register second command"), 1);
	print_success("Registered CMD_WRITING_DELETE_BUFFER");

	printf("  %sTotal registered commands%s: %zu\n", BLUE, WHITE, _dispatcher->count);

	if (true == dispatcher_register(_dispatcher, CMD_WRITING_DELETE_LINE, NULL))
		return (print_error("Should reject NULL function pointer"), 1);
	print_success("Correctly rejected NULL function pointer");

	dispatcher_clean(_dispatcher);
	free(_manager);
	return (0);
}

static int	test_dispatcher_exec(void)
{
	t_Manager		*_manager;
	t_Dispatcher	*_dispatcher;
	t_Command		_cmd;

	print_section("COMMAND EXECUTION");

	_manager = malloc(sizeof(t_Manager));
	if (NULL == _manager)
		return (print_error("Failed to allocate manager"), 1);

	if (false == dispatcher_init(_manager, 10))
		return (print_error("Failed to initialize dispatcher"), 1);

	_dispatcher = _manager->dispatcher;

	if (false == dispatcher_register(_dispatcher, CMD_WRITING_CREATE_BUFFER, test_handler_success))
		return (print_error("Failed to register command"), 1);

	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = NULL;
	_cmd.payload_size = 0;

	if (false == dispatcher_exec(_manager, &_cmd))
		return (print_error("Failed to execute registered command"), 1);
	print_success("Executed CMD_WRITING_CREATE_BUFFER successfully");

	if (false == dispatcher_register(_dispatcher, CMD_WRITING_DELETE_BUFFER, test_handler_failure))
		return (print_error("Failed to register failure handler"), 1);

	_cmd.id = CMD_WRITING_DELETE_BUFFER;
	if (true == dispatcher_exec(_manager, &_cmd))
		return (print_error("Should propagate handler failure"), 1);
	print_success("Correctly propagated handler failure");

	_cmd.id = CMD_WRITING_INSERT_LINE;
	if (true == dispatcher_exec(_manager, &_cmd))
		return (print_error("Should fail on unknown command"), 1);
	print_success("Correctly failed on unregistered command");

	if (true == dispatcher_exec(NULL, &_cmd))
		return (print_error("Should fail with NULL manager"), 1);
	print_success("Correctly rejected NULL manager");

	dispatcher_clean(_dispatcher);
	free(_manager);
	return (0);
}

static int	test_dispatcher_clean(void)
{
	t_Manager		*_manager;
	t_Dispatcher	*_dispatcher;

	print_section("DISPATCHER CLEANUP");

	_manager = malloc(sizeof(t_Manager));
	if (NULL == _manager)
		return (print_error("Failed to allocate manager"), 1);

	if (false == dispatcher_init(_manager, 5))
		return (print_error("Failed to initialize dispatcher"), 1);

	_dispatcher = _manager->dispatcher;

	dispatcher_register(_dispatcher, CMD_WRITING_CREATE_BUFFER, test_handler_success);
	dispatcher_register(_dispatcher, CMD_WRITING_DELETE_BUFFER, test_handler_success);

	printf("  %sCommands registered%s: %zu\n", BLUE, WHITE, _dispatcher->count);

	dispatcher_clean(_dispatcher);
	print_success("Dispatcher cleaned successfully");
	print_success("Dispatcher memory freed");

	free(_manager);
	return (0);
}

int	main(void)
{
	int	_status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║       DISPATCHER TEST SUITE        ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	_status = 0;
	_status |= test_dispatcher_init();
	_status |= test_dispatcher_register();
	_status |= test_dispatcher_exec();
	_status |= test_dispatcher_clean();

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
