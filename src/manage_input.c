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

char *manage_input(char *input)
{
	alg_token_t		*tokens;
	num_expr		*expr;
	json_obj		*json;
	char			json_str[1000];

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
		if ((expr = generate_num_expr(tokens)) != NULL)
			print_num_expr(expr);
		// printf("------------------------------------------\n");
		// printf("\\hline\n");
		solve_by_steps(expr);
		free_num_expr(expr);
		free(tokens);
		// free_num_expr(expr);
	}
	sprint_json(json, json_str);
	delete_json(json);
	#ifdef DEBUG
		fprintf(stderr, "\njson:\n%s\n\n", json_str);
	#else
		(void)tokens;
	#endif
	return (strdup(json_str));
}