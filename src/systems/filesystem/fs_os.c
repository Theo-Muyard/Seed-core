#include "systems/filesystem/fs_os.h"

// +===----- Directory functions -----===+ //

bool		os_dir_create(const char *path, unsigned int mode)
{
	RETURN_IF_NULL(path, false);

	if (-1 == mkdir(path, mode))
		return (false);
	return (true);
}

bool		os_dir_delete(const char *path)
{
	RETURN_IF_NULL(path, false);

	if (-1 == rmdir(path))
		return (false);
	return (true);
}

bool		os_dir_move(const char *old_path, const char *new_path)
{
	RETURN_IF_NULL(old_path, false);
	RETURN_IF_NULL(new_path, false);
	
	if (-1 == rename(old_path, new_path))
		return (false);
	return (true);
}

// +===----- OS Files -----===+ //

FILE		*os_file_create(const char *path, const char *mode)
{
	RETURN_IF_NULL(path, NULL);
	RETURN_IF_NULL(mode, NULL);

	FILE	*file = fopen(path, "r");
	if (file)
		return (fclose(file), NULL);
	file = fopen(path, mode);
	return (file);	
}

bool		os_file_delete(const char *path)
{
	RETURN_IF_NULL(path, false);

	if (-1 == remove(path))
		return (false);
	return (true);
}

FILE		*os_file_open(const char *path, const char *mode)
{
	RETURN_IF_NULL(path, NULL);
	RETURN_IF_NULL(mode, NULL);

	FILE	*file = fopen(path, mode);
	return (file);
}

bool		os_file_move(const char *old_path, const char *new_path)
{
	RETURN_IF_NULL(old_path, false);
	RETURN_IF_NULL(new_path, false);

	if (-1 == rename(old_path, new_path))
		return (false);
	return (true);
}

bool		os_file_write(FILE *file, const char *data)
{
	RETURN_IF_NULL(file, false);
	RETURN_IF_NULL(data, false);

	if (0 > fprintf(file,  "%s", data))
		return (false);
	return (true);
}

char		*os_file_get_data(FILE *file)
{
	RETURN_IF_NULL(file, NULL);

	fseek(file, 0, SEEK_END);
	size_t	_size = ftell(file);
	rewind(file);

	char	*buffer = malloc((_size + 1) * sizeof(char));
	RETURN_IF_NULL(buffer, NULL);
	
	buffer[_size] = '\0';
	size_t	_read = fread(buffer, 1, _size, file);
	rewind(file);

	if (_size != _read)
		return (free(buffer), NULL);

	return (buffer);
}
