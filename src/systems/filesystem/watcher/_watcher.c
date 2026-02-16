#include "filesystem/watcher/_internal.h"
#include "filesystem/watcher/_watcher.h"
#include "filesystem/vfs/_internal.h"
#include "tools/memory.h"

t_WatchCtx	*watcher_init(const char *path)
{
	t_WatchCtx	*watcher;
	int			_fd;

	watcher = malloc(sizeof(t_WatchCtx));
	TEST_NULL(watcher, NULL);

	_fd = inotify_init();
	if (_fd < -1)
		return (NULL);
	watcher->fd = _fd;
	watcher->path = ft_strdup(path);
	if (NULL == watcher->path)
		return (free(watcher), NULL);
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
}

void		watcher_destroy(t_WatchCtx *ctx)
{
	size_t	_i;

	if (NULL == ctx)
		return ;

	_i = 0;
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
	size_t					_i;
	struct inotify_event	*_event;
	char					_buffer[4096];
	char					*_ptr;
	ssize_t					_len;
	char					*_parent_path;
	char					*_entry_path;
	t_FsEvent				*_fs_event;

	_len = read(ctx->fd, _buffer, sizeof(_buffer));
	if (_len < 0)
		return (false);
	_ptr = _buffer;
	_i = ctx->event_count;
	while (_ptr < _buffer + _len)
	{
		_event = (struct inotify_event *)_ptr;
		_parent_path = watch_get_path(ctx, _event->wd);
		if (_event->mask & IN_Q_OVERFLOW || NULL == _parent_path)
			return (NULL);
		_entry_path = join_path(_parent_path, _event->name);
		TEST_NULL(_entry_path, NULL);
		_fs_event = handle_event(ctx, _event, _entry_path);
		if (NULL == _fs_event)
		{
			free(_entry_path);
			_ptr += sizeof(struct inotify_event) + _event->len;
			continue ;
		}
		if (false == queue_reserve(ctx, ctx->event_count + 1))
			return (free(_entry_path), free(_fs_event->path), free(_fs_event), false);
		if (NULL == _fs_event)
			return (free(_entry_path), false);
		ctx->event_queue[_i] = _fs_event;
		free(_entry_path);
		_ptr += sizeof(struct inotify_event) + _event->len;
		ctx->event_count++;
		_i++;
	}
	return (true);
}

int	main(void)
{
	t_WatchCtx	*_ctx;
	int			_fd;

	_ctx = watcher_init("/home/tmuyard/Seed/src/backend/tmp");
	if (false == watch_add_recursive(_ctx, _ctx->path))
		return (watcher_destroy(_ctx), 1);
	printf("Watching...\n");
	int	count = 0;
	while (count < 3)
	{
		if (false == watcher_analyze(_ctx))
			return (printf("Watcher error\n"), watcher_destroy(_ctx), 1);
		printf("\n=== QUEUE ===\n");
		int	_i = 0;
		while (_i < _ctx->event_count)
		{
			printf("-> EVENT: %d (%d)\n", _ctx->event_queue[_i]->type, _ctx->event_queue[_i]->isdir);
			_i++;
		}
		printf("\n=== END QUEUE ===\n");
		count++;
	}
	watcher_destroy(_ctx);
	return (0);
}