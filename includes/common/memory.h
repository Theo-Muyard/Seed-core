#ifndef SEED_TOOLS_MEMORY_H
# define SEED_TOOLS_MEMORY_H

// +===----- Functions -----===+ //

/**
 * @brief Duplicate and allocate the string.
 * @param str The string.
 * @return The allocated string.
*/
char	*ft_strdup(const char *str);

/**
 * @brief Split the string.
 * @param str The string.
 * @param c The caractere where the string will be splited.
 * @return An allocated array of strings.
*/
char	**ft_split(const char *str, char c);

/**
 * @brief Free an array allocated by the split function.
 * @param arr The array.
*/
void	ft_free_split(char **arr);

#endif