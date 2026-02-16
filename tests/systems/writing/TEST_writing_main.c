#include <stdio.h>

// Forward declarations
int	test_internal_main(void);
int	test_commands_main(void);

int	main(void)
{
	int	_internal_status;
	int	_commands_status;

	_internal_status = test_internal_main();
	_commands_status = test_commands_main();

	printf("\n");
	printf("╔════════════════════════════════════╗\n");
	printf("║         FINAL TEST SUMMARY         ║\n");
	printf("╚════════════════════════════════════╝\n");

	if (0 == _internal_status)
		printf("✓ Internal tests: PASSED\n");
	else
		printf("✗ Internal tests: FAILED\n");

	if (0 == _commands_status)
		printf("✓ Commands tests: PASSED\n");
	else
		printf("✗ Commands tests: FAILED\n");

	printf("\n");

	return (_internal_status | _commands_status);
}
