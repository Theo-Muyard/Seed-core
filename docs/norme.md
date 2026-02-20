# C norme - By Sσɾҽɳ

> For readable and secure code.

## Table of contents

- [Quick start](#quick-start)
- [Indentation](#indentation)
- [Breaking long lines](#breaking-long-lines)
- [Braces / spaces](#braces-spaces)
- [Naming](#naming)
- [Typedefs](#typedefs)
- [Functions](#functions)
- [Goto usage](#goto-usage)
- [Commenting](#commenting)
- [Macros](#macros)
- [Headers](#headers)
- [Memory management](#memory-management)

---

## Quick start

This is a short document presenting a coding style that aims to be simple, readable and secure.
Everyone has their own way of writing code. The goal is not to impose a divine style, but to provide food for thought
Well-written code should be as simple as possible, readable and, above all, crash free.

Happy reading, and may the code be with you !

---

## Indentation

Indentation must ALWAYS be done with tabs.
I think 8 characters for 1 tab is perfect, but 4 is acceptable.

If you cannot see the beginning and end of a block, the indentation needs to be increased.

Exemples:
```C
// Don't do it! ❌
if (status)
{
  my_function();
  other_function();
  printf("Status is > 0");
}
else
  printf("Status is < 0")

// Do it! ✔️
if (status)
{
	my_function();
	other_function();
	printf("Status is > 0");
}
else
	printf("Status is < 0")
```

---

## Breaking long lines

The limit on the length of lines is 80 columns.
Lines longer than 80 characters should be broken into several lines.

Exemples:
```C
// Don't do it! ❌
int	my_function(bool first_param, const char *second_param, size_t third_param)

// Do it! ✔️
int	my_function(
	bool first_param,
	const char *second_param,
	size_t third_param
)
```

```C
// Don't do it! ❌
if ((first_condition || second_condition) && (third_condition || fourth_condition))

// Do it! ✔️
if ((first_condition || second_condition)
	&& (third_condition || fourth_condition))
```

---

## Braces / spaces

Braces must be placed alone on an empty line.

```C
// Don't do it! ❌
if (condition) {
	// ...
}

// Do it! ✔️
if (condition)
{
	// ...
}
```

Use a space after these keywords :
```C
if, switch, case, for, do, while
```

Before and after these :
```C
= + - < > * / % | ^ <= >= == != ? :
```


## Naming

The name of anything, whether if be functions, variables or anything else, should be as explicit as possible.

```C
// Don't do it! ❌
char	*n;

// Do it! ✔️
char	*name;
```

Internal variables and functions must be prefixed with a `_`, if the return variable of a function is a local variable, don't prefix it!

```C
// Don't do it! ❌
static int	my_function(void)
{
	char	*buffer;
	int	len;
	// ...
	return (len);
}

// Do it! ✔️
static int	_my_function(void)
{
	char	*_buffer;
	int	len;
	// ...
	return (len);
}
```

Use the `snake_case` everytime, `PascalCase` for typedefs and `UPPER_CASE` for macros. 

## Typedefs

Prefix the type with a `t_`, structures with `s_` and enum with `e_`

```C
// Don't do it! ❌
typedef struct	buffer
{
	// ...
}	buffer;

// Do it! ✔️
typedef struct	s_Buffer
{
	// ...
}	t_Buffer;
```

## Functions

A function should be short and concise: one function, one action.
A function can be as short as 10 lines or as long as 50, but it shoult always do one thing at a time.

## Goto usage

The use of goto should be EXCLUSIVELY for handling common error cases.

```C
char	*my_function(...)
{
	// ...
	if (error)
		goto err_free_buffer
	// ...
	err_free_buffer:
		free(...);
		return (...);
}
```

## Commenting

Use the doxygen commentary method.
Comments should not be written in a function. The code written in the function must be readable and understandable by anyone, comments are only used to provide guidance on how to use the function.
No trivial comments.
Comments are allowed for non-obvious constraints or design decisions.
However, all these specificities must be specified in the documentation for a function.
```C
// Don't do it! ❌
/*
 * Description of the function
 * ...
*/
void	my_function(void);

// Do it! ✔️
/**
 * @brief The description of the function and these specificities
 * @param Param_1 A param of my function
 * @return The return value of my function
*/
void	my_function(void);
```

## Macros

Macros should not be too long, a macro can call a function if necessary.
For magic numbers, macros are definitely preferable.
Macros can also be used to optimize certain recurring elements in your code, but be careful not to overuse them.

## Headers

Headers are the best solution for clean code.
Headers contains the prototypes of functions, these functions must be documented in his header, NO comments in `.c`
Macros, types, functions, all declarations will be placed in the `.h`

Key takeaway: headers are used to structure code, while `.c ` files are only used to establish what has been defined.

## Memory management

As often as possible, allocations should be made using functions, one that allows allocation and another that frees memory.
When funtions return something that has been allocated, this must be specified in the documentation so that it's never forgotten to free it.