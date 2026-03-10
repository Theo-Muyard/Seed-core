/**
 * @file "systems/writing/writing_internal.h"
 * 
 * @brief The core of the writing system. These are the internal
 * 		functions used for execute the commands.
 * 
 * @ingroup "writing system"
*/

#ifndef SEED_WRITING_INTERNAL_H
# define SEED_WRITING_INTERNAL_H

# include "dependency.h"

# define DATA_ALLOC 256

// +===----- Types -----===+ //

/* A line in writing system */
typedef struct	s_Line
{
	char			*data;		/* The data */
	size_t			size;		/* The data size */
	size_t			capacity;	/* The capacity */
	struct s_Line	*prev;			/* The previous line */
	struct s_Line	*next; 			/* The next line */
}	t_Line;

/* A buffer in writing system */
typedef struct	s_Buffer
{
	t_Line	*line;	/* Pointer to first line */
	size_t	count;	/* Lines count */
}	t_Buffer;

// +===----- Buffer functions -----===+ //

/**
 * @brief Creates a new empty buffer.
 * 
 * @retval A newly allocated `t_Buffer` owned by caller.
 * @retval NULL if allocation fails.
 * 
 * @warning Caller must free returned pointer with `buffer_destroy()`.
*/
t_Buffer	*buffer_create(void);

/**
 * @brief Destroys the given buffer.
 * 
 * @param buffer The buffer must not be NULL.
*/
void		buffer_destroy(t_Buffer *buffer);

// +===----- Line functions -----===+ //

/**
 * @brief Creates a new empty line.
 * 
 * @retval A newly allocated `t_Line` owned by caller.
 * @retval NULL if allocation fails.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
t_Line		*line_create(void);

/**
 * @brief Destroys the given line in the buffer.
 * 
 * @param buffer The buffer must not be NULL.
 * @param line The line must not be NULL.
*/
void		buffer_line_destroy(t_Buffer *buffer, t_Line *line);

/**
 * @brief Retrieve a line by its index in the buffer.
 * 
 * @param buffer The buffer must not be NULL.
 * 
 * @param index The index. Setting the value to `-1` you get the last
 * 				line.
 * 
 * @retval The line.
 * @retval NULL if `buffer` is NULL, if the index does not exist
 * 		in the buffer or an error occurred. 
*/
t_Line		*buffer_get_line(t_Buffer *buffer, ssize_t index);

/**
 * @brief Adds the line to the specified index in the buffer.
 * 
 * @param buffer The buffer must not be NULL.
 * @param line The line that will be added in the given index of the buffer.
 * @param index The index. Setting the value to -1 adds the line at
 * 			the end.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `buffer` or `line` is NULL or an error occurred.
*/
bool		buffer_line_insert(t_Buffer *buffer, t_Line *line, ssize_t index);

/**
 * @brief Splits the line into two separate lines at the
 * 		index given.
 * 
 * @param buffer The buffer must not be NULL.
 * @param line The line must not be NULL.
 * @param index The index must not be `0`, `-1` or the length
 * 				of the data.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `buffer` or `line` is NULL or an error occured.
*/
t_Line		*buffer_line_split(t_Buffer *buffer, t_Line *line, size_t index);

/**
 * @brief Joins two lines.
 * 
 * @param buffer The buffer must not be NULL.
 * @param dst The destination line must not be NULL.
 * @param src The source line must not be NULL.
 * 
 * @details The data of the source line is added to the end of
 * 			the content of the destination line. The source line
 * 			is destroyed.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `buffer` or `line` is NULL or an error occured.
*/
t_Line		*buffer_line_join(t_Buffer *buffer, t_Line *dst, t_Line *src);

// +===----- Data functions -----===+ //

/**
 * @brief Add the data to the line.
 * 
 * @param line The line must not be NULL.
 * @param column The first column.
 * @param size The data size in bytes.
 * @param data The data content.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `line` is NULL or an error occurred.
*/
bool		line_insert_data(
	t_Line *line, ssize_t column,
	size_t size, const char *data
);

/**
 * @brief Delete the data to the line.
 * 
 * @param line The line must not be NULL.
 * @param column The first column.
 * @param size The data size in bytes.

 * @retval TRUE for success.
 * @retval FALSE if `line` is NULL or an error occurred.
*/
bool		line_delete_data(t_Line *line, size_t column, size_t size);

#endif