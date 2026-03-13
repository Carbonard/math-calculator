#include "../includes/solve.h"

void print_tokens(alg_token_t *tokens)
{
	char chars[][7][5] =
	{
		[ALG_SIGN] = {"", "-"},
		[ALG_BINARY_OP] = {"+", "-", "*", "/"},
		[ALG_PARENTHESIS] = {"(", ")"},
		[ALG_SINGLE_OP] = {"sqrt", "root", "ln", "log", "exp", "abs"}
	};
	fprintf(stderr, "\n\e[93mTokens:\t");
	for (int i = 0; tokens[i].type != ALG_END; i++)
	{
		if (tokens[i].type == ALG_NUMBER)
			fprintf(stderr, "%d", tokens[i].number);
		else
			fprintf(stderr, "%s", chars[tokens[i].type][tokens[i].sub_type]);
	}
	fprintf(stderr, "\n\e[0m\n");
}

void free_steps(step *steps)
{
	step *i_step = steps;

	while (i_step->step_code >= 0)
	{
		free(i_step->latex);
		i_step++;
	}
	free(steps);
}

char *manage_input(char *input)
{
	alg_token_t		*tokens;
	num_expr		*expr;
	json_obj		*json;
	char			*json_str = NULL;
	step			*steps = NULL;

	if (!input || !input[0])
		return (NULL);
	tokens = check_syntax(input, &json);
	#ifdef DEBUG
		printf("\nTokens read\n");
	#endif
	if (!strcmp(json->data[0].str_value, "ok"))
	{
		#ifdef DEBUG
			printf("\nstatus ok\n");
		#endif
		#ifdef DEBUG
			print_tokens(tokens);
		#endif
		expr = generate_num_expr(tokens);
		if (expr)
		{
			print_num_expr(expr);
			steps = solve_by_steps(expr);
			delete_json(json);
			// free(json);
			json = steps_json(steps);
			free(steps); // no libera todo porque lo usa json
			free_num_expr(expr);
			free(tokens);
		}
	}
	json_str = json_to_str(json);
	delete_json(json);
	#ifdef DEBUG
		fprintf(stderr, "\njson:\n%s\n\n", json_str);
	#else
		(void)tokens;
	#endif
	// printf(json_str);
	return (json_str);
}
