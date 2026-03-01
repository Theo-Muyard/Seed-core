#include "systems/filesystem/_os.h"

// +===----- OS Directory -----===+ //

bool		os_dir_create(char *path, unsigned int mode)
{
	if (NULL == path)
		return (false);
	if (-1 == mkdir(path, mode))
		return (false);
	return (true);
}

bool		os_dir_delete(char *path)
{
	if (NULL == path)
		return (false);
	if (-1 == rmdir(path))
		return (false);
	return (true);
}

bool		os_dir_move(char *old_path, char *new_path)
{
	if (NULL == old_path || NULL == new_path)
		return (false);
	if (-1 == rename(old_path, new_path))
		return (false);
	return (true);
}

// +===----- OS Files -----===+ //

FILE		*os_file_create(char *path, char *mode)
{
	FILE	*file;

	if (NULL == path || NULL == mode)
		return (NULL);
	file = fopen(path, "r");
	if (NULL != file)
		return (fclose(file), NULL);
	file = fopen(path, mode);
	return (file);	
}

bool		os_file_delete(char *path)
{
	if (NULL == path)
		return (false);
	if (-1 == remove(path))
		return (false);
	return (true);
}

FILE		*os_file_open(char *path, char *mode)
{
	FILE	*file;

	if (NULL == path || NULL == mode)
		return (NULL);
	file = fopen(path, mode);
	return (file);
}

bool		os_file_move(char *old_path, char *new_path)
{
	if (NULL == old_path || NULL == new_path)
		return (false);
	if (-1 == rename(old_path, new_path))
		return (false);
	return (true);
}

bool		os_file_write(FILE *file, char *data)
{
	if (NULL == file || NULL == data)
		return (false);
	if (0 > fprintf(file,  "%s", data))
		return (false);
	return (true);
}

void		os_file_save(FILE *file)
{
	fclose(file);
}

char	*os_file_get_data(FILE *file)
{
	char	*buffer;
	size_t	_size;
	size_t	_read;

	if (NULL == file)
		return (NULL);
	fseek(file, 0, SEEK_END);
	_size = ftell(file);
	rewind(file);

	buffer = malloc((_size + 1) * sizeof(char));
	if (NULL == buffer)
		return (NULL);
	_read = fread(buffer, 1, _size, file);
	rewind(file);
	if (_size != _read)
		return (free(buffer), NULL);
	buffer[_size] = '\0';
	return (buffer);
}

