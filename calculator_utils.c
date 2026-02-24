#include "calculator.h"

void remove_spaces(char *str)
{
	int	shift = 0;
	int	i = 0;

	while (str[i])
	{
		if (isspace(str[i]))
			shift++;
		else
			str[i - shift] = str[i];
		i++;
	}
	str[i - shift] = str[i];
}