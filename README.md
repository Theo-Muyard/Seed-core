# Seed Core v0.2 - API Documentation

> High-performance backend core for writing and filesystem operations, driven by a command-based API.

## Table of Contents

- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
- [API Reference](#api-reference)
- [Command System](#command-system)
- [Writing Commands](#writing-commands)
- [Filesystem Commands](#filesystem-commands)
- [Error Handling](#error-handling)
- [Changelog](#changelog)

---

## Quick Start

```c
#include "seed.h"

int main(void)
{
	t_Manager *manager = manager_init();
	if (!manager)
		return (1);

	t_CmdCreateBuffer payload = {0};
	t_Command cmd = {
		.id = CMD_WRITING_CREATE_BUFFER,
		.payload = &payload
	};

	if (manager_exec(manager, &cmd) != ERR_SUCCESS)
		return (manager_clean(manager), 1);

	manager_clean(manager);
	return (0);
}
```

Compile:

```bash
gcc main.c -o main seed_core.a -I./includes
```

---

## Core Concepts

- `t_Manager` is the single entry point.
- All operations are executed through `manager_exec()`.
- Each command uses one dedicated payload type.
- The backend returns typed errors with `t_ErrorCode`.

---

## API Reference

### `t_Manager *manager_init(void)`
Initializes manager, dispatcher, writing system, and filesystem system.

### `void manager_clean(t_Manager *manager)`
Releases all resources. Safe with `NULL`.

### `t_ErrorCode manager_exec(t_Manager *manager, t_Command *cmd)`
Executes a command and returns `ERR_SUCCESS` or an `ERR_*` code.

---

## Command System

```c
typedef struct s_Command
{
	t_CommandId	id;	/* The command ID */
	void		*payload;	/* The payload content */
} t_Command;
```

Rules:

1. Use the matching payload type for each command.
2. `payload = NULL` only for commands that explicitly require no payload.
3. Output fields are valid only after `manager_exec()` returns.

---

## Writing Commands

### `CMD_WRITING_CREATE_BUFFER`
Create a buffer.

Payload:

```c
typedef struct	s_CmdCreateBuffer
{
	size_t	out_buffer_id;	/* The buffer ID that was be created */
}	t_CmdCreateBuffer;
```

Example:

```c
t_CmdCreateBuffer payload = {0};
t_Command cmd = { .id = CMD_WRITING_CREATE_BUFFER, .payload = &payload };
t_ErrorCode err = manager_exec(manager, &cmd);
/* payload.out_buffer_id contains the new id if err == ERR_SUCCESS */
```

---

### `CMD_WRITING_DELETE_BUFFER`
Delete a buffer.

Payload:

```c
typedef struct	s_CmdDestroyBuffer
{
	size_t	buffer_id;	/* The buffer ID */
}	t_CmdDestroyBuffer;
```

Example:

```c
t_CmdDestroyBuffer payload = { .buffer_id = buffer_id };
t_Command cmd = { .id = CMD_WRITING_DELETE_BUFFER, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_WRITING_INSERT_LINE`
Insert a line in a buffer.

Payload:

```c
typedef struct	s_CmdInsertLine
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
}	t_CmdInsertLine;
```

Example:

```c
t_CmdInsertLine payload = { .buffer_id = buffer_id, .line = 0 };
t_Command cmd = { .id = CMD_WRITING_INSERT_LINE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_WRITING_DELETE_LINE`
Delete a line in a buffer.

Payload:

```c
typedef struct	s_CmdDeleteLine
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
}	t_CmdDeleteLine;
```

Example:

```c
t_CmdDeleteLine payload = { .buffer_id = buffer_id, .line = 0 };
t_Command cmd = { .id = CMD_WRITING_DELETE_LINE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_WRITING_SPLIT_LINE`
Split one line at an index.

Payload:

```c
typedef struct	s_CmdSplitLine
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
	size_t	index;	/* The index */
}	t_CmdSplitLine;
```

Example:

```c
t_CmdSplitLine payload = { .buffer_id = buffer_id, .line = 0, .index = 5 };
t_Command cmd = { .id = CMD_WRITING_SPLIT_LINE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_WRITING_JOIN_LINE`
Join two lines.

Payload:

```c
typedef struct	s_CmdJoinLine
{
	size_t	buffer_id;	/* The buffer ID */
	size_t	dst;	/* The destination line */
	size_t	src;	/* The source line */
}	t_CmdJoinLine;
```

Example:

```c
t_CmdJoinLine payload = { .buffer_id = buffer_id, .dst = 0, .src = 1 };
t_Command cmd = { .id = CMD_WRITING_JOIN_LINE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_WRITING_GET_LINE`
Get line content.

Payload:

```c
typedef struct	s_CmdGetLine
{
	size_t		buffer_id;	/* The buffer ID */
	ssize_t		line;	/* The line */
	const char	*out_data;	/* The data that was be readed */
	size_t		out_len;	/* The data length */
}	t_CmdGetLine;
```

Example:

```c
t_CmdGetLine payload = { .buffer_id = buffer_id, .line = 0 };
t_Command cmd = { .id = CMD_WRITING_GET_LINE, .payload = &payload };
if (manager_exec(manager, &cmd) == ERR_SUCCESS)
    printf("%.*s\n", (int)payload.out_len, payload.out_data);
```

---

### `CMD_WRITING_INSERT_TEXT`
Insert data in one line.

Payload:

```c
typedef struct	s_CmdInsertData
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
	ssize_t	column;	/* The column */
	size_t	size;	/* The length of data */
	char	*data;	/* The data content */
}	t_CmdInsertData;
```

Example:

```c
char text[] = "hello";
t_CmdInsertData payload = {
    .buffer_id = buffer_id,
    .line = 0,
    .column = 0,
    .size = 5,
    .data = text
};
t_Command cmd = { .id = CMD_WRITING_INSERT_TEXT, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_WRITING_DELETE_TEXT`
Delete data from one line.

Payload:

```c
typedef struct	s_CmdDeleteData
{
	size_t	buffer_id;	/* The buffer ID */
	ssize_t	line;	/* The line */
	size_t	column;	/* The column */
	size_t	size;	/* The length of data that will be deleted */
}	t_CmdDeleteData;
```

Example:

```c
t_CmdDeleteData payload = {
    .buffer_id = buffer_id,
    .line = 0,
    .column = 0,
    .size = 3
};
t_Command cmd = { .id = CMD_WRITING_DELETE_TEXT, .payload = &payload };
manager_exec(manager, &cmd);
```

---

## Filesystem Commands

Important:

- Open root first with `CMD_FS_OPEN_ROOT`.
- All filesystem paths are relative to this root (except open root path).

### `CMD_FS_OPEN_ROOT`
Open a root directory.

Payload:

```c
typedef struct	s_CmdOpenRoot
{
	char	*path;	/* The absolute path of the directory */
}	t_CmdOpenRoot;
```

Example:

```c
t_CmdOpenRoot payload = { .path = "/absolute/path/to/root" };
t_Command cmd = { .id = CMD_FS_OPEN_ROOT, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_CLOSE_ROOT`
Close root directory.

Payload: `NULL`

Example:

```c
t_Command cmd = { .id = CMD_FS_CLOSE_ROOT, .payload = NULL };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_CREATE_DIR`
Create a directory.

Payload:

```c
typedef struct	s_CmdCreateDir
{
	char	*path;	/* The relative path of the directory */
}	t_CmdCreateDir;
```

Example:

```c
t_CmdCreateDir payload = { .path = "docs" };
t_Command cmd = { .id = CMD_FS_CREATE_DIR, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_DELETE_DIR`
Delete a directory.

Payload:

```c
typedef struct	s_CmdDeleteDir
{
	char	*path;	/* The relative path of the directory */
}	t_CmdDeleteDir;
```

Example:

```c
t_CmdDeleteDir payload = { .path = "docs" };
t_Command cmd = { .id = CMD_FS_DELETE_DIR, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_MOVE_DIR`
Move or rename a directory.

Payload:

```c
typedef struct	s_CmdMoveDir
{
	char	*old_path;	/* The old relative path of the directory */
	char	*new_path;	/* The new relative path of the directory */
}	t_CmdMoveDir;
```

Example:

```c
t_CmdMoveDir payload = { .old_path = "docs", .new_path = "docs_v2" };
t_Command cmd = { .id = CMD_FS_MOVE_DIR, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_CREATE_FILE`
Create a file.

Payload:

```c
typedef struct	s_CmdCreateFile
{
	char	*path;	/* The relative path of the file */
}	t_CmdCreateFile;
```

Example:

```c
t_CmdCreateFile payload = { .path = "docs/readme.txt" };
t_Command cmd = { .id = CMD_FS_CREATE_FILE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_DELETE_FILE`
Delete a file.

Payload:

```c
typedef struct	s_CmdDeleteFile
{
	char	*path;	/* The relative path of the file */
}	t_CmdDeleteFile;
```

Example:

```c
t_CmdDeleteFile payload = { .path = "docs/readme.txt" };
t_Command cmd = { .id = CMD_FS_DELETE_FILE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_READ_FILE`
Read file content.

Payload:

```c
typedef struct	s_CmdReadFile
{
	char	*path;	/* The relative path of the file */
	char	*out_data;	/* The data that was be readed */
	size_t	out_len;	/* The length of the data */
}	t_CmdReadFile;
```

Example:

```c
t_CmdReadFile payload = { .path = "docs/readme.txt" };
t_Command cmd = { .id = CMD_FS_READ_FILE, .payload = &payload };
if (manager_exec(manager, &cmd) == ERR_SUCCESS)
{
    printf("%.*s\n", (int)payload.out_len, payload.out_data);
    free(payload.out_data);
}
```

---

### `CMD_FS_WRITE_FILE`
Write content inside a file.

Payload:

```c
typedef struct	s_CmdWriteFile
{
	char	*path;	/* The relative path of the file */
	char	*data;	/* The data that will be writted */
}	t_CmdWriteFile;
```

Example:

```c
t_CmdWriteFile payload = {
    .path = "docs/readme.txt",
    .data = "hello from seed\n"
};
t_Command cmd = { .id = CMD_FS_WRITE_FILE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

### `CMD_FS_MOVE_FILE`
Move or rename a file.

Payload:

```c
typedef struct	s_CmdMoveFile
{
	char	*old_path;	/* The old relative path of the file */
	char	*new_path;	/* The new relative path of the file */
}	t_CmdMoveFile;
```

Example:

```c
t_CmdMoveFile payload = {
    .old_path = "docs/readme.txt",
    .new_path = "docs/readme_v2.txt"
};
t_Command cmd = { .id = CMD_FS_MOVE_FILE, .payload = &payload };
manager_exec(manager, &cmd);
```

---

## Error Handling

```c
t_ErrorCode err = manager_exec(manager, &cmd);
if (err != ERR_SUCCESS)
{
    fprintf(stderr, "Seed error: %d\n", err);
    return (1);
}
```

Common codes:

- `ERR_INVALID_MANAGER`
- `ERR_INVALID_COMMAND`
- `ERR_INVALID_PAYLOAD`
- `ERR_INVALID_COMMAND_ID`
- `ERR_BUFFER_NOT_FOUND`
- `ERR_LINE_NOT_FOUND`
- `ERR_FS_CONTEXT_NOT_INITIALIZED`
- `ERR_DIR_NOT_FOUND`
- `ERR_FILE_NOT_FOUND`

---

## Changelog

### v0.1

- Initial release
- Writing system (buffer/line/text)
- Command dispatch architecture

### v0.2 (current)

- Added filesystem subsystem and command set
- Naming changes:
  - `t_CmdAddData` -> `t_CmdInsertData`
  - `t_CmdDestroyLine` -> `t_CmdDeleteLine`
- `manager_exec()` returns `t_ErrorCode`

---

## Support

Useful references in this repository:

- `includes/seed.h`
- `tests/systems/writing/`
- `tests/systems/filesystem/TEST_fs.c`

---

**Seed Core v0.2** | Command-based backend API
