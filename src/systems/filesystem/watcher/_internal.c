#include "systems/filesystem/vfs/_internal.h"
#include "systems/filesystem/watcher/_internal.h"
#include "tools/memory.h"

// +===----- Static functions -----===+ //

/**
 * @brief Add a new watch entry.
 * @param ctx The watcher context.
 * @param path The absolute path of the entry.
 * @return TRUE for success or FALSE if an error occured.
*/
static bool	watch_add_one(t_WatchCtx *ctx, const char *path)
{
	int				_wd;
	t_WatchEntry	*_entry;

	_wd = inotify_add_watch(ctx->fd, path, WATCH_MASK);
	if (_wd < 0)
		return (false);
	if (false == entries_reserve(ctx, ctx->entry_count + 1))
		return (inotify_rm_watch(ctx->fd, _wd), false);
	_entry = malloc(sizeof(t_WatchEntry));
	TEST_NULL(_entry, false);
	_entry->wd = _wd;
	_entry->path = ft_strdup(path);
	if (NULL == _entry->path)
		return (inotify_rm_watch(ctx->fd, _wd), false);
	ctx->entries[ctx->entry_count] = _entry;
	ctx->entry_count++;
	return (true);
}

/**
 * @brief Remove a watch entry by this wd.
 * @param ctx The watch context.
 * @param wd The wd of the entry.
*/
static void	watch_remove_one(t_WatchCtx *ctx, int wd)
{
	size_t			_i;
	t_WatchEntry	*_entry;

	_i = 0;
	while (_i < ctx->entry_count)
	{
		_entry = ctx->entries[_i];
		if (_entry->wd == wd)
		{
			free(_entry->path);
			free(_entry);
			memmove(
				ctx->entries + _i,
				ctx->entries + _i + 1,
				(ctx->entry_count - _i - 1) * sizeof(t_WatchEntry *)
			);
		}
		_i++;
	}
}

// +===----- Functions -----===+ //

bool		is_dir(const char *path)
{
	struct stat	_st;

	TEST_NULL(path, false);
	if (0 != lstat(path, &_st))
		return (false);
	return (S_ISDIR(_st.st_mode));
}

char		*watch_get_path(t_WatchCtx *ctx, int wd)
{
	size_t			_i;
	t_WatchEntry	*_entry;

	_i = 0;
	while (_i < ctx->entry_count)
	{
		_entry = ctx->entries[_i];
		if (_entry->wd == wd)
			return (_entry->path);
		_i++;
	}
	return (NULL);
}

int			watch_get_wd(t_WatchCtx *ctx, const char *path)
{
	size_t			_i;
	t_WatchEntry	*_entry;

	_i = 0;
	while (_i < ctx->entry_count)
	{
		_entry = ctx->entries[_i];
		if (_entry->path == path)
			return (_entry->wd);
		_i++;
	}
	return (-1);
}

bool		queue_reserve(t_WatchCtx *ctx, size_t new_count)
{
	t_FsEvent		**_tmp;
	size_t			_new_cap;
	
	if (new_count <= ctx->event_capacity)
		return (true);
	if (0 == ctx->event_capacity)
	{
		ctx->event_queue = malloc(EVENT_ALLOC * sizeof(t_FsEvent *));
		TEST_NULL(ctx->event_queue, false);
		ctx->event_capacity = EVENT_ALLOC;
	}
	else
	{
		_new_cap = ctx->event_capacity;
		while (_new_cap < new_count)
			_new_cap *= 2;
		_tmp = realloc(ctx->event_queue, _new_cap * sizeof(t_FsEvent *));
		TEST_NULL(_tmp, false);
		ctx->event_queue = _tmp;
		ctx->event_capacity = _new_cap;
	}
	return (true);
}

bool		pending_reserve(t_WatchCtx *ctx, size_t new_count)
{
	t_MovePending	**_tmp;
	size_t			_new_cap;
	
	if (new_count <= ctx->pending_capacity)
		return (true);
	if (0 == ctx->pending_capacity)
	{
		ctx->pending = malloc(PENDING_ALLOC * sizeof(t_MovePending *));
		TEST_NULL(ctx->pending, false);
		ctx->pending_capacity = PENDING_ALLOC;
	}
	else
	{
		_new_cap = ctx->pending_capacity;
		while (_new_cap < new_count)
			_new_cap *= 2;
		_tmp = realloc(ctx->pending, _new_cap * sizeof(t_MovePending *));
		TEST_NULL(_tmp, false);
		ctx->pending = _tmp;
		ctx->pending_capacity = _new_cap;
	}
	return (true);
}

bool		entries_reserve(t_WatchCtx *ctx, size_t new_count)
{
	t_WatchEntry	**_tmp;
	size_t			_new_cap;
	
	if (new_count <= ctx->entry_capacity)
		return (true);
	if (0 == ctx->entry_capacity)
	{
		ctx->entries = malloc(ENTRY_ALLOC * sizeof(t_WatchEntry *));
		TEST_NULL(ctx->entries, false);
		ctx->entry_capacity = ENTRY_ALLOC;
	}
	else
	{
		_new_cap = ctx->entry_capacity;
		while (_new_cap < new_count)
			_new_cap *= 2;
		_tmp = realloc(ctx->entries, _new_cap * sizeof(t_WatchEntry *));
		TEST_NULL(_tmp, false);
		ctx->entries = _tmp;
		ctx->entry_capacity = _new_cap;
	}
	return (true);
}

bool		watch_add_recursive(t_WatchCtx *ctx, const char *path)
{
	DIR				*_dir;
	struct dirent	*_entry;
	char			*_child_path;

	if (!is_dir(path))
		return (true);
	if (false == watch_add_one(ctx, path))
		return (false);
	_dir = opendir(path);
	TEST_NULL(_dir, false);
	while ((_entry = readdir(_dir)) != NULL)
	{
		if (strcmp(_entry->d_name, ".") == 0 || strcmp(_entry->d_name, "..") == 0)
			continue ;
		_child_path = join_path(path, _entry->d_name);
		TEST_NULL(_child_path, false);
		if (is_dir(_child_path) && false == watch_add_recursive(ctx, _child_path))
			return (free(_child_path), closedir(_dir), false);
		free(_child_path);
	}
	closedir(_dir);
	return (true);
}

bool		watch_remove_recursive(t_WatchCtx *ctx, const char *path)
{
	DIR				*_dir;
	struct dirent	*_entry;
	char			*_child_path;
	int				_wd;

	if (!is_dir(path))
		return (true);
	_wd = watch_get_wd(ctx, path);
	if (_wd < 0)
		return (false);
	watch_remove_one(ctx, _wd);
	_dir = opendir(path);
	TEST_NULL(_dir, false);
	while ((_entry = readdir(_dir)) != NULL)
	{
		if (strcmp(_entry->d_name, ".") == 0 || strcmp(_entry->d_name, "..") == 0)
			continue ;
		_child_path = join_path(path, _entry->d_name);
		TEST_NULL(_child_path, false);
		if (is_dir(_child_path))
			watch_remove_recursive(ctx, _child_path);
		free(_child_path);
	}
	closedir(_dir);
	return (true);
}

t_FsEvent	*handle_event
(
	t_WatchCtx	*ctx,
	struct inotify_event *event,
	const char *path
)
{
	t_FsEvent	*ev;
	bool		handled;

	ev = malloc(sizeof(t_FsEvent));
	TEST_NULL(ev, NULL);
	ev->path = ft_strdup(path);
	if (NULL == ev->path)
		return (free(ev), NULL);
	ev->new_path = NULL;
	ev->isdir = false;
	ev->type = FS_EVENT_OVERFLOW;
	handled = false;
	if ((event->mask & IN_CREATE) && (event->mask & IN_ISDIR))
	{
		printf("Create dir: %s\n", path);
		if (false == watch_add_recursive(ctx, path))
			return (free(ev->path), free(ev), NULL);
		ev->isdir = true;
		ev->type = FS_EVENT_CREATE;
		handled = true;
	}
	else if (event->mask & IN_CREATE)
	{
		printf("Create file: %s\n", path);
		ev->type = FS_EVENT_CREATE;
		handled = true;
	}
	if ((event->mask & IN_DELETE) && (event->mask & IN_ISDIR))
	{
		printf("Delete dir: %s\n", path);
		if (false == watch_remove_recursive(ctx, path))
			return (free(ev->path), free(ev), NULL);
		ev->isdir = true;
		ev->type = FS_EVENT_DELETE;
		handled = true;
	}
	else if (event->mask & IN_DELETE)
	{
		printf("Delete file: %s\n", path);
		ev->type = FS_EVENT_DELETE;
		handled = true;
	}
	if ((event->mask & IN_MOVED_FROM) &&  (event->mask & IN_ISDIR))
	{
		printf("Move from dir: %s\n", path);
		ev->isdir = true;
		ev->type = FS_EVENT_MOVE;
		handled = true;
	}
	else if (event->mask & IN_MOVED_FROM)
	{
		printf("Move from file: %s\n", path);
		ev->type = FS_EVENT_MOVE;
		handled = true;
	}
	if ((event->mask & IN_MOVED_TO) && (event->mask & IN_ISDIR))
	{
		printf("Move to dir: %s\n", path);
		ev->isdir = true;
		ev->type = FS_EVENT_MOVE;
		handled = true;
	}
	else if (event->mask & IN_MOVED_TO)
	{
		printf("Move to file: %s\n", path);
		ev->type = FS_EVENT_MOVE;
		handled = true;
	}
	if (false == handled)
		return (free(ev->path), free(ev), NULL);
	return (ev);
}
