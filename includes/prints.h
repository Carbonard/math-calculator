#ifndef PRINTS_H
#define PRINTS_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct
{
	char	*str;
	size_t	length;
	size_t	size;
} dynamic_str;

void dynamic_strcatf(dynamic_str *dest, const char *fstring, ...);

#endif