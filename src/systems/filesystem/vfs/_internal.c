#include "tools/memory.h"
#include "systems/filesystem/vfs/_internal.h"

// TODO: delete
static void	print_vfs_tree_node(const t_Directory *dir, const char *prefix, bool is_last)
{
	size_t			i;
	size_t			total;
	char			next_prefix[1024];
	const char		*name;

	if (NULL == dir)
		return ;
	name = dir->dirname;
	if (NULL == name || '\0' == name[0])
		name = ".";
	if (NULL == prefix)
		printf("%s\n", name);
	else
		printf("%s%s%s\n", prefix, is_last ? "`-- " : "|-- ", name);
	if (NULL == prefix)
		snprintf(next_prefix, sizeof(next_prefix), "");
	else
		snprintf(next_prefix, sizeof(next_prefix), "%s%s", prefix, is_last ? "    " : "|   ");
	total = dir->subdir_count + dir->files_count;
	i = 0;
	while (i < dir->subdir_count)
	{
		print_vfs_tree_node(dir->subdir[i], next_prefix, (i + 1 == total));
		i++;
	}
	i = 0;
	while (i < dir->files_count)
	{
		printf("%s%s%s\n", next_prefix, (dir->subdir_count + i + 1 == total)
			? "`-- " : "|-- ", dir->files[i]->filename);
		i++;
	}
}

// +===----- Path -----===+ //

char	*join_path(const char *base, const char *path)
{
	char	*joined_path;
	size_t	_base_len;
	size_t	_path_len;
	size_t	_len;
	int		_need_slash;

	TEST_NULL(base, NULL);
	TEST_NULL(path, NULL);

	_base_len = strlen(base);
	_path_len = strlen(path);
	_len = _base_len + _path_len;
	_need_slash = (_base_len > 0 && base[_base_len - 1] != '/');
	joined_path = malloc((_len + _need_slash + 1) * sizeof(char));
	TEST_NULL(joined_path, NULL);
	memcpy(joined_path, base, _base_len);
	if (_need_slash)
		joined_path[_base_len++] = '/';
	memcpy(
		joined_path + _base_len,
		path,
		_path_len
	);
	joined_path[_len + _need_slash] = '\0';
	return (joined_path);
}

char		*directory_get_relative_path(const t_Directory *dir)
{
	t_Directory	*_tmp;
	char		*path;
	size_t		_size;
	size_t		_len;
	size_t		_start;

	TEST_NULL(dir, NULL);
	_tmp = (t_Directory *)dir;
	_size = 1;
	while (_tmp)
	{
		TEST_NULL(_tmp->dirname, NULL);
		_size += strlen(_tmp->dirname) + 1;
		_tmp = _tmp->parent;
	}
	path = malloc(_size * sizeof(char));
	TEST_NULL(path, NULL);
	path[_size - 1] = '\0';
	_tmp = (t_Directory *)dir;
	_start = _size - 1;
	while (_tmp)
	{
		_len = strlen(_tmp->dirname);
		_start -= _len;
		memcpy(path + _start, _tmp->dirname, _len);
		path[--_start] = '/';
		_tmp = _tmp->parent;
	}
	return (path);
}

char		*file_get_relative_path(const t_File *file)
{
	char		*path;
	char		*_tmp;
	size_t		_size;
	size_t		_path_len;
	size_t		_filename_len;

	TEST_NULL(file, NULL);
	path = directory_get_relative_path(file->parent);
	TEST_NULL(path, NULL);
	_path_len = strlen(path);
	_filename_len = strlen(file->filename);
	_size = _path_len + 1 + _filename_len + 1;
	_tmp = realloc(path, _size * sizeof(char));
	if (NULL == _tmp)
		return (free(path), NULL);
	path = _tmp;
	path[_path_len++] = '/';
	memcpy(path + _path_len, file->filename, _filename_len);
	path[_path_len + _filename_len] = '\0';
	return (path);
}

// +===----- Directory -----===+ //

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

	dir->subdir = NULL;
	dir->subdir_count = 0;
	dir->subdir_capacity = 0;
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
	while (_i < dir->subdir_count)
	{
		directory_destroy(dir->subdir[_i]);
		_i++;
	}
	free(dir->files);
	free(dir->subdir);
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
	if (dir->subdir_capacity == 0)
	{
		dir->subdir = malloc(DIR_ALLOC * sizeof(t_Directory *));
		TEST_NULL(dir->subdir, false);
		dir->subdir_capacity = DIR_ALLOC;
	}
	if (dir->subdir_count >= dir->subdir_capacity)
	{
		_tmp = realloc(
			dir->subdir,
			DIR_ALLOC + dir->subdir_capacity * sizeof(t_Directory *)
		);
		TEST_NULL(_tmp, false);
		dir->subdir = _tmp;
		dir->subdir_capacity += DIR_ALLOC;
	}
	dir->subdir[dir->subdir_count] = subdir;
	dir->subdir_count++;
	subdir->parent = dir;
	return (true);
}

bool		directory_subdir_remove(t_Directory *dir, t_Directory *subdir)
{
	size_t	_i;

	TEST_NULL(dir, false);
	TEST_NULL(subdir, false);

	_i = 0;
	while (_i < dir->subdir_count)
	{
		if (dir->subdir[_i] == subdir)
		{
			dir->subdir[_i] = NULL;
			memmove(
				dir->subdir + _i,
				dir->subdir + _i + 1,
				(dir->subdir_count - _i - 1) * sizeof(t_Directory *)
			);
			dir->subdir_count--;
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
	while (_i < parent->subdir_count)
	{
		if (strcmp(dirname, parent->subdir[_i]->dirname) == 0)
			return (parent->subdir[_i]);
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
	while (_i < dir->subdir_count)
	{
		if (dir->subdir[_i] == subdir)
			return (true);
		_i++;
	}
	return (false);
}
