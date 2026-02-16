#include "systems/filesystem/_watcher.h"
#include "systems/filesystem/_internal.h"

#define WATCH_MASK (IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM \
	| IN_MOVED_TO | IN_ATTRIB | IN_DELETE_SELF | IN_MOVE_SELF \
	| IN_Q_OVERFLOW | IN_UNMOUNT | IN_IGNORED)

static char	*xstrdup(const char *s)
{
	char	*cpy;

	if (NULL == s)
		return (NULL);
	cpy = malloc(strlen(s) + 1);
	if (NULL == cpy)
		return (NULL);
	strcpy(cpy, s);
	return (cpy);
}

static bool	is_dir_path(const char *path)
{
	struct stat	st;

	if (NULL == path)
		return (false);
	if (0 != lstat(path, &st))
		return (false);
	return (S_ISDIR(st.st_mode));
}

static bool	is_within_root(const t_WatchCtx *ctx, const char *path)
{
	size_t	root_len;

	if (NULL == ctx || NULL == ctx->root_path || NULL == path)
		return (false);
	root_len = strlen(ctx->root_path);
	if (0 != strncmp(path, ctx->root_path, root_len))
		return (false);
	return (path[root_len] == '\0' || path[root_len] == '/');
}

static bool	entries_reserve(t_WatchCtx *ctx, size_t new_count)
{
	t_WatchEntry	*tmp;
	size_t			new_cap;

	if (new_count <= ctx->entry_capacity)
		return (true);
	new_cap = ctx->entry_capacity;
	if (0 == new_cap)
		new_cap = 32;
	while (new_cap < new_count)
		new_cap *= 2;
	tmp = realloc(ctx->entries, new_cap * sizeof(t_WatchEntry));
	if (NULL == tmp)
		return (false);
	ctx->entries = tmp;
	ctx->entry_capacity = new_cap;
	return (true);
}

static bool	pending_reserve(t_WatchCtx *ctx, size_t new_count)
{
	t_MovePending	*tmp;
	size_t			new_cap;

	if (new_count <= ctx->pending_capacity)
		return (true);
	new_cap = ctx->pending_capacity;
	if (0 == new_cap)
		new_cap = 16;
	while (new_cap < new_count)
		new_cap *= 2;
	tmp = realloc(ctx->pending, new_cap * sizeof(t_MovePending));
	if (NULL == tmp)
		return (false);
	ctx->pending = tmp;
	ctx->pending_capacity = new_cap;
	return (true);
}

static const char	*watch_get_path(const t_WatchCtx *ctx, int wd)
{
	size_t	i;

	i = 0;
	while (i < ctx->entry_count)
	{
		if (ctx->entries[i].wd == wd)
			return (ctx->entries[i].path);
		i++;
	}
	return (NULL);
}

static void	watch_remove_by_wd(t_WatchCtx *ctx, int wd)
{
	size_t	i;

	i = 0;
	while (i < ctx->entry_count)
	{
		if (ctx->entries[i].wd == wd)
		{
			free(ctx->entries[i].path);
			ctx->entries[i] = ctx->entries[ctx->entry_count - 1];
			ctx->entry_count--;
			return ;
		}
		i++;
	}
}

static bool	watch_add_one(t_WatchCtx *ctx, const char *abs_path)
{
	int	wd;

	wd = inotify_add_watch(ctx->fd, abs_path, WATCH_MASK);
	if (wd < 0)
		return (false);
	if (!entries_reserve(ctx, ctx->entry_count + 1))
		return (inotify_rm_watch(ctx->fd, wd), false);
	ctx->entries[ctx->entry_count].wd = wd;
	ctx->entries[ctx->entry_count].path = xstrdup(abs_path);
	if (NULL == ctx->entries[ctx->entry_count].path)
		return (inotify_rm_watch(ctx->fd, wd), false);
	ctx->entry_count++;
	return (true);
}

static bool	watch_add_recursive(t_WatchCtx *ctx, const char *abs_path)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*child_path;

	if (!is_dir_path(abs_path))
		return (true);
	if (!watch_add_one(ctx, abs_path))
		return (false);
	dir = opendir(abs_path);
	if (NULL == dir)
		return (false);
	while ((entry = readdir(dir)) != NULL)
	{
		if (0 == strcmp(entry->d_name, ".") || 0 == strcmp(entry->d_name, ".."))
			continue ;
		child_path = join_path(abs_path, entry->d_name);
		if (NULL == child_path)
			return (closedir(dir), false);
		if (is_dir_path(child_path) && !watch_add_recursive(ctx, child_path))
			return (free(child_path), closedir(dir), false);
		free(child_path);
	}
	closedir(dir);
	return (true);
}

static bool	pending_add(t_WatchCtx *ctx, uint32_t cookie, bool is_dir, const char *from_path)
{
	if (!pending_reserve(ctx, ctx->pending_count + 1))
		return (false);
	ctx->pending[ctx->pending_count].cookie = cookie;
	ctx->pending[ctx->pending_count].is_dir = is_dir;
	ctx->pending[ctx->pending_count].from_path = xstrdup(from_path);
	if (NULL == ctx->pending[ctx->pending_count].from_path)
		return (false);
	ctx->pending[ctx->pending_count].created_at = time(NULL);
	ctx->pending_count++;
	return (true);
}

static t_MovePending	*pending_take(t_WatchCtx *ctx, uint32_t cookie)
{
	size_t	i;
	t_MovePending	*res;

	i = 0;
	while (i < ctx->pending_count)
	{
		if (ctx->pending[i].cookie == cookie)
		{
			res = malloc(sizeof(t_MovePending));
			if (NULL == res)
				return (NULL);
			*res = ctx->pending[i];
			ctx->pending[i] = ctx->pending[ctx->pending_count - 1];
			ctx->pending_count--;
			return (res);
		}
		i++;
	}
	return (NULL);
}

static void	pending_flush_expired(t_WatchCtx *ctx, int ttl_seconds)
{
	size_t	i;
	time_t	now;

	now = time(NULL);
	i = 0;
	while (i < ctx->pending_count)
	{
		if (now - ctx->pending[i].created_at > ttl_seconds)
		{
			printf("MOVE OUTSIDE ROOT: %s\n", ctx->pending[i].from_path);
			free(ctx->pending[i].from_path);
			ctx->pending[i] = ctx->pending[ctx->pending_count - 1];
			ctx->pending_count--;
			continue ;
		}
		i++;
	}
}

static bool	handle_move_from(t_WatchCtx *ctx, const struct inotify_event *ev, const char *from_abs)
{
	return (pending_add(ctx, ev->cookie, (ev->mask & IN_ISDIR) != 0, from_abs));
}

static bool	handle_move_to(t_WatchCtx *ctx, const struct inotify_event *ev, const char *to_abs)
{
	t_MovePending	*pending;

	pending = pending_take(ctx, ev->cookie);
	if (NULL == pending)
	{
		printf("MOVE INTO ROOT: %s\n", to_abs);
		if (ev->mask & IN_ISDIR)
			return (watch_add_recursive(ctx, to_abs));
		return (true);
	}
	printf("MOVE INTERNAL: %s -> %s\n", pending->from_path, to_abs);
	if (pending->is_dir && !is_within_root(ctx, pending->from_path))
	{
		free(pending->from_path);
		free(pending);
		return (watch_add_recursive(ctx, to_abs));
	}
	free(pending->from_path);
	free(pending);
	return (true);
}

static bool	apply_events(t_WatchCtx *ctx)
{
	struct inotify_event	*ev;
	char					buffer[4096];
	ssize_t					len;
	char					*ptr;
	const char				*parent_abs;
	char					*entry_abs;

	len = read(ctx->fd, buffer, sizeof(buffer));
	if (len < 0)
		return (errno == EAGAIN || errno == EINTR);
	ptr = buffer;
	while (ptr < buffer + len)
	{
		ev = (struct inotify_event *)ptr;
		parent_abs = watch_get_path(ctx, ev->wd);
		if (ev->mask & IN_Q_OVERFLOW || NULL == parent_abs)
			return (false);
		entry_abs = xstrdup(parent_abs);
		if (ev->len > 0)
		{
			free(entry_abs);
			entry_abs = join_path(parent_abs, ev->name);
		}
		if (NULL == entry_abs)
			return (false);
		if ((ev->mask & IN_CREATE) && (ev->mask & IN_ISDIR))
		{
			printf("CREATE DIR: %s\n", entry_abs);
			if (!watch_add_recursive(ctx, entry_abs))
				return (free(entry_abs), false);
		}
		else if (ev->mask & IN_CREATE)
			printf("CREATE FILE: %s\n", entry_abs);
		if (ev->mask & IN_DELETE)
			printf("DELETE: %s\n", entry_abs);
		if (ev->mask & IN_MODIFY)
			printf("MODIFY: %s\n", entry_abs);
		if (ev->mask & IN_MOVED_FROM)
		{
			if (!handle_move_from(ctx, ev, entry_abs))
				return (free(entry_abs), false);
		}
		else if (ev->mask & IN_MOVED_TO)
		{
			if (!handle_move_to(ctx, ev, entry_abs))
				return (free(entry_abs), false);
		}
		if (ev->mask & IN_IGNORED)
			watch_remove_by_wd(ctx, ev->wd);
		if (ev->mask & (IN_UNMOUNT | IN_DELETE_SELF))
			return (free(entry_abs), false);
		free(entry_abs);
		ptr += sizeof(struct inotify_event) + ev->len;
	}
	pending_flush_expired(ctx, 1);
	return (true);
}

static void	watch_ctx_destroy(t_WatchCtx *ctx)
{
	size_t	i;

	i = 0;
	while (i < ctx->entry_count)
	{
		inotify_rm_watch(ctx->fd, ctx->entries[i].wd);
		free(ctx->entries[i].path);
		i++;
	}
	free(ctx->entries);
	i = 0;
	while (i < ctx->pending_count)
	{
		free(ctx->pending[i].from_path);
		i++;
	}
	free(ctx->pending);
	free(ctx->root_path);
	if (ctx->fd >= 0)
		close(ctx->fd);
}

int	main(int ac, char **av)
{
	t_WatchCtx	ctx;

	if (ac != 2)
		return (fprintf(stderr, "usage: %s /absolute/root/path\n", av[0]), 1);
	memset(&ctx, 0, sizeof(ctx));
	ctx.fd = -1;
	ctx.root_path = xstrdup(av[1]);
	if (NULL == ctx.root_path)
		return (1);
	ctx.fd = inotify_init1(0);
	if (ctx.fd < 0)
		return (perror("inotify_init1"), free(ctx.root_path), 1);
	if (!watch_add_recursive(&ctx, ctx.root_path))
		return (fprintf(stderr, "failed to add recursive watches\n"),
			watch_ctx_destroy(&ctx), 1);
	printf("watching recursively: %s\n", ctx.root_path);
	while (1)
	{
		if (!apply_events(&ctx))
		{
			printf("desync detected -> full resync required\n");
			break ;
		}
	}
	watch_ctx_destroy(&ctx);
	return (0);
}
