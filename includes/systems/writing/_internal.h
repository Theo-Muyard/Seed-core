#ifndef SEED_WRITING_INTERNAL_H
# define SEED_WRITING_INTERNAL_H

# include "dependency.h"

// +===----- Types -----===+ //

/* A line in writing system */
typedef struct	s_Line
{
	char			*data;	/* The data content */
	size_t			size;	/* The data size content */
	size_t			capacity;	/* The data capacity */
	struct s_Line	*prev;	/* The previous line */
	struct s_Line	*next; 	/* The next line */
}	t_Line;

/* A buffer in writing system */
typedef struct	s_Buffer
{
	t_Line	*line;	/* The  first line */
	size_t	size;	/* The count of lines */
}	t_Buffer;

// +===----- Buffer -----===+ //

/**
 * @brief Creates a new empty buffer.
 * @return The buffer that has just been created.
*/
t_Buffer	*buffer_create(void);

/**
 * @brief Destroys the given buffer.
 * @param buffer The buffer that will be destroyed.
*/
void		buffer_destroy(t_Buffer *buffer);

// +===----- Lines -----===+ //

/**
 * @brief Creates a new empty line.
 * @return The line that has just been created.
*/
t_Line		*line_create(void);

/**
 * @brief Destroys the given line.
 * @param buffer The buffer that contains lines.
 * @param line The line that will be destroyed.
*/
void		buffer_line_destroy(t_Buffer *buffer, t_Line *line);

/**
 * @brief Adds the line to the index of the buffer.
 * @param buffer The buffer that contains lines.
 * @param line The line that will be added in the given index of the buffer.
 * @param index The index of the line will be added.
*/
bool		buffer_line_insert(t_Buffer *buffer, t_Line *line, ssize_t index);

/**
 * @brief Splits the given line in two lines.
 * @param buffer The buffer that contains lines.
 * @param line The line that will be split.
 * @param index The index where the line will be splited.
 * @return TRUE for success or FALSE if an error occured.
*/
t_Line		*buffer_line_split(t_Buffer *buffer, t_Line *line, size_t index);

/**
 * @brief Joins the givens line in one line.
 * @param buffer The buffer that contains lines.
 * @param dst The line that will be contains the lines joined.
 * @param src The line that will be destroyed.
 * @return TRUE for success or FALSE if an error occured.
*/
t_Line		*buffer_line_join(t_Buffer *buffer, t_Line *dst, t_Line *src);

/**
 * @brief Get the line of the given index.
 * @param buffer The buffer that contains lines.
 * @param index The index of the line.
 * @return The desired line.
*/
t_Line		*buffer_get_line(t_Buffer *buffer, ssize_t index);

// +===----- Data -----===+ //

/**
 * @brief Add the data to the given line.
 * @param line The line.
 * @param column The first column where data is added.
 * @param size The size of the data.
 * @param data The data that will be added.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		line_insert_data(t_Line *line, ssize_t column, size_t size, const char *data);

/**
 * @brief Delete the data to the given line.
 * @param line The line.
 * @param column The first column where data is added.
 * @param size The size of the data.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		line_delete_data(t_Line *line, size_t column, size_t size);

#endif