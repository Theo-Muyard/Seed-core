# Seed Core v0.1 - API Documentation

> A high-performance backend system for text editing operations, providing a command-based interface for buffer and line manipulation.

## Table of Contents

- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
- [API Reference](#api-reference)
- [Command System](#command-system)
- [Writing System](#writing-system)
- [Usage Examples](#usage-examples)
- [Error Handling](#error-handling)

---

## Quick Start

### Basic Setup

```c
#include "seed.h"

int main(void)
{
    // Initialize the manager
    t_Manager *manager = manager_init();
    if (!manager)
        return (1);
    
    // Execute a command
    t_Command cmd = {
        .id = CMD_WRITING_CREATE_BUFFER,
        .payload = NULL,
        .payload_size = 0
    };
    
    if (!manager_exec(manager, &cmd))
        return (1);
    
    // Clean up
    manager_clean(manager);
    return (0);
}
```

### Compilation

```bash
gcc main.c -o main -L. -lseed_core -I./includes
```

---

## Core Concepts

### Manager

The **Manager** is the central entry point for the Seed Core system. It acts as a gateway to all operations.

- **One manager per application** - You typically create one manager at startup
- **Thread-safe operations** - Each operation is atomic through the command dispatch system
- **Automatic initialization** - The manager sets up all internal systems (dispatcher, writing context)

### Command-Based Architecture

Seed Core uses a **command dispatch pattern**:

1. Create a `t_Command` structure
2. Fill in the command ID and payload
3. Execute via `manager_exec()`
4. Results are written back to the payload

This design allows for:
- Easy serialization for network communication
- Clean separation of concerns
- Future support for async operations

### Buffers

**Buffers** are the main data structures in the Writing System:

- Each buffer contains a **linked list of lines**
- Each line contains **text data** with dynamic capacity
- Buffers are created/destroyed on demand
- Each buffer gets a unique **buffer ID** for later reference

---

## API Reference

### Manager Functions

#### `t_Manager *manager_init(void)`

Initializes the Seed Core manager and all subsystems.

**Returns:**
- `t_Manager*` - Pointer to the newly created manager
- `NULL` - If initialization failed

**Example:**
```c
t_Manager *manager = manager_init();
if (!manager) {
    perror("Failed to initialize manager");
    return (1);
}
```

---

#### `void manager_clean(t_Manager *manager)`

Cleans up and frees all resources allocated by the manager.

**Parameters:**
- `manager` - The manager to clean (can be NULL)

**Important:** 
- Always call this before exiting your application
- Automatically cleans all buffers and internal structures
- Safe to call with NULL pointer

**Example:**
```c
manager_clean(manager);  // Safe, even if manager is NULL
```

---

#### `bool manager_exec(t_Manager *manager, t_Command *cmd)`

Executes a command on the manager.

**Parameters:**
- `manager` - The manager instance
- `cmd` - Pointer to the command to execute

**Returns:**
- `true` - Command executed successfully
- `false` - Command failed or invalid

**Important Notes:**
- The `cmd` parameter must point to a properly initialized `t_Command` structure
- Some commands write results back into the payload
- Always check the return value
- Payload must be valid for the entire execution duration

**Example:**
```c
t_CmdCreateBuffer payload = {0};
t_Command cmd = {
    .id = CMD_WRITING_CREATE_BUFFER,
    .payload = &payload,
    .payload_size = sizeof(t_CmdCreateBuffer)
};

if (!manager_exec(manager, &cmd)) {
    fprintf(stderr, "Command failed\n");
    return (1);
}

// After execution, payload.out_buffer_id contains the new buffer ID
size_t buffer_id = payload.out_buffer_id;
```

---

## Command System

### Command Structure

```c
typedef struct s_Command
{
    t_CommandId id;              // Which command to execute
    void *payload;               // Command-specific data (can be NULL)
    size_t payload_size;         // Size of the payload
} t_Command;
```

### Payload Rules

1. **Payload can be NULL** - For commands that don't need parameters
2. **Size must match** - The payload_size should exactly match the struct size
3. **Input vs Output fields** - Some fields are written to, some are read from
4. **Lifetime** - The payload must remain valid during `manager_exec()` execution

---

## Writing System

The Writing System manages text buffers, lines, and text operations.

### Command Reference

#### Buffer Commands

##### `CMD_WRITING_CREATE_BUFFER`

Creates a new empty buffer.

**Payload:** `t_CmdCreateBuffer`
```c
typedef struct {
    size_t out_buffer_id;  // OUTPUT: The ID of the created buffer
} t_CmdCreateBuffer;
```

**Usage:**
```c
t_CmdCreateBuffer payload = {0};
t_Command cmd = {
    .id = CMD_WRITING_CREATE_BUFFER,
    .payload = &payload,
    .payload_size = sizeof(t_CmdCreateBuffer)
};

manager_exec(manager, &cmd);
size_t buffer_id = payload.out_buffer_id;  // Use this ID for future operations
```

---

##### `CMD_WRITING_DELETE_BUFFER`

Destroys a buffer and all its contents.

**Payload:** `t_CmdDestroyBuffer`
```c
typedef struct {
    size_t buffer_id;  // INPUT: The buffer to destroy
} t_CmdDestroyBuffer;
```

**Important:** After this command, all lines in the buffer are freed. Don't reference them afterwards.

**Usage:**
```c
t_CmdDestroyBuffer payload = {
    .buffer_id = buffer_id
};
t_Command cmd = {
    .id = CMD_WRITING_DELETE_BUFFER,
    .payload = &payload,
    .payload_size = sizeof(t_CmdDestroyBuffer)
};

manager_exec(manager, &cmd);
```

---

#### Line Commands

##### `CMD_WRITING_INSERT_LINE`

Inserts a new empty line at the specified position.

**Payload:** `t_CmdInsertLine`
```c
typedef struct {
    size_t buffer_id;  // INPUT: Target buffer
    size_t line;       // INPUT: Position to insert (0-based)
} t_CmdInsertLine;
```

**Line Numbering:**
- Line 0 is the first line
- Line count equals the index of the last line + 1
- Line -1 appends to the end
- Using index beyond current size will fail

**Usage:**
```c
t_CmdInsertLine payload = {
    .buffer_id = buffer_id,
    .line = 0  // Insert at the beginning
};
t_Command cmd = {
    .id = CMD_WRITING_INSERT_LINE,
    .payload = &payload,
    .payload_size = sizeof(t_CmdInsertLine)
};

manager_exec(manager, &cmd);
```

---

##### `CMD_WRITING_DELETE_LINE`

Deletes a line at the specified position.

**Payload:** `t_CmdDestroyLine`
```c
typedef struct {
    size_t buffer_id;  // INPUT: Target buffer
    size_t line;       // INPUT: Line to delete (0-based)
} t_CmdDestroyLine;
```

**Usage:**
```c
t_CmdDestroyLine payload = {
    .buffer_id = buffer_id,
    .line = 0,
};
t_Command cmd = {
    .id = CMD_WRITING_DELETE_LINE,
    .payload = &payload,
    .payload_size = sizeof(t_CmdDestroyLine)
};

manager_exec(manager, &cmd);
```

---

##### `CMD_WRITING_SPLIT_LINE`

Splits a line into two at the specified index.

**Payload:** `t_CmdSplitLine`
```c
typedef struct {
    size_t buffer_id;  // INPUT: Target buffer
    size_t line;       // INPUT: Line to split (0-based)
    size_t index;      // INPUT: Character index where to split
} t_CmdSplitLine;
```

**Behavior:**
- Text before `index` stays in the original line
- Text from `index` onwards goes to the new line
- The new line is inserted right after the original line

**Example:**
```
Original line: "Hello World"
After split at index 5: 
  Line N:   "Hello"
  Line N+1: " World"
```

---

##### `CMD_WRITING_JOIN_LINE`

Joins two consecutive lines into one.

**Payload:** `t_CmdJoinLine`
```c
typedef struct {
    size_t buffer_id;  // INPUT: Target buffer
    size_t dst;        // INPUT: Destination line (0-based)
    size_t src;        // INPUT: Source line (must be dst + 1)
} t_CmdJoinLine;
```

**Requirements:**
- The source line must immediately follow the destination line
- The source line content is appended to the destination line

**Example:**
```
Line N:   "Hello"
Line N+1: " World"
After join:
  Line N: "Hello World"
```

---

##### `CMD_WRITING_GET_LINE`

Retrieves the content of a line.

**Payload:** `t_CmdGetLine`
```c
typedef struct {
    size_t buffer_id;      // INPUT: Target buffer
    size_t line;           // INPUT: Line to retrieve (0-based)
    const char *out_data;  // OUTPUT: Pointer to line data
    size_t out_len;        // OUTPUT: Length of the data
} t_CmdGetLine;
```

**Important:**
- The returned pointer points to internal buffer memory
- Do **NOT** free this pointer
- The pointer becomes invalid after other operations on the same line

**Usage:**
```c
t_CmdGetLine payload = {
    .buffer_id = buffer_id,
    .line = 0
};
t_Command cmd = {
    .id = CMD_WRITING_GET_LINE,
    .payload = &payload,
    .payload_size = sizeof(t_CmdGetLine)
};

manager_exec(manager, &cmd);

// Use the data immediately
printf("Line content: %.*s\n", (int)payload.out_len, payload.out_data);
// or copy it if you need it later
char *copy = malloc(payload.out_len + 1);
memcpy(copy, payload.out_data, payload.out_len);
copy[payload.out_len] = '\0';
```

---

#### Text Data Commands

##### `CMD_WRITING_INSERT_TEXT`

Inserts text into a line at the specified column.

**Payload:** `t_CmdAddData`
```c
typedef struct {
    size_t buffer_id;  // INPUT: Target buffer
    size_t line;       // INPUT: Target line (0-based)
    size_t column;     // INPUT: Column to insert at (0-based)
    size_t size;       // INPUT: Number of bytes to insert
    char *data;        // INPUT: Pointer to the data to insert
} t_CmdAddData;
```

**Column Indexing:**
- Column 0 is before the first character
- Column equal to line length or -1 appends to the end

**Important:**
- The `data` pointer must be valid during execution
- You can insert any binary data (not just text)

**Usage:**
```c
const char *text = "Hello";
t_CmdAddData payload = {
    .buffer_id = buffer_id,
    .line = 0,
    .column = 0,
    .size = 5,
    .data = (char *)text
};
t_Command cmd = {
    .id = CMD_WRITING_INSERT_TEXT,
    .payload = &payload,
    .payload_size = sizeof(t_CmdAddData)
};

manager_exec(manager, &cmd);
```

---

##### `CMD_WRITING_DELETE_TEXT`

Deletes text from a line at the specified column.

**Payload:** `t_CmdDeleteData`
```c
typedef struct {
    size_t buffer_id;  // INPUT: Target buffer
    size_t line;       // INPUT: Target line (0-based)
    size_t column;     // INPUT: Starting column (0-based)
    size_t size;       // INPUT: Number of bytes to delete
} t_CmdDeleteData;
```

**Behavior:**
- Deletes `size` bytes starting from `column`
- If deletion would exceed line length, deletes up to the end

**Usage:**
```c
t_CmdDeleteData payload = {
    .buffer_id = buffer_id,
    .line = 0,
    .column = 0,
    .size = 5  // Delete 5 characters from the start
};
t_Command cmd = {
    .id = CMD_WRITING_DELETE_TEXT,
    .payload = &payload,
    .payload_size = sizeof(t_CmdDeleteData)
};

manager_exec(manager, &cmd);
```

---

## Usage Examples

### Example 1: Create a Buffer and Add Text

```c
#include "seed.h"
#include <stdio.h>

int main(void)
{
    t_Manager *manager = manager_init();
    if (!manager)
        return (1);
    
    // Create buffer
    t_CmdCreateBuffer create_payload = {0};
    t_Command create_cmd = {
        .id = CMD_WRITING_CREATE_BUFFER,
        .payload = &create_payload,
        .payload_size = sizeof(t_CmdCreateBuffer)
    };
    manager_exec(manager, &create_cmd);
    size_t buffer_id = create_payload.out_buffer_id;
    
    // Insert a line
    t_CmdInsertLine insert_payload = {
        .buffer_id = buffer_id,
        .line = 0
    };
    t_Command insert_cmd = {
        .id = CMD_WRITING_INSERT_LINE,
        .payload = &insert_payload,
        .payload_size = sizeof(t_CmdInsertLine)
    };
    manager_exec(manager, &insert_cmd);
    
    // Add text to the line
    const char *text = "Hello, Seed!";
    t_CmdAddData add_payload = {
        .buffer_id = buffer_id,
        .line = 0,
        .column = 0,
        .size = 12,
        .data = (char *)text
    };
    t_Command add_cmd = {
        .id = CMD_WRITING_INSERT_TEXT,
        .payload = &add_payload,
        .payload_size = sizeof(t_CmdAddData)
    };
    manager_exec(manager, &add_cmd);
    
    // Read the text
    t_CmdGetLine get_payload = {
        .buffer_id = buffer_id,
        .line = 0
    };
    t_Command get_cmd = {
        .id = CMD_WRITING_GET_LINE,
        .payload = &get_payload,
        .payload_size = sizeof(t_CmdGetLine)
    };
    manager_exec(manager, &get_cmd);
    
    printf("Content: %.*s\n", (int)get_payload.out_len, get_payload.out_data);
    
    manager_clean(manager);
    return (0);
}
```

---

### Example 2: Split and Join Lines

```c
// After setup (buffer with one line containing "HelloWorld")

// Split the line at index 5
t_CmdSplitLine split_payload = {
    .buffer_id = buffer_id,
    .line = 0,
    .index = 5
};
t_Command split_cmd = {
    .id = CMD_WRITING_SPLIT_LINE,
    .payload = &split_payload,
    .payload_size = sizeof(t_CmdSplitLine)
};
manager_exec(manager, &split_cmd);
// Now: Line 0 = "Hello", Line 1 = "World"

// Join them back
t_CmdJoinLine join_payload = {
    .buffer_id = buffer_id,
    .dst = 0,
    .src = 1
};
t_Command join_cmd = {
    .id = CMD_WRITING_JOIN_LINE,
    .payload = &join_payload,
    .payload_size = sizeof(t_CmdJoinLine)
};
manager_exec(manager, &join_cmd);
// Now: Line 0 = "HelloWorld"
```

---

## Error Handling

### Command Failure

Always check the return value of `manager_exec()`:

```c
if (!manager_exec(manager, &cmd)) {
    fprintf(stderr, "Command execution failed\n");
    // Handle error - specific error codes not available, check your logic
}
```

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| `manager_exec()` returns false | Invalid buffer ID | Verify buffer was created successfully |
| Segfault on `out_data` access | Pointer used after other operations | Copy data immediately after retrieval |
| Data not inserted | `column` beyond line length | Ensure column <= line length |
| Join fails | Source line not adjacent to destination | Use correct line indices |
| Split fails | `index` beyond line length | Ensure index <= line length |

### Safety Practices

1. **Always initialize the manager** - Check for NULL return value
2. **Always clean up** - Call `manager_clean()` before exit
3. **Validate indices** - Check buffer and line IDs before use
4. **Copy output data immediately** - Don't rely on pointer validity
5. **Use exact sizes** - Match payload sizes exactly

---

## Best Practices

### Memory Management

```c
// ✅ CORRECT
t_CmdGetLine payload = {.buffer_id = bid, .line = 0};
manager_exec(manager, &cmd);
// Use payload.out_data immediately or copy it
char *copy = malloc(payload.out_len + 1);
memcpy(copy, payload.out_data, payload.out_len);
copy[payload.out_len] = '\0';

// ❌ WRONG
manager_exec(manager, &cmd);
// ... do other stuff ...
printf("%s\n", payload.out_data);  // Pointer may be invalid
```

### Command Building

```c
// ✅ CORRECT - Initialize all fields
t_CmdInsertLine payload = {
    .buffer_id = buffer_id,
    .line = 0
};

// ❌ WRONG - Using uninitialized fields
t_CmdInsertLine payload;
payload.buffer_id = buffer_id;
// payload.line is uninitialized!
```

### Error Recovery

```c
if (!manager_exec(manager, &cmd)) {
    // Don't continue with dependent operations
    fprintf(stderr, "Buffer creation failed\n");
    manager_clean(manager);
    return (1);
}

// Only proceed if operation succeeded
size_t buffer_id = payload.out_buffer_id;
```

---

## Changelog

### v0.1 (Current)
- Initial release
- Writing system with buffer, line, and text operations
- Command-based architecture
- 9 core commands

---

## Support

For issues or questions, refer to the source code documentation or test files in the `tests/` directory for additional usage examples.

---

**Seed Core v0.1** | Built for high-performance text editing operations