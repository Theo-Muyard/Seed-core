#ifndef SEED_H
# define SEED_H

// +===----- Seed Backend Public API -----===+ //
// This header exposes only what the frontend needs

# include "dependency.h"

// +===----- Types -----===+ //

/* The seed API manager */
typedef struct s_Manager	t_Manager;

/* Error codes for API manager */
typedef enum	e_ErrorCode
{
	ERR_SUCCESS = 0, /* Success */

	ERR_INTERNAL_MEMORY,	/* Internal memory error */
	ERR_OPERATION_FAILED,	/* Operation failed */
	ERR_PERMISSION_DENIED,	/* Permission denied */

	ERR_INVALID_MANAGER,	/* Invalid manager */
	ERR_INVALID_PAYLOAD,	/* Invalid payload content */
	ERR_INVALID_COMMAND,	/* Invalid command */
	ERR_INVALID_COMMAND_ID,	/* Invalid command ID*/

	ERR_DISPATCHER_NOT_INITIALIZED,	/* Dispatcher not initialized */
	ERR_WRITING_CONTEXT_NOT_INITIALIZED,	/* Writing context not initialized */
	ERR_FS_CONTEXT_NOT_INITIALIZED,	/* Filesystem context not initialized */

	/* +==-- Writing system errors --==+ */
	ERR_BUFFER_NOT_FOUND,	/* Buffer not found */
	ERR_LINE_NOT_FOUND,	/* Line not found */

	/* +==-- Filesystem errors --==+ */
	ERR_DIR_NOT_FOUND,	/* Directory not found */
	ERR_DIR_ACCESS,	/* Directory access denied */
	ERR_DIR_EXIST,	/* Directory aleady exist */
	ERR_FILE_NOT_FOUND,	/* File not found */
	ERR_FILE_ACCESS,	/* File access denied */
	ERR_FILE_EXIST	/* File already exist */
}	t_ErrorCode;

/* Command ID for API manager */
typedef enum	e_CommandId
{
	/* +==-- Writing system commands ID --==+ */
	CMD_WRITING_CREATE_BUFFER,	/* Create a buffer */
	CMD_WRITING_DELETE_BUFFER,	/* Delete a buffer */
	CMD_WRITING_INSERT_LINE,	/* Insert a line */
	CMD_WRITING_DELETE_LINE,	/* Delete a line */
	CMD_WRITING_SPLIT_LINE,	/* Split a line */
	CMD_WRITING_JOIN_LINE,	/* Join a line */
	CMD_WRITING_GET_LINE,	/* Get a line content */
	CMD_WRITING_INSERT_TEXT,	/* Insert text inside a line */
	CMD_WRITING_DELETE_TEXT,	/* Delete text inside a line */

	/* +==-- Filesystem commands ID --==+ */
	CMD_FS_OPEN_ROOT,	/* Open a root directory */
	CMD_FS_CLOSE_ROOT,	/* Close a root directory */
	CMD_FS_CREATE_DIR,	/* Create a directory */
	CMD_FS_DELETE_DIR,	/* Delete a directory */
	CMD_FS_MOVE_DIR,	/* Move a directory */
	CMD_FS_CREATE_FILE,	/* Create a file */
	CMD_FS_DELETE_FILE,	/* Delete a file */
	CMD_FS_READ_FILE,	/* Read text inside a file */
	CMD_FS_WRITE_FILE,	/* Write text inside a file */
	CMD_FS_MOVE_FILE	/* Move a file */
}	t_CommandId;

/* The command content for API manager */
typedef struct s_Command
{
	t_CommandId		id;	/* The command ID */
	void			*payload;	/* The payload content */
}	t_Command;

// +===----- Payload types -----===+ //
// All payload are unique for his command, make sure use the correct payload

/* +==-- Writing system --==+ */
// Payloads for the entire writing system

typedef struct	s_CmdCreateBuffer
{
	size_t	out_buffer_id;	/* The buffer ID that was be created */
}	t_CmdCreateBuffer;

typedef struct	s_CmdDestroyBuffer
{
	size_t	buffer_id;	/* The buffer ID */
}	t_CmdDestroyBuffer;

typedef struct	s_CmdInsertLine
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
}	t_CmdInsertLine;

typedef struct	s_CmdDeleteLine
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
}	t_CmdDeleteLine;

typedef struct	s_CmdSplitLine
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
	size_t	index;	/* The index */
}	t_CmdSplitLine;

typedef struct	s_CmdJoinLine
{
	size_t	buffer_id;	/* The buffer ID */
	size_t	dst;	/* The destination line */
	size_t	src;	/* The source line */
}	t_CmdJoinLine;

typedef struct	s_CmdGetLine
{
	size_t		buffer_id;	/* The buffer ID */
	ssize_t		line;	/* The line */
	const char	*out_data;	/* The data that was be readed */
	size_t		out_size;	/* The data size */
}	t_CmdGetLine;

typedef struct	s_CmdInsertData
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
	ssize_t	index;	/* The index */
	size_t	size;	/* The data size */
	char	*data;	/* The data content */
}	t_CmdInsertData;

typedef struct	s_CmdDeleteData
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
	size_t	index;	/* The index */
	size_t	size;	/* The length of data that will be deleted */
}	t_CmdDeleteData;

/* +==-- Filesystem payload --==+ */
// Payloads for the entire filesystem

typedef struct	s_CmdOpenRoot
{
	char	*path;	/* The absolute path of the directory */
}	t_CmdOpenRoot;

typedef struct	s_CmdCreateDir
{
	char	*path;	/* The relative path of the directory */
}	t_CmdCreateDir;

typedef struct	s_CmdDeleteDir
{
	char	*path;	/* The relative path of the directory */
}	t_CmdDeleteDir;

typedef struct	s_CmdMoveDir
{
	char	*old_path;	/* The old relative path of the directory */
	char	*new_path;	/* The new relative path of the directory */
}	t_CmdMoveDir;

typedef struct	s_CmdCreateFile
{
	char	*path;	/* The relative path of the file */
}	t_CmdCreateFile;

typedef struct	s_CmdDeleteFile
{
	char	*path;	/* The relative path of the file */
}	t_CmdDeleteFile;

typedef struct	s_CmdMoveFile
{
	char	*old_path;	/* The old relative path of the file */
	char	*new_path;	/* The new relative path of the file */
}	t_CmdMoveFile;

typedef struct	s_CmdReadFile
{
	char	*path;	/* The relative path of the file */
	char	*out_data;	/* The data that was be readed */
	size_t	out_len;	/* The length of the data */
}	t_CmdReadFile;

typedef struct	s_CmdWriteFile
{
	char	*path;	/* The relative path of the file */
	char	*data;	/* The data that will be writted */
}	t_CmdWriteFile;

// +===----- Functions -----===+ //

/**
 * @brief Initialize the seed core manager.
 * @return The manager who was created.
*/
t_Manager	*manager_init(void);

/**
 * @brief Clean the seed core manager.
 * @param manager The manager.
*/
void		manager_clean(t_Manager *manager);

/**
 * @brief Execute a command with the seed core manager.
 * @param manager The manager.
 * @param cmd The command content.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	manager_exec(t_Manager *manager, t_Command *cmd);

#endif