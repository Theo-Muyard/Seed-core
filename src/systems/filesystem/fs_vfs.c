#include "common/memory.h"
#include "systems/filesystem/fs_vfs.h"

// +===----- Resolving functions -----===+ //

char		*vfs_get_dir_relative_path(const t_Directory *dir)
{
	RETURN_IF_NULL(dir, NULL);

	t_Directory	*_tmp = (t_Directory *)dir;
	size_t	_size = 1;

	while (_tmp)
	{
		RETURN_IF_NULL(_tmp->dirname, NULL);

		_size += strlen(_tmp->dirname) + 1;
		_tmp = _tmp->parent;
	}

	char	*path = malloc(_size * sizeof(char));
	RETURN_IF_NULL(path, NULL);

	path[_size - 1] = '\0';
	_tmp = (t_Directory *)dir;
	size_t	_start = _size - 1;
	while (_tmp)
	{
		size_t	_len = strlen(_tmp->dirname);
		_start -= _len;
		memcpy(path + _start, _tmp->dirname, _len);
		path[--_start] = '/';
		_tmp = _tmp->parent;
	}

	return (path);
}

char		*vfs_get_file_relative_path(const t_File *file)
{
	RETURN_IF_NULL(file, NULL);

	char	*path = directory_get_relative_path(file->parent);
	RETURN_IF_NULL(path, NULL);

	size_t	_path_len = strlen(path);
	size_t	_filename_len = strlen(file->filename);
	size_t	_size = _path_len + 1 + _filename_len + 1;

	char		*_tmp = realloc(path, _size * sizeof(char));
	GOTO_IF_NULL(_tmp, exit_free_path);

	path = _tmp;
	path[_path_len++] = '/';
	memcpy(path + _path_len, file->filename, _filename_len);
	path[_path_len + _filename_len] = '\0';

	return (path);

	/* GOTO EXIT */
	exit_free_path:
		return (free(path), NULL);
}

// +===----- Directory functions -----===+ //

// TODO: Reformat functions

t_Directory	*directory_create(t_Directory *parent, const char *dirname)
{
	t_Directory	*dir;

	TEST_NULL(dirname, NULL);
	dir = malloc(sizeof(t_Directory));
	TEST_NULL(dir, NULL);
	dir->dirname = ft_strdup(dirname);
	if (NULL == dir->dirname)
		return (free(dir), NULL);
	if (NULL != parent)
	{
		TEST_ERROR_FN(
			directory_subdir_add(parent, dir),
			NULL
		);
	}
	dir->parent = parent;

	dir->files = NULL;
	dir->files_count = 0;
	dir->files_capacity = 0;

	dir->subdirs = NULL;
	dir->subdirs_count = 0;
	dir->subdirs_capacity = 0;
	return (dir);
}

void		directory_destroy(t_Directory *dir)
{
	size_t	_i;

	if (NULL == dir)
		return ;
	_i = 0;
	while (_i < dir->files_count)
	{
		file_destroy(dir->files[_i]);
		_i++;
	}
	_i = 0;
	while (_i < dir->subdirs_count)
	{
		directory_destroy(dir->subdirs[_i]);
		_i++;
	}
	free(dir->files);
	free(dir->subdirs);
	free(dir->dirname);
	free(dir);
}

// +===----- Files -----===+ //

t_File		*file_create(t_Directory *parent, const char *filename)
{
	t_File	*file;

	TEST_NULL(filename, NULL);
	file = malloc(sizeof(t_File));
	TEST_NULL(file, NULL);
	file->filename = ft_strdup(filename);
	if (NULL == file->filename)
		return (free(file), NULL);
	if (NULL != parent)
	{
		TEST_ERROR_FN(
			directory_file_add(parent, file),
			NULL
		);
	}
	return (file);
}

void		file_destroy(t_File *file)
{
	if (NULL == file)
		return ;
	free(file->filename);
	free(file);
}

bool		directory_file_add(t_Directory *dir, t_File *file)
{
	t_File	**_tmp;

	TEST_NULL(dir, false);
	TEST_NULL(file, false);
	if (dir->files_capacity == 0)
	{
		dir->files = malloc(FILE_ALLOC * sizeof(t_File *));
		TEST_NULL(dir->files, false);
		dir->files_capacity = FILE_ALLOC;
	}
	if (dir->files_count >= dir->files_capacity)
	{
		_tmp = realloc(
			dir->files,
			FILE_ALLOC + dir->files_capacity * sizeof(t_File *)
		);
		TEST_NULL(_tmp, false);
		dir->files = _tmp;
		dir->files_capacity += FILE_ALLOC;
	}
	dir->files[dir->files_count] = file;
	dir->files_count++;
	file->parent = dir;
	return (true);
}

bool		directory_file_remove(t_Directory *dir, t_File *file)
{
	size_t	_i;

	TEST_NULL(dir, false);
	TEST_NULL(file, false);

	_i = 0;
	while (_i < dir->files_count)
	{
		if (dir->files[_i] == file)
		{
			dir->files[_i] = NULL;
			memmove(
				dir->files + _i,
				dir->files + _i + 1,
				(dir->files_count - _i - 1) * sizeof(t_File *)
			);
			dir->files_count--;
			file->parent = NULL;
			return (true);
		}
		_i++;
	}
	return (false);
}

t_File	*directory_find_file(t_Directory *parent, const char *filename)
{
	size_t	_i;

	TEST_NULL(parent, NULL);
	TEST_NULL(filename, NULL);

	_i = 0;
	while (_i < parent->files_count)
	{
		if (strcmp(filename, parent->files[_i]->filename) == 0)
			return (parent->files[_i]);
		_i++;
	}
	return (NULL);
}

t_File	*file_resolve(t_Directory *root, const char *path)
{
	t_Directory	*_dir;
	t_File		*file;
	char		*_slash;
	char		*_cpy;

	TEST_NULL(root, NULL);
	TEST_NULL(path, NULL);

	_cpy = ft_strdup(path);
	TEST_NULL(_cpy, NULL);

	_slash = strrchr(_cpy, '/');
	if (NULL == _slash)
	{
		file = directory_find_file(root, _cpy);
		free(_cpy);
		return (file);
	}
	*_slash = '\0';
	_dir = directory_resolve(root, _cpy);
	if (NULL == _dir)
		return (free(_cpy), NULL);
	file = directory_find_file(_dir, _slash + 1);
	free(_cpy);
	return (file);
}

bool		directory_file_move(t_Directory *dst, t_Directory *src, t_File *file)
{
	TEST_NULL(dst, false);
	TEST_NULL(src, false);
	TEST_NULL(file, false);
	TEST_ERROR_FN(directory_file_remove(src, file), false);
	TEST_ERROR_FN(directory_file_add(dst, file), false);
	return (true);
}

bool		directory_file_rename(t_File *file, const char *filename)
{
	char	*_old_filename;
	char	*_new_filename;

	TEST_NULL(file, false);
	TEST_NULL(filename, false);
	if (strcmp(filename, file->filename) == 0)
		return (true);
	_old_filename = file->filename;
	_new_filename = ft_strdup(filename);
	TEST_NULL(_new_filename, false);
	free(_old_filename);
	file->filename = _new_filename;
	return (true);
}

bool		directory_contains_file(t_Directory *dir, t_File *file)
{
	size_t	_i;

	TEST_NULL(dir, NULL);
	TEST_NULL(file, NULL);

	_i = 0;
	while (_i < dir->files_count)
	{
		if (dir->files[_i] == file)
			return (true);
		_i++;
	}
	return (false);
}

// +===----- Sub directory -----===+ //

bool		directory_subdir_add(t_Directory *dir, t_Directory *subdir)
{
	t_Directory	**_tmp;

	TEST_NULL(dir, false);
	TEST_NULL(subdir, false);
	if (dir->subdirs_capacity == 0)
	{
		dir->subdirs = malloc(DIR_ALLOC * sizeof(t_Directory *));
		TEST_NULL(dir->subdirs, false);
		dir->subdirs_capacity = DIR_ALLOC;
	}
	if (dir->subdirs_count >= dir->subdirs_capacity)
	{
		_tmp = realloc(
			dir->subdirs,
			DIR_ALLOC + dir->subdirs_capacity * sizeof(t_Directory *)
		);
		TEST_NULL(_tmp, false);
		dir->subdirs = _tmp;
		dir->subdirs_capacity += DIR_ALLOC;
	}
	dir->subdirs[dir->subdirs_count] = subdir;
	dir->subdirs_count++;
	subdir->parent = dir;
	return (true);
}

bool		directory_subdir_remove(t_Directory *dir, t_Directory *subdir)
{
	size_t	_i;

	TEST_NULL(dir, false);
	TEST_NULL(subdir, false);

	_i = 0;
	while (_i < dir->subdirs_count)
	{
		if (dir->subdirs[_i] == subdir)
		{
			dir->subdirs[_i] = NULL;
			memmove(
				dir->subdirs + _i,
				dir->subdirs + _i + 1,
				(dir->subdirs_count - _i - 1) * sizeof(t_Directory *)
			);
			dir->subdirs_count--;
			subdir->parent = NULL;
			return (true);
		}
		_i++;
	}
	return (false);
}

t_Directory	*directory_find_subdir(t_Directory *parent, const char *dirname)
{
	size_t	_i;

	TEST_NULL(parent, NULL);
	TEST_NULL(dirname, NULL);

	_i = 0;
	while (_i < parent->subdirs_count)
	{
		if (strcmp(dirname, parent->subdirs[_i]->dirname) == 0)
			return (parent->subdirs[_i]);
		_i++;
	}
	return (NULL);
}

t_Directory	*directory_resolve(t_Directory *root, const char *path)
{
	t_Directory	*current;
	char		**_tokens;
	size_t		_i;

	TEST_NULL(root, NULL);
	TEST_NULL(path, NULL);
	
	_tokens = ft_split(path, '/');
	TEST_NULL(_tokens, NULL);
	_i = 0;
	current = root;
	while (_tokens[_i])
	{
		if (strcmp(_tokens[_i], ".") == 0)
		{
			_i++;
			continue;
		}
		else if (strcmp(_tokens[_i], "..") == 0)
		{
			current = current->parent;
			_i++;
			continue;
		}
		current = directory_find_subdir(current, _tokens[_i]);
		if (NULL == current)
			return (ft_free_split(_tokens), NULL);
		_i++;
	}
	ft_free_split(_tokens);
	return (current);
}

bool		directory_subdir_move(t_Directory *dst, t_Directory *src, t_Directory *subdir)
{
	TEST_NULL(dst, false);
	TEST_NULL(src, false);
	TEST_NULL(subdir, false);
	TEST_ERROR_FN(directory_subdir_remove(src, subdir), false);
	TEST_ERROR_FN(directory_subdir_add(dst, subdir), false);
	return (true);
}

bool		directory_subdir_rename(t_Directory *dir, const char *dirname)
{
	char	*_old_dirname;
	char	*_new_dirname;

	TEST_NULL(dir, false);
	TEST_NULL(dirname, false);
	if (strcmp(dirname, dir->dirname) == 0)
		return (true);
	_old_dirname = dir->dirname;
	_new_dirname = ft_strdup(dirname);
	TEST_NULL(_new_dirname, false);
	free(_old_dirname);
	dir->dirname = _new_dirname;
	return (true);
}

bool		directory_contains_subdir(t_Directory *dir, t_Directory *subdir)
{
	size_t	_i;

	TEST_NULL(dir, NULL);
	TEST_NULL(subdir, NULL);

	_i = 0;
	while (_i < dir->subdirs_count)
	{
		if (dir->subdirs[_i] == subdir)
			return (true);
		_i++;
	}
	return (false);
}
