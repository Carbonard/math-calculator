#include "../includes/prints.h"

void dynamic_strcatf(dynamic_str *dest, const char *fstring, ...)
{
	va_list	args;
	va_list	args_copy;
	int		needed;

	va_start(args, fstring);
	va_copy(args_copy, args);

	needed = vsnprintf(NULL, 0, fstring, args_copy);
	va_end(args_copy);

	if (dest->length + needed + 1 > dest->size)
	{
		while (dest->length + needed + 1 > dest->size)
			dest->size *= 2;
		dest->str = realloc(dest->str, dest->size);
	}

	vsnprintf(dest->str + dest->length, dest->size - dest->length, fstring, args);
	dest->length += needed;

	va_end(args);
}