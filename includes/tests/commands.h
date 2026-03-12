/**
 * @file "tests/commands.h"
 * 
 * @brief Includes easier-to-use commands for testing.
*/

#ifndef SEED_TESTS_COMMMANDS_H
# define SEED_TESTS_COMMMANDS_H

# include "seed.h"

// +===----- Functions -----===+ //

t_ErrorCode	test_buffer_create(t_Manager *manager, t_CmdCreateBuffer *payload);

t_ErrorCode	test_buffer_destroy(t_Manager *manager, size_t id);

t_ErrorCode	test_line_insert(t_Manager *manager, size_t buffer_id, ssize_t line);

t_ErrorCode	test_line_delete(t_Manager *manager, size_t buffer_id, ssize_t line);

t_ErrorCode	test_get_line(t_Manager *manager, t_CmdGetLine *payload);

t_ErrorCode	test_line_split(t_Manager *manager, size_t buffer_id, ssize_t line, size_t index);

t_ErrorCode	test_line_join(t_Manager *manager, size_t buffer_id, size_t dst, size_t src);

t_ErrorCode	test_insert_data(
	t_Manager *manager,
	size_t buffer_id, size_t line,
	size_t index, size_t size, const char *data
);

t_ErrorCode	test_delete_data(
	t_Manager *manager, size_t buffer_id,
	size_t line, size_t index, size_t size
);

#endif