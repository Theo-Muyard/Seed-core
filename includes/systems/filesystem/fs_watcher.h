/**
 * @file "systems/filesystem/fs_watcher.h"
 * 
 * @brief Monitors the OS filesystem and creates detailed events
 * 		based on functions.
 * 
 * @ingroup "filesystem"
*/

#ifndef SEED_FILESYSTEM_WATCHER_H
# define SEED_FILESYSTEM_WATCHER_H

# include "systems/filesystem/fs_watcher_internal.h"

// +===----- Functions -----===+ //

/**
 * @brief Initialize the watcher context.
 * 
 * @param path The path must not be NULL.
 * 
 * @retval A newly allocated watch context owned by caller.
 * @retval NULL if `path` is NULL or an error occurred.
 * 
 * @warning Caller must free returned pointer with `watcher_destroy()`.
*/
t_WatchCtx	*watcher_init(const char *path);

/**
 * @brief Destroy the watcher context.
 * 
 * @param ctx The context must not be NULL.
*/
void		watcher_destroy(t_WatchCtx *ctx);

/**
 * @brief Analyze events of the OS filesystem.
 * 
 * @param ctx The watcher context.
 * 
 * @retval TRUE for succcess.
 * @retval FALSE if `ctx` is NULL or an error occurred.
*/
bool		watcher_analyze(t_WatchCtx *ctx);

#endif