/**
 * @file "dependency.h"
 * 
 * @brief Includes all global dependencies, such as
 * 		libraries or macros.
*/

#ifndef SEED_DEPENDENCY_H
# define SEED_DEPENDENCY_H

// +===----- External libraries -----===+ //

# include <sys/inotify.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
# include <dirent.h>
# include <time.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <string.h>

// +===----- Macros -----===+ //

/**
 * @brief Return the given value if ptr is NULL.
 * 
 * @param ptr The pointer.
 * @param retval The return value.
*/
# define RETURN_IF_NULL(ptr, retval) \
	do { if (NULL == ptr) return (retval); } while (0)

/**
 * @brief Return the given value if var is FALSE.
 * 
 * @param var The variable.
 * @param retval The return value.
*/
# define RETURN_IF_FALSE(var, retval) \
	do { if (false == var) return (retval); } while (0)

/**
 * @brief Execute the specified goto if ptr is NULL.
 * 
 * @param ptr The pointer.
 * @param label The goto label.
*/
# define GOTO_IF_NULL(ptr, label) \
	do { if (NULL == ptr) goto label; } while (0)

/**
 * @brief Execute the specified goto if var is FALSE.
 * 
 * @param var The variable.
 * @param label The goto label.
*/
# define GOTO_IF_FALSE(var, label) \
	do { if (false == var) goto label; } while (0)

#endif