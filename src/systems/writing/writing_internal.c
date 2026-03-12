#include "systems/writing/writing_internal.h"

// +===----- Buffer functions -----===+ //

t_Buffer	*buffer_create(void)
{
	t_Buffer	*buffer = malloc(sizeof(t_Buffer));
	RETURN_IF_NULL(buffer, NULL);

	buffer->line = NULL;
	buffer->count = 0;

	return (buffer);
}

void		buffer_destroy(t_Buffer *buffer)
{
	if (NULL == buffer)
		return ;

	while (buffer->line)
	{
		t_Line	*_tmp = buffer->line->next;
		buffer_line_destroy(buffer, buffer->line);
		buffer->line = _tmp;
	}
	free(buffer);
}

// +===----- Line functions -----===+ //

t_Line	*line_create(void)
{
	t_Line	*line = malloc(sizeof(t_Line));
	RETURN_IF_NULL(line, NULL);

	line->data = NULL;
	line->size = 0;
	line->capacity = 0;
	line->prev = NULL;
	line->next = NULL;

	return (line);
}

void		buffer_line_destroy(t_Buffer *buffer, t_Line *line)
{
	if (NULL == line)
		return ;

	t_Line	*_prev = line->prev;
	t_Line	*_next = line->next;

	if (_prev)
		_prev->next = _next;
	else
		buffer->line = _next;
	if (_next)
		_next->prev = _prev;
	if (buffer->count > 0)
		buffer->count--;

	free(line->data);
	line->data = NULL;
	free(line);
}

t_Line	*buffer_get_line(t_Buffer *buffer, ssize_t index)
{
	RETURN_IF_NULL(buffer, NULL);

	if (index < 0)
		index = buffer->count ? buffer->count - 1 : 0;

	if ((size_t)index > buffer->count)
		return (NULL);

	t_Line	*_tmp = buffer->line;
	size_t	_i = 0;

	while (_tmp && _i < (size_t)index)
	{
		_tmp = _tmp->next;
		_i++;
	}
	return (_tmp);
}

bool		buffer_line_insert(t_Buffer *buffer, t_Line *line, ssize_t index)
{
	RETURN_IF_NULL(buffer, NULL);
	RETURN_IF_NULL(line, NULL);

	if (index < 0)
		index = buffer->count;

	if ((size_t)index > buffer->count)
		return (NULL);

	if (index == 0)
	{
		line->next = buffer->line;
		line->prev = NULL;
		if (buffer->line)
			buffer->line->prev = line;
		buffer->line = line;
		buffer->count++;
		return (true);
	}

	t_Line	*_tmp = buffer_get_line(buffer, index);
	RETURN_IF_NULL(_tmp, false);

	line->next = _tmp->next;
	line->prev = _tmp;
	if (_tmp->next)
		_tmp->next->prev = line;
	_tmp->next = line;
	buffer->count++;

	return (true);
}

t_Line	*buffer_line_split(t_Buffer *buffer, t_Line *line, size_t index)
{
	RETURN_IF_NULL(buffer, NULL);
	RETURN_IF_NULL(line, NULL);

	if (index > line->size)
		return (NULL);

	t_Line	*_new_line = line_create();
	RETURN_IF_NULL(_new_line, NULL);

	size_t	_size = line->size - index;

	GOTO_IF_FALSE(
		line_insert_data(_new_line, 0, _size, line->data + index),
		exit_destroy_line
	);

	GOTO_IF_FALSE(
		line_delete_data(line, index, _size),
		exit_destroy_line
	);

	t_Line	*_tmp = line->next;
	_new_line->prev = line;
	_new_line->next = _tmp;
	if (_tmp)
		_tmp->prev = _new_line;
	line->next = _new_line;
	buffer->count++;

	return (_new_line);

	/* GOTO EXIT */
	exit_destroy_line:
		return (buffer_line_destroy(buffer, _new_line), NULL);
}

t_Line	*buffer_line_join(t_Buffer *buffer, t_Line *dst, t_Line *src)
{
	RETURN_IF_NULL(buffer, NULL);
	RETURN_IF_NULL(dst, NULL);
	RETURN_IF_NULL(src, NULL);

	RETURN_IF_FALSE(
		line_insert_data(dst, dst->size, src->size, src->data),
		NULL
	);

	buffer_line_destroy(buffer, src);
	return (dst);
}

// +===----- Data functions -----===+ //

bool		line_insert_data(t_Line *line, ssize_t index, size_t size, const char *data)
{
	RETURN_IF_NULL(line, false);
	RETURN_IF_NULL(data, false);

	if (index < 0)
		index = line->size;

	if ((size_t)index > line->size)
		return (false);

	size_t	_needed_capacity = line->size + size + 1;
	if (_needed_capacity > line->capacity)
	{
		size_t	_new_capacity = line->capacity ? line->capacity : DATA_ALLOC;
		while (_new_capacity < _needed_capacity)
			_new_capacity *= 2;

		char	*_new_data = realloc(line->data, _new_capacity * sizeof(char));
		RETURN_IF_NULL(_new_data, false);

		line->data = _new_data;
		line->capacity = _new_capacity;
	}

	size_t	_insert_size = size;
	if (size > 0 && data[size - 1] == '\0')
		_insert_size--;

	printf("INDEX: %zu\n", index);
	memmove(
		line->data + index + _insert_size,
		line->data + index,
		line->size - index
	);

	memcpy(line->data + index, data, _insert_size);

	line->size += _insert_size;
	line->data[line->size] = '\0';

	return (true);
}

bool		line_delete_data(t_Line *line, size_t index, size_t size)
{
	RETURN_IF_NULL(line, false);

	if (index > line->size)
		return (false);

	if (index + size > line->size)
    	size = line->size - index;

	if (NULL == line->data || line->size == 0)
		return (false);

	memmove(
		line->data + index,
		line->data + index + size,
		line->size - (index + size)
	);

	line->size -= size;
	line->data[line->size] = '\0';

	return (true);
}
