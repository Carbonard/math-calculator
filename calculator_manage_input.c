#include "calculator.h"

void print_tokens(alg_token_t *tokens)
{
	char chars[5][7][5] =
	{
		[ALG_BINARY_OP] = {"+", "-", "*", "/"},
		[ALG_PARENTHESIS] = {"(", ")"},
		[ALG_SINGLE_OP] = {"sqrt", "root", "ln", "log", "exp", "abs"}
	};
	fprintf(stderr, "\nTokens:\n");
	for (int i = 0; tokens[i].type != ALG_END; i++)
	{
		if (tokens[i].type == ALG_NUMBER)
			fprintf(stderr, "%d", tokens[i].number);
		else
			fprintf(stderr, "%s", chars[tokens[i].type][tokens[i].sub_type]);
	}
	fprintf(stderr, "\n\n");
}

char *manage_input(char *input)
{
	// t_bin_operation	*op_tree;
	alg_token_t		*tokens;
	num_expr		*expr;
	// int				syntax_status;
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
		// remove_spaces(input);
		// op_tree = generate_tree(input);
		expr = generate_num_expr(tokens);
		// print_tree(op_tree, NULL);
		print_num_expr(expr);
		// solve_tree_by_steps(op_tree);
	}
	sprint_json(json, json_str);
	#ifdef DEBUG
		fprintf(stderr, "\njson:\n%s\n\n", json_str);
	#else
		(void)tokens;
	#endif
	return (strdup(json_str));
}