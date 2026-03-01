#ifndef SEED_WRITING_COMMANDS_H
# define SEED_WRITING_COMMANDS_H

# include "core/manager.h"

// +===----- Buffer -----===+ //

/**
 * @brief Creates a new empty buffer.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_create(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Destroys the given buffer.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_destroy(t_Manager *manager, const t_Command *cmd);

// +===----- Lines -----===+ //

/**
 * @brief Adds the line to the index of the buffer.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_line_insert(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Delete the given line.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_line_delete(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Splits the given line in two lines.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_line_split(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Joins the givens line in one line.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_line_join(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Get the line of the given index.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_buffer_get_line(t_Manager *manager, const t_Command *cmd);

// +===----- Data -----===+ //

/**
 * @brief Insert the data to the given line.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_line_insert_data(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Delete the data to the given line.
 * @param manager The manager that will contains contexts.
 * @param cmd The content of the command.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	cmd_line_delete_data(t_Manager *manager, const t_Command *cmd);

#endif