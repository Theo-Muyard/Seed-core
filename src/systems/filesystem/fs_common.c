#include "systems/filesystem/fs_common.h"

char		*join_path(const char *base, const char *path)
{
	RETURN_IF_NULL(base, NULL);
	RETURN_IF_NULL(path, NULL);

	size_t	_base_len = strlen(base);
	size_t	_path_len = strlen(path);
	size_t	_len = _base_len + _path_len;
	bool		_need_slash = (_base_len > 0 && base[_base_len - 1] != '/');

	char		*new_path = malloc((_len + _need_slash + 1) * sizeof(char));
	RETURN_IF_NULL(new_path, NULL);

	memcpy(new_path, base, _base_len);
	if (_need_slash)
		new_path[_base_len++] = '/';
	memcpy(
		new_path + _base_len,
		path, _path_len
	);
	new_path[_len + _need_slash] = '\0';

	return (new_path);
}

bool		is_dir(const char *path)
{
	RETURN_IF_NULL(path, NULL);

	struct stat	_st;
	if (0 != lstat(path, &_st))
		return (false);
	return (S_ISDIR(_st.st_mode));
}

t_ErrorCode	handle_dir_error(void)
{
	switch (errno)
	{
		case EEXIST:
			return (ERR_DIR_EXIST);
		case EACCES:
			return (ERR_DIR_ACCESS);
		case ENOENT:
			return (ERR_DIR_NOT_FOUND);
		default:
			return (ERR_OPERATION_FAILED);
	}
	return (ERR_OPERATION_FAILED);
}

t_ErrorCode	handle_file_error(void)
{
	switch (errno)
	{
		case EEXIST:
			return (ERR_FILE_EXIST);
		case EACCES:
			return (ERR_FILE_ACCESS);
		case ENOENT:
			return (ERR_FILE_NOT_FOUND);
		default:
			return (ERR_OPERATION_FAILED);
	}
	return (ERR_OPERATION_FAILED);
}
