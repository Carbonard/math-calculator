#include "../includes/calculator.h"
#include <readline/readline.h>
#include <readline/history.h>

char *read_input()
{
	return (readline("-> "));
}

int main()
{
	char	*input;

	while (1)
	{
		input = read_input();
		if (!input)
			return (1);
		if (!strcmp(input, "exit"))
		{
			rl_clear_history();
			return (0);
		}
		add_history(input);
		free(manage_input(input));
		free(input);
	}
	return (2);
}
