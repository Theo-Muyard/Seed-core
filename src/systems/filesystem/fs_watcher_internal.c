#include "systems/filesystem/fs_watcher_internal.h"
#include "systems/filesystem/fs_common.h"
#include "common/memory.h"

// +===----- Static functions -----===+ //

/**
 * @brief Add a new watch entry in the watch context.
 * 
 * @param ctx The context must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` or `path` is NULL or an error occurred.
*/
static bool	watch_add_one(t_WatchCtx *ctx, const char *path)
{
	RETURN_IF_NULL(ctx, false);
	RETURN_IF_NULL(path, false);

	int	_wd = inotify_add_watch(ctx->fd, path, WATCH_MASK);
	if (_wd < 0)
		return (false);

	GOTO_IF_FALSE(
		entries_reserve(ctx, ctx->entry_count + 1),
		exit_inotify_rm_watch
	);
	
	t_WatchEntry	*_entry = malloc(sizeof(t_WatchEntry));
	GOTO_IF_NULL(_entry, exit_inotify_rm_watch);

	_entry->path = ft_strdup(path);
	GOTO_IF_NULL(_entry->path, exit_inotify_rm_watch);
	_entry->wd = _wd;

	ctx->entries[ctx->entry_count] = _entry;
	ctx->entry_count++;

	return (true);

	/* GOTO EXIT */
	exit_inotify_rm_watch:
		return (inotify_rm_watch(ctx->fd, _wd), false);
}

/**
 * @brief Remove a watch entry by its wd.
 *
 * @param ctx The context must not be NULL.
 * @param wd The entry wd.
*/
static void	watch_remove_one(t_WatchCtx *ctx, int wd)
{
	if (NULL == ctx || wd < 0)
		return ;

	size_t	_i = 0;
	while (_i < ctx->entry_count)
	{
		t_WatchEntry	*_entry = ctx->entries[_i];
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

/**
 * @brief Add a new pending in the watch context.
 * 
 * @param ctx The context must not be NULL.
 * @param cookie The inotify cookie.
 * @param path The path must not be NULL.
 * @param isdir Pending is a folder?
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` or `path` is NULL or an error occurred.
*/
static bool	pending_add_one(
	t_WatchCtx *ctx, uint32_t cookie,
	const char *path, bool isdir
)
{
	RETURN_IF_NULL(ctx, false);
	RETURN_IF_NULL(path, false);

	RETURN_IF_FALSE(
		pending_reserve(ctx, ctx->pending_count + 1),
		false
	);

	t_MovePending	*_pending = malloc(sizeof(t_MovePending));
	RETURN_IF_NULL(_pending, false);

	_pending->from_path = ft_strdup(path);
	GOTO_IF_NULL(_pending, exit_free_pending);

	_pending->cookie = cookie;
	_pending->is_dir = isdir;
	ctx->pending[ctx->pending_count] = _pending;
	ctx->pending_count++;

	return (true);

	/* GOTO EXIT */
	exit_free_pending:
		return (free(_pending), false);
}

/**
 * @brief Remove a pending by its cookie.
 * 
 * @param ctx The context must not be NULL.
 * @param cookie The inotify cookie.
*/
static void	pending_remove_one(t_WatchCtx *ctx, uint32_t cookie)
{
	if (NULL == ctx)
		return ;

	size_t	_i = 0;
	while (_i < ctx->pending_count)
	{
		t_MovePending	*_pending = ctx->pending[_i];
		if (_pending->cookie == cookie)
		{
			free(_pending->from_path);
			free(_pending);
			memmove(
				ctx->pending + _i,
				ctx->pending + _i + 1,
				(ctx->pending_count - _i - 1) * sizeof(t_MovePending *)
			);
			ctx->pending_count--;
		}
		_i++;
	}
}

/**
 * @brief Remove a move pending.
 * 
 * @param ctx The context must not be NULL.
 * @param cookie The inotify cookie.
 * 
 * @retval The pending.
 * @retval NULL if `ctx` is NULL, pending not found or an error occurred.
*/
static t_MovePending	*pending_find_one(t_WatchCtx *ctx, uint32_t cookie)
{
	RETURN_IF_NULL(ctx, NULL);

	size_t	_i = 0;
	while (_i < ctx->pending_count)
	{
		t_MovePending	*_pending = ctx->pending[_i];
		if (_pending->cookie == cookie)
			return (_pending);
		_i++;
	}

	return (NULL);
}

// +===----- Functions -----===+ //

char		*watch_get_path(t_WatchCtx *ctx, int wd)
{
	RETURN_IF_NULL(ctx, NULL);
	if (wd < 0)
		return (NULL);

	size_t	_i = 0;
	while (_i < ctx->entry_count)
	{
		t_WatchEntry	*_entry = ctx->entries[_i];
		if (_entry->wd == wd)
			return (_entry->path);
		_i++;
	}

	return (NULL);
}

int		watch_get_wd(t_WatchCtx *ctx, const char *path)
{
	RETURN_IF_NULL(ctx, -1);
	RETURN_IF_NULL(path, -1);

	size_t	_i = 0;
	while (_i < ctx->entry_count)
	{
		t_WatchEntry	*_entry = ctx->entries[_i];
		if (strcmp(_entry->path, path) == 0)
			return (_entry->wd);
		_i++;
	}
	return (-1);
}

bool		queue_reserve(t_WatchCtx *ctx, size_t new_count)
{
	RETURN_IF_NULL(ctx, false);

	if (new_count <= ctx->event_capacity)
		return (true);

	if (0 == ctx->event_capacity)
	{
		ctx->event_queue = malloc(EVENT_ALLOC * sizeof(t_FsEvent *));
		RETURN_IF_NULL(ctx->event_queue, false);

		ctx->event_capacity = EVENT_ALLOC;
	}
	else
	{
		size_t	_new_cap = ctx->event_capacity;
		while (_new_cap < new_count)
			_new_cap *= 2;

		t_FsEvent	**_tmp = realloc(ctx->event_queue, _new_cap * sizeof(t_FsEvent *));
		RETURN_IF_NULL(_tmp, false);

		ctx->event_queue = _tmp;
		ctx->event_capacity = _new_cap;
	}

	return (true);
}

bool		pending_reserve(t_WatchCtx *ctx, size_t new_count)
{
	RETURN_IF_NULL(ctx, false);

	if (new_count <= ctx->pending_capacity)
		return (true);

	if (0 == ctx->pending_capacity)
	{
		ctx->pending = malloc(EVENT_ALLOC * sizeof(t_MovePending *));
		RETURN_IF_NULL(ctx->pending, false);

		ctx->pending_capacity = EVENT_ALLOC;
	}
	else
	{
		size_t	_new_cap = ctx->pending_capacity;
		while (_new_cap < new_count)
			_new_cap *= 2;

		t_MovePending	**_tmp = realloc(ctx->pending, _new_cap * sizeof(t_MovePending *));
		RETURN_IF_NULL(_tmp, false);

		ctx->pending = _tmp;
		ctx->pending_capacity = _new_cap;
	}

	return (true);
}

bool		entries_reserve(t_WatchCtx *ctx, size_t new_count)
{
	RETURN_IF_NULL(ctx, false);

	if (new_count <= ctx->entry_capacity)
		return (true);

	if (0 == ctx->entry_capacity)
	{
		ctx->entries = malloc(EVENT_ALLOC * sizeof(t_WatchEntry *));
		RETURN_IF_NULL(ctx->entries, false);

		ctx->entry_capacity = EVENT_ALLOC;
	}
	else
	{
		size_t	_new_cap = ctx->entry_capacity;
		while (_new_cap < new_count)
			_new_cap *= 2;

		t_WatchEntry	**_tmp = realloc(ctx->entries, _new_cap * sizeof(t_WatchEntry *));
		RETURN_IF_NULL(_tmp, false);

		ctx->entries = _tmp;
		ctx->entry_capacity = _new_cap;
	}

	return (true);
}

bool		watch_add_recursive(t_WatchCtx *ctx, const char *path)
{
	RETURN_IF_NULL(ctx, false);
	RETURN_IF_NULL(path, false);

	RETURN_IF_FALSE(is_dir(path), false);
	RETURN_IF_FALSE(watch_add_one(ctx, path), false);

	DIR	*_dir = opendir(path);
	GOTO_IF_NULL(_dir, exit_watch_remove);

	char	*_child_path = NULL;
	struct dirent	*_entry;
	while (NULL != (_entry = readdir(_dir)))
	{
		if (strcmp(_entry->d_name, ".") == 0 || strcmp(_entry->d_name, "..") == 0)
			continue ;

		_child_path = join_path(path, _entry->d_name);
		GOTO_IF_NULL(_child_path, exit_free_and_close);

		if (is_dir(_child_path))
			GOTO_IF_FALSE(watch_add_recursive(ctx, _child_path), exit_free_and_close);
		free(_child_path);
	}

	closedir(_dir);
	return (true);

	/* GOTO EXIT */
	exit_watch_remove:
		int	_wd = watch_get_wd(ctx, path);
		return (watch_remove_one(ctx, _wd), false);

	exit_free_and_close:
		return (free(_child_path), closedir(_dir), false);
}

bool		watch_remove_recursive(t_WatchCtx *ctx, const char *path)
{
	RETURN_IF_NULL(ctx, false);
	RETURN_IF_NULL(path, false);
	RETURN_IF_FALSE(is_dir(path), false);

	int	_wd = watch_get_wd(ctx, path);
	if (_wd < 0)
		return (false);
	watch_remove_one(ctx, _wd);

	DIR	*_dir = opendir(path);
	RETURN_IF_NULL(_dir, false);

	char	*_child_path = NULL;
	struct dirent	*_entry;
	while (NULL != (_entry = readdir(_dir)))
	{
		if (strcmp(_entry->d_name, ".") == 0 || strcmp(_entry->d_name, "..") == 0)
			continue ;

		_child_path = join_path(path, _entry->d_name);
		GOTO_IF_NULL(_child_path, exit_free_and_close);

		if (is_dir(_child_path))
			GOTO_IF_FALSE(watch_remove_recursive(ctx, _child_path), exit_free_and_close);
		free(_child_path);
	}

	closedir(_dir);
	return (true);

	/* GOTO EXIT */
	exit_free_and_close:
		return (free(_child_path), closedir(_dir), false);
}

bool		flush_pending(t_WatchCtx *ctx)
{
	RETURN_IF_NULL(ctx, false);

	t_FsEvent	*_event = NULL;
	while (ctx->pending_count > 0)
	{
		t_MovePending	*_pending = ctx->pending[0];
		RETURN_IF_NULL(_pending, false);

		RETURN_IF_FALSE(queue_reserve(ctx, ctx->event_count + 1), false);
		
		_event = malloc(sizeof(t_FsEvent));
		RETURN_IF_NULL(_event, false);

		_event->path = ft_strdup(_pending->from_path);
		GOTO_IF_NULL(_event->path, exit_free_event);

		_event->type = FS_EVENT_DELETE;
		_event->isdir = _pending->is_dir;
		_event->new_path = NULL;

		ctx->event_queue[ctx->event_count++] = _event;
		memmove(
			ctx->pending,
			ctx->pending + 1,
			(ctx->pending_count - 1) * sizeof(t_MovePending *)
		);

		ctx->pending_count--;
		free(_pending->from_path);
		free(_pending);
	}

	return (true);

	/* GOTO EXIT */
	exit_free_event:
		return (free(_event), false);
}

t_FsEvent	*handle_event
(
	t_WatchCtx	*ctx,
	struct inotify_event *event, const char *path
)
{
	t_MovePending	*_pending = NULL;
	if (event->mask & IN_MOVED_FROM)
	{
		pending_add_one(ctx, event->cookie, path, true);
		return (NULL);
	}

	t_FsEvent	*ev = malloc(sizeof(t_FsEvent));
	RETURN_IF_NULL(ev, NULL);

	ev->type = FS_EVENT_OVERFLOW;
	ev->path = NULL;
	ev->new_path = NULL;
	ev->isdir = false;

	if (event->mask & IN_MOVED_TO)
	{
		if (event->mask & IN_ISDIR)
			ev->isdir = true;

		ev->new_path = ft_strdup(path);
		GOTO_IF_NULL(ev->new_path, exit_free_event);

		t_MovePending	*_pending = pending_find_one(ctx, event->cookie);
		if (NULL == _pending)
		{
			ev->type = FS_EVENT_CREATE;
			return (ev);
		}

		ev->type = FS_EVENT_MOVE;
		ev->path = ft_strdup(_pending->from_path);
		GOTO_IF_NULL(ev->path, exit_free_event);

		pending_remove_one(ctx, event->cookie);		
		return (ev);
	}

	ev->path = ft_strdup(path);
	GOTO_IF_NULL(ev->path, exit_free_event);

	if ((event->mask & IN_MOVE_SELF) || (event->mask & IN_DELETE_SELF))
	{
		ev->isdir = true;
		ev->type = FS_EVENT_DELETE;
		return (ev);
	}

	if (event->mask & IN_CREATE)
	{
		if (event->mask & IN_ISDIR)
		{
			GOTO_IF_FALSE(
				watch_add_recursive(ctx, path),
				exit_free_event
			);
			ev->isdir = true;
		}

		ev->type = FS_EVENT_CREATE;
		return (ev);
	}

	if (event->mask & IN_DELETE)
	{
		if (event->mask & IN_ISDIR)
		{
			GOTO_IF_FALSE(
				watch_remove_recursive(ctx, path),
				exit_free_event
			);
			ev->isdir = true;
		}

		ev->type = FS_EVENT_DELETE;
		return (ev);
	}

	goto exit_free_event;

	/* GOTO EXIT */
	exit_free_event:
		free(ev->path);
		free(ev);
		return (NULL);
}
