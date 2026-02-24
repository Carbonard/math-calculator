#include "calculator.h"
#include <readline/readline.h>
#include <readline/history.h>

char *read_input()
{
	char *input;

	input = readline("-> ");
	return (input);
}

int main()
{
	char	*input;

	do
	{
		input = read_input();
		if (input && !strcmp(input, "exit"))
			return (0);
		add_history(input);
		manage_input(input);
		free(input);
	} while (input);
	return (1);
}
