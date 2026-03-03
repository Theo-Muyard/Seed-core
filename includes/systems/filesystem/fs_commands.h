/**
 * @file "systems/filesystem/fs_commands.h"
 * 
 * @brief  Manage the filesystem commands. These are
 * 		the functions that are executed according to the command
 * 		given by the client.
 * 
 * @ingroup "filesystem"
*/

#ifndef SEED_FILESYSTEM_COMMANDS_H
# define SEED_FILESYSTEM_COMMANDS_H

# include "core/manager.h"

// +===----- Root functions -----===+ //

/**
 * @brief Open the root directory.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_root_open(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Close the root directory.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_root_close(t_Manager *manager, const t_Command *cmd);

// +===----- Directory functions -----===+ //

/**
 * @brief Creates a new directory.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_directory_create(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Create a directory.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_directory_delete(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Move a directory.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_directory_move(t_Manager *manager, const t_Command *cmd);

// +===----- File functions -----===+ //

/**
 * @brief Creates a new file.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_file_create(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Delete a file.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_file_delete(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Move a file.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_file_move(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Read a file.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_file_read(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Overwrites the contents of a file.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_file_write(t_Manager *manager, const t_Command *cmd);

#endif