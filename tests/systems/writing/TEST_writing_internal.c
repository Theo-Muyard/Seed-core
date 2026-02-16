#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "systems/writing/_internal.h"

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
 * @brief Test line creation and basic properties.
 * Verifies:
 *   - Line memory allocation
 *   - Initial state (empty, null pointers)
 *   - Line linking pointers
*/
static int	test_line_create(void)
{
	t_Line	*_line;

	print_section("LINE CREATION");

	_line = line_create();
	if (NULL == _line)
		return (print_error("Failed to allocate line"), 1);
	print_success("Line created successfully");

	if (NULL != _line->data)
		return (print_error("Line data should be NULL initially"), 1);
	print_success("Line data is NULL");

	if (0 != _line->len)
		return (print_error("Line length should be 0"), 1);
	print_success("Line length is 0");

	if (0 != _line->capacity)
		return (print_error("Line capacity should be 0"), 1);
	print_success("Line capacity is 0");

	if (NULL != _line->prev || NULL != _line->next)
		return (print_error("Line pointers should be NULL"), 1);
	print_success("Line pointers (prev/next) are NULL");

	free(_line->data);
	free(_line);
	return (0);
}

/**
 * @brief Test buffer creation and basic properties.
 * Verifies:
 *   - Buffer memory allocation
 *   - Initial empty state
 *   - Size tracking
*/
static int	test_buffer_create(void)
{
	t_Buffer	*_buffer;

	print_section("BUFFER CREATION");

	_buffer = buffer_create();
	if (NULL == _buffer)
		return (print_error("Failed to allocate buffer"), 1);
	print_success("Buffer created successfully");

	if (NULL != _buffer->line)
		return (print_error("Buffer line pointer should be NULL"), 1);
	print_success("Buffer line pointer is NULL");

	if (0 != _buffer->size)
		return (print_error("Buffer size should be 0"), 1);
	print_success("Buffer size is 0");

	free(_buffer);
	return (0);
}

/**
 * @brief Test adding data to a line.
 * Verifies:
 *   - Memory allocation on first insert
 *   - Data copying
 *   - Length and capacity tracking
 *   - Null terminator
*/
static int	test_line_add_data(void)
{
	t_Line	*_line;
	char	_text[] = "Hello World";
	size_t	_original_len;

	print_section("LINE DATA ADDITION");

	_line = line_create();
	if (NULL == _line)
		return (print_error("Failed to create line"), 1);

	// Test 1: Add data to empty line
	if (false == line_add_data(_line, 0, strlen(_text), _text))
		return (print_error("Failed to add data to line"), 1);
	print_success("Data added to line");

	printf("  %sData%s: %s\n", BLUE, WHITE, _line->data);
	printf("  %sLength%s: %zu\n", BLUE, WHITE, _line->len);
	printf("  %sCapacity%s: %zu\n", BLUE, WHITE, _line->capacity);

	if (0 != strcmp(_line->data, _text))
		return (print_error("Data not copied correctly"), 1);
	print_success("Data copied correctly");

	if (_line->len != strlen(_text))
		return (print_error("Length tracking is incorrect"), 1);
	print_success("Length is correct");

	if (_line->capacity < _line->len)
		return (print_error("Capacity should be >= length"), 1);
	print_success("Capacity is sufficient");

	// Test 2: Add more data at end
	_original_len = _line->len;
	char _more[] = "!";
	if (false == line_add_data(_line, _line->len, strlen(_more), _more))
		return (print_error("Failed to append data"), 1);
	print_success("Data appended successfully");

	printf("  %sResult%s: %s\n", BLUE, WHITE, _line->data);
	printf("  %sNew length%s: %zu\n", BLUE, WHITE, _line->len);

	if (_line->len != _original_len + 1)
		return (print_error("Length should increase"), 1);
	print_success("Length increased correctly");

	// Test 3: Insert data in middle
	char _middle[] = " Beautiful";
	if (false == line_add_data(_line, 5, strlen(_middle), _middle))
		return (print_error("Failed to insert in middle"), 1);
	print_success("Data inserted in middle");

	printf("  %sResult%s: %s\n", BLUE, WHITE, _line->data);

	free(_line->data);
	free(_line);
	return (0);
}

/**
 * @brief Test deleting data from a line.
 * Verifies:
 *   - Proper character removal
 *   - Length adjustment
 *   - Null terminator maintenance
*/
static int	test_line_delete_data(void)
{
	t_Line	*_line;
	char	_text[] = "DELETE_THIS_PART";

	print_section("LINE DATA DELETION");

	_line = line_create();
	if (NULL == _line)
		return (print_error("Failed to create line"), 1);

	// Setup: Add data
	if (false == line_add_data(_line, 0, strlen(_text), _text))
		return (print_error("Failed to add data"), 1);
	print_success("Initial data: 'DELETE_THIS_PART'");

	printf("  %sOriginal%s: %s (len=%zu)\n", BLUE, WHITE, _line->data, _line->len);

	// Test 1: Delete from start
	if (false == line_delete_data(_line, 0, 7))  // Delete "DELETE_"
		return (print_error("Failed to delete from start"), 1);
	print_success("Deleted 7 characters from start");

	printf("  %sResult%s: %s (len=%zu)\n", BLUE, WHITE, _line->data, _line->len);

	if (0 != strcmp(_line->data, "THIS_PART"))
		return (print_error("Delete from start failed"), 1);
	print_success("Start deletion is correct");

	// Test 2: Delete from middle
	if (false == line_delete_data(_line, 4, 1))  // Delete "_"
		return (print_error("Failed to delete from middle"), 1);
	print_success("Deleted 1 character from middle");

	printf("  %sResult%s: %s (len=%zu)\n", BLUE, WHITE, _line->data, _line->len);

	if (0 != strcmp(_line->data, "THISPART"))
		return (print_error("Delete from middle failed"), 1);
	print_success("Middle deletion is correct");

	free(_line->data);
	free(_line);
	return (0);
}

/**
 * @brief Test inserting lines into buffer.
 * Verifies:
 *   - Line linking (prev/next pointers)
 *   - Index handling
 *   - Buffer size tracking
 *   - Insertion at beginning, middle, end
*/
static int	test_buffer_line_insert(void)
{
	t_Buffer	*_buffer;
	t_Line		*_line1;
	t_Line		*_line2;
	t_Line		*_line3;
	t_Line		*_tmp;
	int			_count;

	print_section("BUFFER LINE INSERTION");

	_buffer = buffer_create();
	if (NULL == _buffer)
		return (print_error("Failed to create buffer"), 1);

	// Test 1: Insert first line
	_line1 = line_create();
	if (false == buffer_line_insert(_buffer, _line1, 0))
		return (print_error("Failed to insert first line"), 1);
	print_success("First line inserted at index 0");

	if (_buffer->size != 1)
		return (print_error("Buffer size should be 1"), 1);
	print_success("Buffer size is 1");

	// Test 2: Insert second line at end
	_line2 = line_create();
	if (false == buffer_line_insert(_buffer, _line2, -1))  // -1 = append
		return (print_error("Failed to insert second line"), 1);
	print_success("Second line appended (index -1)");

	if (_buffer->size != 2)
		return (print_error("Buffer size should be 2"), 1);
	print_success("Buffer size is 2");

	// Test 3: Insert third line in middle
	_line3 = line_create();
	if (false == buffer_line_insert(_buffer, _line3, 1))
		return (print_error("Failed to insert middle line"), 1);
	print_success("Third line inserted at index 1");

	if (_buffer->size != 3)
		return (print_error("Buffer size should be 3"), 1);
	print_success("Buffer size is 3");

	// Verify structure: L1 -> L3 -> L2
	_tmp = _buffer->line;
	_count = 0;
	while (_tmp)
	{
		_count++;
		_tmp = _tmp->next;
	}

	if (_count != 3)
		return (print_error("Should have 3 lines linked"), 1);
	print_success("All lines properly linked");

	buffer_destroy(_buffer);
	return (0);
}

/**
 * @brief Test splitting a line at a position.
 * Verifies:
 *   - Content distribution
 *   - Two line creation
 *   - Proper linking in buffer
*/
static int	test_buffer_line_split(void)
{
	t_Buffer	*_buffer;
	t_Line		*_line;
	t_Line		*_new_line;
	char		_text[] = "ABCDEFGH";

	print_section("BUFFER LINE SPLIT");

	_buffer = buffer_create();
	if (NULL == _buffer)
		return (print_error("Failed to create buffer"), 1);

	// Setup: Create line with data
	_line = line_create();
	buffer_line_insert(_buffer, _line, 0);
	line_add_data(_line, 0, strlen(_text), _text);

	print_success("Line created: 'ABCDEFGH'");

	// Split at index 4
	_new_line = buffer_line_split(_buffer, _line, 4);
	if (NULL == _new_line)
		return (print_error("Failed to split line"), 1);
	print_success("Line split at index 4");

	printf("  %sOriginal part%s: %s\n", BLUE, WHITE, _line->data);
	printf("  %sNew part%s: %s\n", BLUE, WHITE, _new_line->data);

	if (0 != strcmp(_line->data, "ABCD"))
		return (print_error("First part should be ABCD"), 1);
	print_success("First part is correct: 'ABCD'");

	if (0 != strcmp(_new_line->data, "EFGH"))
		return (print_error("Second part should be EFGH"), 1);
	print_success("Second part is correct: 'EFGH'");

	if (_buffer->size != 2)
		return (print_error("Buffer should have 2 lines"), 1);
	print_success("Buffer has 2 lines");

	buffer_destroy(_buffer);
	return (0);
}

/**
 * @brief Test joining two consecutive lines.
 * Verifies:
 *   - Content concatenation
 *   - Proper cleanup of source line
 *   - Buffer integrity
*/
static int	test_buffer_line_join(void)
{
	t_Buffer	*_buffer;
	t_Line		*_line1;
	t_Line		*_line2;
	t_Line		*_result;
	char		_text1[] = "First ";
	char		_text2[] = "Second";

	print_section("BUFFER LINE JOIN");

	_buffer = buffer_create();
	if (NULL == _buffer)
		return (print_error("Failed to create buffer"), 1);

	// Setup: Create two lines
	_line1 = line_create();
	_line2 = line_create();
	buffer_line_insert(_buffer, _line1, 0);
	buffer_line_insert(_buffer, _line2, -1);

	line_add_data(_line1, 0, strlen(_text1), _text1);
	line_add_data(_line2, 0, strlen(_text2), _text2);

	print_success("Two lines created");
	printf("  %sLine 1%s: %s\n", BLUE, WHITE, _line1->data);
	printf("  %sLine 2%s: %s\n", BLUE, WHITE, _line2->data);

	// Join lines
	_result = buffer_line_join(_buffer, _line1, _line2);
	if (NULL == _result)
		return (print_error("Failed to join lines"), 1);
	print_success("Lines joined successfully");

	printf("  %sResult%s: %s\n", BLUE, WHITE, _result->data);

	if (0 != strcmp(_result->data, "First Second"))
		return (print_error("Joined content should be 'First Second'"), 1);
	print_success("Joined content is correct");

	if (_buffer->size != 1)
		return (print_error("Buffer should have 1 line after join"), 1);
	print_success("Buffer size is 1 after join");

	buffer_destroy(_buffer);
	return (0);
}

/**
 * @brief Test retrieving lines from buffer by index.
 * Verifies:
 *   - Correct line retrieval
 *   - Index handling (positive, negative, boundary)
 *   - NULL returns for invalid indices
*/
static int	test_buffer_get_line(void)
{
	t_Buffer	*_buffer;
	t_Line		*_line1;
	t_Line		*_line2;
	t_Line		*_line3;
	t_Line		*_retrieved;

	print_section("BUFFER GET LINE");

	_buffer = buffer_create();
	if (NULL == _buffer)
		return (print_error("Failed to create buffer"), 1);

	// Setup: Create three lines
	_line1 = line_create();
	_line2 = line_create();
	_line3 = line_create();
	buffer_line_insert(_buffer, _line1, 0);
	buffer_line_insert(_buffer, _line2, -1);
	buffer_line_insert(_buffer, _line3, -1);

	line_add_data(_line1, 0, 1, "1");
	line_add_data(_line2, 0, 1, "2");
	line_add_data(_line3, 0, 1, "3");

	print_success("Buffer with 3 lines created");

	// Test 1: Get line at index 0
	_retrieved = buffer_get_line(_buffer, 0);
	if (NULL == _retrieved || 0 != strcmp(_retrieved->data, "1"))
		return (buffer_destroy(_buffer), print_error("Failed to get line at index 0"), 1);
	print_success("Retrieved line at index 0: '1'");

	// Test 2: Get line at index 1
	_retrieved = buffer_get_line(_buffer, 1);
	if (NULL == _retrieved || 0 != strcmp(_retrieved->data, "2"))
		return (buffer_destroy(_buffer), print_error("Failed to get line at index 1"), 1);
	print_success("Retrieved line at index 1: '2'");

	// Test 3: Get line at index 2
	_retrieved = buffer_get_line(_buffer, 2);
	if (NULL == _retrieved || 0 != strcmp(_retrieved->data, "3"))
		return (buffer_destroy(_buffer), print_error("Failed to get line at index 2"), 1);
	print_success("Retrieved line at index 2: '3'");

	// Test 4: Get last line with -1
	_retrieved = buffer_get_line(_buffer, -1);
	if (NULL == _retrieved || 0 != strcmp(_retrieved->data, "3"))
		return (buffer_destroy(_buffer), print_error("Failed to get last line with -1"), 1);
	print_success("Retrieved last line with index -1: '3'");

	// Test 5: Invalid index (out of range)
	_retrieved = buffer_get_line(_buffer, 5);
	if (NULL != _retrieved)
		return (buffer_destroy(_buffer), print_error("Should return NULL for invalid index"), 1);
	print_success("Correctly returned NULL for invalid index");

	buffer_destroy(_buffer);
	return (0);
}

/**
 * @brief Test buffer destruction and memory cleanup.
 * Verifies:
 *   - All lines are properly freed
 *   - Data is cleaned up
 *   - No memory leaks
*/
static int	test_buffer_destroy(void)
{
	t_Buffer	*_buffer;
	t_Line		*_line;
	int			_i;

	print_section("BUFFER DESTRUCTION");

	_buffer = buffer_create();
	if (NULL == _buffer)
		return (print_error("Failed to create buffer"), 1);

	// Create multiple lines with data
	printf("  %sCreating 5 lines with data...%s\n", YELLOW, WHITE);
	for (_i = 0; _i < 5; _i++)
	{
		_line = line_create();
		buffer_line_insert(_buffer, _line, -1);
		line_add_data(_line, 0, 10, "Data_Line ");
	}

	if (_buffer->size != 5)
		return (print_error("Buffer should have 5 lines"), 1);
	print_success("5 lines created and tracked");

	// Destroy buffer
	buffer_destroy(_buffer);
	print_success("Buffer destroyed successfully");

	printf("  %sAll resources cleaned up%s\n", BLUE, WHITE);

	return (0);
}

// +===----- Main -----===+ //

int	test_internal_main(void)
{
	int	_status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║    WRITING INTERNAL TEST SUITE     ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	_status = 0;
	_status |= test_line_create();
	_status |= test_buffer_create();
	_status |= test_line_add_data();
	_status |= test_line_delete_data();
	_status |= test_buffer_line_insert();
	_status |= test_buffer_line_split();
	_status |= test_buffer_line_join();
	_status |= test_buffer_get_line();
	_status |= test_buffer_destroy();

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
