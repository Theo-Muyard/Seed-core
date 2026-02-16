#ifndef SEED_FILESYSTEM_WATCHER_H
# define SEED_FILESYSTEM_WATCHER_H

# include "dependency.h"

// +===----- Types -----===+ //

typedef enum	e_FsEventsType
{
	FS_EVENT_CREATE,
	FS_EVENT_DELETE,
	FS_EVENT_MOVE,
	FS_EVENT_OVERFLOW
}	t_FsEventsType;

typedef struct	s_FsEvent
{
	t_FsEventsType	type;
	char			*path;
	char			*new_path;
	bool			isdir;
}	t_FsEvent;

typedef struct	s_WatchEntry
{
	int		wd;
	char	*path;
}	t_WatchEntry;

typedef struct	s_MovePending
{
	uint32_t	cookie;
	bool		is_dir;
	char		*from_path;
	time_t		created_at;
}	t_MovePending;

typedef struct	s_WatchCtx
{
	int				fd;
	char			*path;

	t_FsEvent		**event_queue;
	size_t			event_count;
	size_t			event_capacity;

	t_WatchEntry	**entries;
	size_t			entry_count;
	size_t			entry_capacity;

	t_MovePending	**pending;
	size_t			pending_count;
	size_t			pending_capacity;
}	t_WatchCtx;

// +===----- Functions -----===+ //

/**
 * @brief Initialize the watcher context.
 * @param path The absolute path of the root folder to watch.
 * @return The watcher context that was been created.
*/
t_WatchCtx	*watcher_init(const char *path);

/**
 * @brief Destroy the watcher context.
 * @param ctx The watcher context that will be destroy.
*/
void		watcher_destroy(t_WatchCtx *ctx);

/**
 * @brief Analyze events of the OS filesystem.
 * @param ctx The watcher context.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		watcher_analyze(t_WatchCtx *ctx);

#endif