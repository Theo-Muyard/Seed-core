#include <stdlib.h>
#include <string.h>
#include "systems/writing/_internal.h"

#define DATA_ALLOC 256

// +===----- BUFFER -----===+ //

t_Buffer	*buffer_create(void)
{
	t_Buffer	*buffer;

	buffer = malloc(sizeof(t_Buffer));
	if (NULL == buffer)
		return (NULL);
	buffer->line = NULL;
	buffer->size = 0;
	return (buffer);
}

void		buffer_destroy(t_Buffer *buffer)
{
	t_Line	*_tmp;

	if (NULL == buffer)
		return ;
	while (buffer->line)
	{
		_tmp = buffer->line->next;
		buffer_line_destroy(buffer, buffer->line);
		buffer->line = _tmp;
	}
	free(buffer);
}

// +===----- LINES -----===+ //

t_Line		*line_create(void)
{
	t_Line	*line;

	line = malloc(sizeof(t_Line));
	if (NULL == line)
		return (NULL);
	line->data = NULL;
	line->len = 0;
	line->capacity = 0;
	line->prev = NULL;
	line->next = NULL;
	return (line);
}

void		buffer_line_destroy(t_Buffer *buffer, t_Line *line)
{
	t_Line	*_prev;
	t_Line	*_next;

	if (NULL == line)
		return ;
	
	_prev = line->prev;
	_next = line->next;
	if (_prev)
		_prev->next = _next;
	else
		buffer->line = _next;
	if (_next)
		_next->prev = _prev;
	if (buffer->size > 0)
		buffer->size--;
	free(line->data);
	free(line);
}

bool		buffer_line_insert(t_Buffer *buffer, t_Line *line, ssize_t index)
{
	t_Line	*_tmp;
	ssize_t	_i;

	if (NULL == buffer || NULL == line)
		return (false);
	
	if (index < 0)
		index = buffer->size;
	if (index > buffer->size)
		return (false);

	if (index == 0)
	{
		line->next = buffer->line;
		line->prev = NULL;
		if (buffer->line)
			buffer->line->prev = line;
		buffer->line = line;
		buffer->size++;
		return (true);
	}
	_i = 0;
	_tmp = buffer->line;
	while (_tmp && _i < index - 1)
	{
		_tmp = _tmp->next;
		_i++;
	}
	if (NULL == _tmp)
		return (false);
	line->next = _tmp->next;
	line->prev = _tmp;
	if (_tmp->next)
		_tmp->next->prev = line;
	_tmp->next = line;
	buffer->size++;
	return (true);
}

t_Line		*buffer_line_split(t_Buffer *buffer, t_Line *line, size_t index)
{
	t_Line	*_new_line;
	t_Line	*_tmp;
	size_t	_size;

	if (NULL == buffer || NULL == line)
		return (NULL);
	if (index > line->len)
		return (NULL);

	_new_line = line_create();
	if (NULL == _new_line)
		return (NULL);
	_size = line->len - index;
	if (false == line_add_data(_new_line, 0, _size, line->data + index))
		return (buffer_line_destroy(buffer, _new_line), NULL);
	if (false == line_delete_data(line, index, _size))
		return (buffer_line_destroy(buffer, _new_line), NULL);
	_tmp = line->next;
	_new_line->prev = line;
	_new_line->next = _tmp;
	if (_tmp)
		_tmp->prev = _new_line;
	line->next = _new_line;
	buffer->size++;
	return (_new_line);
}

t_Line		*buffer_line_join(t_Buffer *buffer, t_Line *dst, t_Line *src)
{
	if (NULL == dst || NULL == src)
		return (NULL);
	if (false == line_add_data(dst, dst->len, src->len, src->data))
		return (NULL);
	buffer_line_destroy(buffer, src);
	return (dst);
}

t_Line		*buffer_get_line(t_Buffer *buffer, ssize_t index)
{
	t_Line	*_tmp;
	ssize_t	_i;

	if (NULL == buffer || index >= buffer->size)
		return (NULL);
	
	if (index < 0)
		index = buffer->size - 1;

	_tmp = buffer->line;
	_i = 0;
	while (_tmp && _i < index)
	{
		_tmp = _tmp->next;
		_i++;
	}
	return (_tmp);
}

// +===----- DATA -----===+ //

bool		line_add_data(t_Line *line, ssize_t column, size_t size, const char *data)
{
	char	*_new_data;
	size_t	_needed_capacity;
	size_t	_new_capacity;

	if (NULL == line || NULL == data)
		return (false);
	if (column < 0)
		column = line->len;
	if ((size_t)column > line->len)
		return (false);

	_needed_capacity = line->len + size + 1;
	if (_needed_capacity > line->capacity)
	{
		_new_capacity = line->capacity ? line->capacity : DATA_ALLOC;
		while (_new_capacity < _needed_capacity)
			_new_capacity *= 2;
		_new_data = realloc(line->data, _new_capacity * sizeof(char));
		if (NULL == _new_data)
			return (false);
		line->data = _new_data;
		line->capacity = _new_capacity;
	}
	memmove(
		line->data + column + size,
		line->data + column,
		line->len - column
	);
	memcpy(line->data + column, data, size);
	line->len += size;
	line->data[line->len] = '\0';
	return (true);
}

bool		line_delete_data(t_Line *line, ssize_t column, size_t size)
{
	if (NULL == line)
		return (false);
	if (column < 0)
		column = line->len;
	if ((size_t)column > line->len)
		return (false);
	if (column + size > line->len)
    	size = line->len - column;
	if (NULL == line->data || line->len == 0)
		return (false);

	memmove(
			line->data + column,
			line->data + column + size,
			line->len - (column + size)
	);
	line->len = line->len - size;
	line->data[line->len] = '\0';
	return (true);
}
