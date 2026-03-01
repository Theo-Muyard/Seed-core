/**
 * @file memory.h
 * 
 * @brief Memory helper used across seed-core.
 * 	Provides allocation helpers.
 * 
 * @ingroup common
*/

#ifndef SEED_COMMON_MEMORY_H
# define SEED_COMMON_MEMORY_H

/**
 * @brief Duplicate a NULL-terminated string.
 *
 * @param str The string must not be NULL.
 *
 * @retval A newly allocated string owned by caller.
 * @retval NULL if `str` is NULL or allocation fails.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char	*ft_strdup(const char *str);

/**
 * @brief Split a NULL-terminated string into multiple strings
 * 	by cutting at the given character. The character is not
 * 	added to the strings.
 * 
 * @param str The string must not be NULL.
 * @param c The separator character.
 * 
 * @retval A newly allocated array of strings owned by caller.
 * @retval NULL if `str` is NULL or allocation fails.
 * 
 * @warning Caller must free returned pointer with `free()` as well
 * 	as all strings in the array.
 * 	Or use `ft_free_split()`.
*/
char	**ft_split(const char *str, char c);

/**
 * @brief Free a pointer given by the split function.
 * 
 * @param arr The array must not be NULL.
*/
void	ft_free_split(char **arr);

#endif