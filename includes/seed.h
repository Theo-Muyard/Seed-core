#ifndef SEED_H
# define SEED_H

// +===----- Seed Backend Public API -----===+ //
// This header exposes only what the frontend needs

# include <sys/types.h>
# include <stdbool.h>

// +===----- Types -----===+ //

typedef struct s_Manager	t_Manager;

typedef enum	e_CommandId
{
	CMD_WRITING_CREATE_BUFFER,
	CMD_WRITING_DELETE_BUFFER,

	CMD_WRITING_INSERT_LINE,
	CMD_WRITING_DELETE_LINE,
	CMD_WRITING_SPLIT_LINE,
	CMD_WRITING_JOIN_LINE,
	CMD_WRITING_GET_LINE,

	CMD_WRITING_INSERT_TEXT,
	CMD_WRITING_DELETE_TEXT,
}	t_CommandId;

typedef struct s_Command
{
	t_CommandId		id;
	void			*payload;
	size_t			payload_size;
}	t_Command;

// +===----- Payload types -----===+ //

// +===----- Writing -----===+ //

typedef struct	s_CmdCreateBuffer
{
	size_t	out_buffer_id;
}	t_CmdCreateBuffer;

typedef struct	s_CmdDestroyBuffer
{
	size_t	buffer_id;
}	t_CmdDestroyBuffer;

typedef struct	s_CmdInsertLine
{
	size_t	buffer_id;
	size_t	line;
}	t_CmdInsertLine;

typedef struct	s_CmdDestroyLine
{
	size_t	buffer_id;
	size_t	line;
}	t_CmdDestroyLine;

typedef struct	s_CmdSplitLine
{
	size_t	buffer_id;
	size_t	line;
	size_t	index;
}	t_CmdSplitLine;

typedef struct	s_CmdJoinLine
{
	size_t	buffer_id;
	size_t	dst;
	size_t	src;
}	t_CmdJoinLine;

typedef struct	s_CmdGetLine
{
	size_t	buffer_id;
	size_t	line;
	const char	*out_data;
	size_t		out_len;
}	t_CmdGetLine;

typedef struct	s_CmdAddData
{
	size_t	buffer_id;
	size_t	line;
	size_t	column;
	size_t	size;
	char	*data;
}	t_CmdAddData;

typedef struct	s_CmdDeleteData
{
	size_t	buffer_id;
	size_t	line;
	size_t	column;
	size_t	size;
}	t_CmdDeleteData;

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
 * @brief Clean the seed core manager.
 * @param manager The manager.
 * @param cmd The command content.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		manager_exec(t_Manager *manager, t_Command *cmd);

#endif