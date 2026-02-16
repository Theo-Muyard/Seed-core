#ifndef SEED_FILESYSTEM_WATCHER_INTERNAL_H
# define SEED_FILESYSTEM_WATCHER_INTERNAL_H

# include "filesystem/watcher/_watcher.h"

// +===----- Macros -----===+ //

# define EVENT_ALLOC		32
# define PENDING_ALLOC		32
# define ENTRY_ALLOC		32

# define WATCH_MASK	(IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM \
	| IN_MOVED_TO | IN_ATTRIB | IN_DELETE_SELF | IN_MOVE_SELF \
	| IN_Q_OVERFLOW | IN_UNMOUNT | IN_IGNORED)

// +===----- Functions -----===+ //

/**
 * @brief Check if the given path is a directory.
 * @param path The absolute path of the entry.
 * @return TRUE for directory, FALSE else.
*/
bool		is_dir(const char *path);

/**
 * @brief Get the path of the entry.
 * @param ctx The watcher context.
 * @param wd The wd of the entry.
*/
char		*watch_get_path(t_WatchCtx *ctx, int wd);

/**
 * @brief Get the wd of the entry.
 * @param ctx The watcher context.
 * @param path The path of the entry.
*/
int 		watch_get_wd(t_WatchCtx *ctx, const char *path);

/**
 * @brief Realloc the queue capacity if needed.
 * @param ctx The watcher context.
 * @param new_count The new count of pending entries needed.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		queue_reserve(t_WatchCtx *ctx, size_t new_count);

/**
 * @brief Realloc the pending capacity if needed.
 * @param ctx The watcher context.
 * @param new_count The new count of pending entries needed.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		pending_reserve(t_WatchCtx *ctx, size_t new_count);

/**
 * @brief Realloc the entries capacity if needed.
 * @param ctx The watcher context.
 * @param new_count The new count of watch entries needed.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		entries_reserve(t_WatchCtx *ctx, size_t new_count);

/**
 * @brief Add recursive watch entries.
 * @param ctx The watcher context.
 * @param path The absolute path of the entry.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		watch_add_recursive(t_WatchCtx *ctx, const char *path);

/**
 * @brief Remove recursive watch entries.
 * @param ctx The watcher context.
 * @param path The absolute path of the entry.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		watch_remove_recursive(t_WatchCtx *ctx, const char *path);

/**
 * @brief Handle an event.
 * @param ctx The watcher context.
 * @param event The inotify event.
 * @param path The path of the entry event.
 * @return An FsEvent of the handled event.
*/
t_FsEvent	*handle_event
(
	t_WatchCtx	*ctx,
	struct inotify_event *event,
	const char *path
);

#endif