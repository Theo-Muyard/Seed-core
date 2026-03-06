#include "systems/filesystem/fs_watcher.h"
#include "common/memory.h"

t_WatchCtx	*watcher_init(const char *path)
{
	RETURN_IF_NULL(path, NULL);

	t_WatchCtx	*watcher = malloc(sizeof(t_WatchCtx));
	RETURN_IF_NULL(watcher, NULL);

	int	_fd = inotify_init();
	if (_fd < -1)
		return (NULL);

	watcher->fd = _fd;
	watcher->path = ft_strdup(path);
	GOTO_IF_NULL(watcher->path, exit_free_watcher);

	watcher->event_queue = NULL;
	watcher->event_count = 0;
	watcher->event_capacity = 0;

	watcher->entries = NULL;
	watcher->entry_count = 0;
	watcher->entry_capacity = 0;

	watcher->pending = NULL;
	watcher->pending_count = 0;
	watcher->pending_capacity = 0;

	return (watcher);

	/* GOTO EXIT */
	exit_free_watcher:
		return (free(watcher), NULL);
}

void		watcher_destroy(t_WatchCtx *ctx)
{
	if (NULL == ctx)
		return ;

	size_t	_i = 0;
	while (_i < ctx->event_count)
	{
		free(ctx->event_queue[_i]->path);
		free(ctx->event_queue[_i]->new_path);
		free(ctx->event_queue[_i]);
		_i++;
	}
	free(ctx->event_queue);

	_i = 0;
	while (_i < ctx->entry_count)
	{
		inotify_rm_watch(ctx->fd, ctx->entries[_i]->wd);
		free(ctx->entries[_i]->path);
		free(ctx->entries[_i]);
		_i++;
	}
	free(ctx->entries);

	_i = 0;
	while (_i < ctx->pending_count)
	{
		free(ctx->pending[_i]->from_path);
		free(ctx->pending[_i]);
		_i++;
	}
	free(ctx->pending);

	free(ctx->path);
	free(ctx);
}

bool		watcher_analyze(t_WatchCtx *ctx)
{
	char		*_parent_path = NULL;
	char		*_entry_path = NULL;
	t_FsEvent	*_fs_event = NULL;
	char		_buffer[4096] = NULL;

	size_t	_len = read(ctx->fd, _buffer, sizeof(_buffer));
	if (_len < 0)
		return (false);

	char		*_ptr = _buffer;
	size_t	_i = ctx->event_count;
	while (_ptr < _buffer + _len)
	{
		struct inotify_event	*_event = (struct inotify_event *)_ptr;
		_parent_path = watch_get_path(ctx, _event->wd);
		
		if (_event->mask & IN_Q_OVERFLOW || NULL == _parent_path)
			return (NULL);

		if (_event->len > 0)
		{
			_entry_path = join_path(_parent_path, _event->name);
			RETURN_IF_NULL(_entry_path, false);
		}
		else
		{
			_entry_path = ft_strdup(_parent_path);
			RETURN_IF_NULL(_entry_path, false);
		}

		_fs_event = handle_event(ctx, _event, _entry_path);
		if (NULL == _fs_event)
		{
			free(_entry_path);
			_ptr += sizeof(struct inotify_event) + _event->len;
			continue ;
		}

		GOTO_IF_FALSE(
			queue_reserve(ctx, ctx->event_count + 1),
			exit_free_path_and_event
		);

		_fs_event = handle_event(ctx, _event, _entry_path);
		GOTO_IF_NULL(_fs_event, exit_free_path_and_event);

		ctx->event_queue[_i] = _fs_event;
		free(_entry_path);
		_ptr += sizeof(struct inotify_event) + _event->len;
		ctx->event_count++;
		_i++;
	}

	RETURN_IF_FALSE(flush_pending(ctx), false);
	return (true);

	/* GOTO EXIT */
	exit_free_path_and_event:
		free(_entry_path);
		free(_fs_event);
		return (false);
}

// TODO: delete
// void print_event(t_FsEvent *event)
// {
// 	if (event->type == FS_EVENT_CREATE)
// 	{
// 		printf("-> EVENT: CREATE (%s)\n-> Path: %s\n", event->isdir ? "dir" : "file", event->path);
// 		return ;
// 	}
// 	if (event->type == FS_EVENT_DELETE)
// 	{
// 		printf("-> EVENT: DELETE (%s)\n-> Path: %s\n", event->isdir ? "dir" : "file", event->path);
// 		return ;
// 	}
// 	if (event->type == FS_EVENT_MOVE)
// 	{
// 		printf("-> EVENT: MOVE (%s)\n-> Old path: %s\n-> New path: %s\n", event->isdir ? "dir" : "file", event->path, event->new_path);
// 		return ;
// 	}
// 	if (event->type == FS_EVENT_OVERFLOW)
// 	{
// 		printf("-> EVENT: OVERFLOW (%s)\n", event->isdir ? "dir" : "file");
// 		return ;
// 	}
// }

// int	main(void)
// {
// 	t_WatchCtx	*_ctx;

// 	_ctx = watcher_init("/home/theo/Documents/Development/seed-core/tmp");
// 	if (NULL == _ctx)
// 		return (printf("Folder not found.\n"), 1);
// 	if (false == watch_add_recursive(_ctx, _ctx->path))
// 		return (watcher_destroy(_ctx), 1);
// 	printf("Watching...\n");
// 	int	count = 0;
// 	while (count < 10)
// 	{
// 		if (false == watcher_analyze(_ctx))
// 			return (printf("Watcher error\n"), watcher_destroy(_ctx), 1);
// 		printf("\n=== QUEUE ===\n");
// 		size_t	_i = 0;
// 		while (_i < _ctx->event_count)
// 		{
// 			print_event(_ctx->event_queue[_i]);
// 			printf("\n");
// 			_i++;
// 		}
// 		printf("\n=== END QUEUE ===\n");
// 		count++;
// 	}
// 	watcher_destroy(_ctx);
// 	return (0);
// }
