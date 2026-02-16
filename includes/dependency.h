#ifndef SEED_DEPENDENCY_H
# define SEED_DEPENDENCY_H

// +===----- External libraries -----===+ //

# include <sys/inotify.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
# include <dirent.h>
# include <time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <string.h>

// +===----- Macros -----===+ //

/**
 * @brief Test if the variable is NULL.
 * @param var The variable that will be tested.
 * @param err The value returned.
*/
# define TEST_NULL(var, err) \
	if (NULL == var) return (err)

/**
 * @brief Test if the function return false.
 * @param fn The function that will be tested.
 * @param err The value returned.
*/
# define TEST_ERROR_FN(fn, err) \
	if (false == fn) return (err)

#endif