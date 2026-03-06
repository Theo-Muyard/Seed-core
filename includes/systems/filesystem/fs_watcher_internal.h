/**
 * @file "systems/filesystem/fs_watcher_internal.h"
 * 
 * @brief Managing the entire internal structure of the watcher.
 * 
 * @ingroup "filesystem"
*/

#ifndef SEED_FILESYSTEM_WATCHER_INTERNAL_H
# define SEED_FILESYSTEM_WATCHER_INTERNAL_H

# include "dependency.h"

// +===----- Types -----===+ //

/* An filesystem event type */
typedef enum	e_FsEventsType
{
	FS_EVENT_CREATE = 0,	/* Create file/folder event */
	FS_EVENT_DELETE,		/* Delete file/folder event */
	FS_EVENT_MOVE,		/* Move file/folder event */
	FS_EVENT_OVERFLOW		/* Queue/event overflow event */
}	t_FsEventsType;

/* An filesystem event */
typedef struct	s_FsEvent
{
	t_FsEventsType	type;		/* Event type */
	char			*path;	/* File/folder path */
	char			*new_path;	/* Optional new path */
	bool			isdir;	/* Is folder ? */
}	t_FsEvent; 

/* A watching entry */
typedef struct	s_WatchEntry
{
	int		wd;	/* Watch descriptor */
	char	*path;	/* Folder path */
}	t_WatchEntry;

/* A move pending file/folder */
typedef struct	s_MovePending
{
	uint32_t	cookie;	/* The pending cookie */
	bool		is_dir;	/* Is folder ? */
	char		*from_path;	/* The file/folder old path */
}	t_MovePending;

/* The watch context in filesystem */
typedef struct	s_WatchCtx
{
	int			fd;			/* Root file descriptor */
	char			*path;		/* Root path */

	t_FsEvent		**event_queue;	/* Events queue */
	size_t		event_count;	/* Count */
	size_t		event_capacity;	/* Capacity */

	t_WatchEntry	**entries;		/* Watch entries */
	size_t		entry_count;	/* Count */
	size_t		entry_capacity;	/* Capacity */

	t_MovePending	**pending;		/* Pending queue */
	size_t		pending_count;	/* Count */
	size_t		pending_capacity;	/* Capacity */
}	t_WatchCtx;

// +===----- Macros -----===+ //

# define EVENT_ALLOC		32
# define PENDING_ALLOC		32
# define ENTRY_ALLOC		32

# define WATCH_MASK	(IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM \
	| IN_MOVED_TO | IN_ATTRIB | IN_DELETE_SELF | IN_MOVE_SELF \
	| IN_Q_OVERFLOW | IN_UNMOUNT | IN_IGNORED)

// +===----- Functions -----===+ //

/**
 * @brief Retrieve the path using its watch descriptor.
 * 
 * @param ctx The context must not be NULL.
 * @param wd The file/folder wd.
 * 
 * @retval The path.
 * @retval NULL if `ctx` is NULL, if then wd does'nt exist or an error occurred.
*/
char		*watch_get_path(t_WatchCtx *ctx, int wd);

/**
 * @brief Retrieve the watch descriptor using its path.
 * 
 * @param ctx The context must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval The wd.
 * @retval `-1` if `ctx` or `path` is NULL, the entry does'nt exist or
 * 		an error occurred.
*/
int 		watch_get_wd(t_WatchCtx *ctx, const char *path);

/**
 * @brief Realloc the queue capacity if needed.
 * 
 * @param ctx The context must not be NULL.
 * @param new_count The new count of pending entries.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` is NULL or an error occurred.
*/
bool		queue_reserve(t_WatchCtx *ctx, size_t new_count);

/**
 * @brief Realloc the pending capacity if needed.
 * 
 * @param ctx The context must not be NULL.
 * @param new_count The new count of pending entries.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` is NULL or an error occurred.
*/
bool		pending_reserve(t_WatchCtx *ctx, size_t new_count);

/**
 * @brief Realloc the entries capacity if needed.
 * 
 * @param ctx The context must not be NULL.
 * @param new_count The new count of pending entries.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` is NULL or an error occurred.
*/
bool		entries_reserve(t_WatchCtx *ctx, size_t new_count);

/**
 * @brief Adds watch entries recursively.
 * 
 * @param ctx The context must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` or `path` is NULL or an error occurred.
*/
bool		watch_add_recursive(t_WatchCtx *ctx, const char *path);

/**
 * @brief Removes watch entries recursively.
 * 
 * @param ctx The context must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` or `path` is NULL or an error occurred.
*/
bool		watch_remove_recursive(t_WatchCtx *ctx, const char *path);

/**
 * @brief Flushs the remaining move pending.
 * 
 * @param ctx The context must not be NULL. 
 * 
 * @retval TRUE for success.
 * @retval FALSE if `ctx` is NULL or an error occurred.
*/
bool		flush_pending(t_WatchCtx *ctx);

/**
 * @brief Handle an event.
 * 
 * @param ctx The context must not be NULL.
 * @param event The event must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval A newly allocated event.
 * @retval NULL if `ctx`, `event` or `path` is NULL or an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
t_FsEvent	*handle_event
(
	t_WatchCtx	*ctx,
	struct inotify_event *event, const char *path
);

#endif