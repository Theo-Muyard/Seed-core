#include "dependency.h"
#include "tools/memory.h"

char	*ft_strdup(const char *str)
{
	int		_i;
	char	*ptr;

	TEST_NULL(str, NULL);
	ptr = malloc((strlen(str) + 1) * sizeof(char));
	TEST_NULL(ptr, NULL);
	_i = 0;
	while (str[_i])
	{
		ptr[_i] = str[_i];
		_i++;
	}
	ptr[_i] = '\0';
	return (ptr);
}

static size_t	count_words(const char *s, char c)
{
	size_t	words;
	size_t	_i;

	words = 0;
	_i = 0;
	while (s[_i])
	{
		if (s[_i] != c && (s[_i + 1] == c || s[_i + 1] == '\0'))
			words++;
		_i++;
	}
	return (words);
}

static void	fill_tab(char *new, const char *str, char c)
{
	size_t	_i;

	_i = 0;
	while (str[_i] && str[_i] != c)
	{
		new[_i] = str[_i];
		_i++;
	}
	new[_i] = '\0';
}

static void	set_mem(char **tab, const char *str, char c)
{
	size_t	_count;
	size_t	_index;
	size_t	_i;

	_index = 0;
	_i = 0;
	while (str[_index])
	{
		_count = 0;
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

char	**ft_split(const char *str, char c)
{
	size_t	_words;
	char	**tab;

	_words = count_words(str, c);
	tab = malloc((_words + 1) * sizeof(char *));
	TEST_NULL(tab, NULL);
	set_mem(tab, str, c);
	return (tab);
}

void	ft_free_split(char **arr)
{
	size_t	_i;

	if (NULL == arr)
		return ;
	_i = 0;
	while (arr[_i])
	{
		free(arr[_i]);
		_i++;
	}
	free(arr);
}