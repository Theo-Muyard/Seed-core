#include "dependency.h"
#include "common/memory.h"

// +===----- Static functions -----===+ //

/**
 * @brief Counts the words in the string.
 * 
 * @param str The string must not be NULL.
 * @param c The characater.
 * 
 * @return The words count.
*/
static size_t	count_words(const char *str, char c)
{
	size_t	_i = 0;
	size_t	words = 0;
	while (str[_i])
	{
		if (str[_i] != c && (str[_i + 1] == c || str[_i + 1] == '\0'))
			words++;
		_i++;
	}
	return (words);
}

/**
 * @brief Copy the string until the end of the word.
 * 		Like `strcpy`.
 * 
 * @param dst The destination must not be NULL.
 * @param src The source must not be NULL.
 * @param c The character.
*/
static void	fill_tab(char *dst, const char *src, char c)
{
	size_t	_i = 0;
	while (src[_i] && src[_i] != c)
	{
		dst[_i] = src[_i];
		_i++;
	}
	dst[_i] = '\0';
}

/**
 * @brief Set the words in the tab.
 * 
 * @param tab The tab must not be NULL.
 * @param str The string must not be NULL.
 * @param c The character.
*/
static void	set_mem(char **tab, const char *str, char c)
{
	size_t	_index = 0;
	size_t	_i = 0;
	while (str[_index])
	{
		size_t	_count = 0;
		while (str[_index + _count] && str[_index + _count] != c)
			_count++;
		if (_count > 0)
		{
			tab[_i] = malloc(sizeof(char) * (_count + 1));
			if (NULL == tab[_i])
				return ;
			fill_tab(tab[_i], (str + _index), c);
			_i++;
			_index = _index + _count;
		}
		else
			_index++;
	}
	tab[_i] = 0;
}

// +===----- Functions -----===+ //

char	*ft_strdup(const char *str)
{
	RETURN_IF_NULL(str, NULL);

	char	*ptr = malloc((strlen(str) + 1) * sizeof(char));
	RETURN_IF_NULL(ptr, NULL);

	size_t	_i = 0;
	while (str[_i])
	{
		ptr[_i] = str[_i];
		_i++;
	}
	ptr[_i] = '\0';
	return (ptr);
}

char	**ft_split(const char *str, char c)
{
	size_t	_words = count_words(str, c);
	char	**tab = malloc((_words + 1) * sizeof(char *));
	RETURN_IF_NULL(tab, NULL);

	set_mem(tab, str, c);
	return (tab);
}

void	ft_free_split(char **arr)
{
	if (NULL == arr)
		return ;
	size_t	_i = 0;
	while (arr[_i])
	{
		free(arr[_i]);
		_i++;
	}
	free(arr);
}
