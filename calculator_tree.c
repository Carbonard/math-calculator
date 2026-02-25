#include "calculator.h"

alg_token_t *slice_tokens(alg_token_t *tokens, int start, int end)
{
	alg_token_t *slice = malloc((end - start + 2) * sizeof(alg_token_t));

	for (int i = 0; i < end - start; i++)
	{
		#ifdef DEBUG
			printf("coping token %d\n", tokens[start + i].type);
		#endif
		slice[i].type = tokens[start + i].type;
		slice[i].sub_type = tokens[start + i].sub_type;
		if (tokens[start + i].type == ALG_NUMBER)
			slice[i].number = tokens[start + i].number;
	}
	slice[end-start].type = ALG_END;
	return (slice);
}

int	skip_parenthesis(alg_token_t *tokens, int i)
{
	if (tokens[i].type == ALG_PARENTHESIS)
	{
		int parenthesis = 1;
		i++;
		while (parenthesis && tokens[i].type != ALG_END)
		{
			if (tokens[i].type == ALG_PARENTHESIS)
				parenthesis += 1 - 2 * tokens[i].sub_type;// '(' => 1-0, ')' => 1-2
			i++;
		}
	}
	return (i);
}

void	add_operand(num_expr *expr, int op_i, int op, alg_token_t *tokens, int start, int end)
{
	alg_token_t	*slice;

	expr->operands = realloc(expr->operands, (op_i+1) * sizeof(operand));
	expr->operands[op_i].operation = op;
	slice = slice_tokens(tokens, start, end);
	expr->operands[op_i].operand = generate_num_expr(slice);
	free(slice);
}

num_expr	*split_num_expr_bin_op(alg_token_t *tokens, int op1, int op2)
{
	num_expr	*expr = NULL;
	int			i = 0, op_i = 0, break_point = 0, op = OP_ERR;

	while (tokens[i].type != ALG_END)
	{
		#ifdef DEBUG
			printf("Reading token %d\n", tokens[i].type);
		#endif
		i = skip_parenthesis(tokens, i);
		if (tokens[i].type == ALG_BINARY_OP && (tokens[i].sub_type == op1 ||  tokens[i].sub_type == op2))
		{
			if (!expr)
			{
				expr = malloc(sizeof(num_expr));
				expr->type = ALG_BINARY_OP;
				expr->operands = NULL;
			}
			add_operand(expr, op_i, op, tokens, break_point, i);
			op = tokens[i].sub_type;
			break_point = i + 1;
			op_i++;
		}
		#ifdef DEBUG
			printf("Finishing reading token %d\n", tokens[i].type);
		#endif
		if (tokens[i].type != ALG_END)
			i++;
	}
	if (expr)
	{
		#ifdef DEBUG
			printf("Reading token %d\n", tokens[i].type);
		#endif
		add_operand(expr, op_i, op, tokens, break_point, i);
		#ifdef DEBUG
			printf("Finishing reading token %d\n", tokens[i].type);
		#endif
		op_i++;
		expr->operands = realloc(expr->operands, (op_i+1) * sizeof(operand));
		expr->operands[op_i].operand = NULL;
	}
	return (expr);
}

num_expr	*generate_num_expr_sing_op(alg_token_t *tokens)
{
	num_expr	*expr = NULL;
	alg_token_t	*slice = NULL;

	#ifdef DEBUG
		printf("Reading token %d\n", tokens[0].type);
	#endif
	if (tokens[0].type == ALG_SINGLE_OP)
	{
		if (!expr)
		{
			expr = malloc(sizeof(num_expr));
			expr->type = ALG_SINGLE_OP;
			expr->operands = malloc(sizeof(operand));
		}
		expr->operands->operation = tokens[0].sub_type;
		if (tokens[0+1].type == ALG_PARENTHESIS)
			slice = slice_tokens(tokens, 0+2, skip_parenthesis(tokens, 0+1) - 1);
		else if (tokens[0+1].type == ALG_NUMBER)
			slice = slice_tokens(tokens, 0+1, 0+2);
		else if (tokens[0+1].type == ALG_SINGLE_OP)
			expr->operands->operand = generate_num_expr(tokens + 0 + 1);
		else
			printf("please, report issue (undefined behaviour after single operator)\n");
		if (slice)
			expr->operands->operand = generate_num_expr(slice);
		free(slice);
	}
	return (expr);
}

num_expr	*generate_num_expr(alg_token_t *tokens)
{
	num_expr	*expr = NULL;

	#ifdef DEBUG
		printf("generating from:\n\t");
		print_tokens(tokens);
	#endif
	if (tokens[0].type == ALG_PARENTHESIS && tokens[skip_parenthesis(tokens, 0)].type == ALG_END)
	{
		alg_token_t	*slice = slice_tokens(tokens, 1, skip_parenthesis(tokens, 0) - 1);
		expr = generate_num_expr(slice);
		free(slice);
	}
	if (!expr)
		expr = split_num_expr_bin_op(tokens, OP_SUM, OP_SUBS);
	if (!expr)
		expr = split_num_expr_bin_op(tokens, OP_PROD, OP_DIV);
	if (!expr)
		expr = split_num_expr_bin_op(tokens, OP_POW, OP_POW);
	if (!expr)
		expr = generate_num_expr_sing_op(tokens);
	if (!expr)
	{
		expr = malloc(sizeof(num_expr));
		expr->type = ALG_NUMBER;
		expr->result = tokens->number;
		expr->resolved = 1;
		if (tokens[1].type != ALG_END)
			printf("Please, report issue (unexpected distribution of operands)\n");
	}
	else
	{
		expr->resolved = 0;
	}
	return (expr);
}

static void	print_numexpr(num_expr *expr)
{
	char bin_operands[] = {'+','-','*','/','^'};
	char sing_operands[][5] =
	{
		[OP_SQRT] = "sqrt",
		[OP_ROOT] = "root",
		[OP_LN] = "ln",
		[OP_LOG] = "log",
		[OP_EXP] = "exp",
		[OP_ABS] = "abs"
	};

	if (expr->type == ALG_NUMBER)
		printf("%d", expr->result);
	else if (expr->type == ALG_BINARY_OP)
	{
		if (expr->operands[0].operand->type == ALG_BINARY_OP)
			printf("(");
		print_numexpr(expr->operands[0].operand);
		if (expr->operands[0].operand->type == ALG_BINARY_OP)
			printf(")");
		for (int i = 1; expr->operands[i].operand; i++)
		{
			putchar(bin_operands[expr->operands[i].operation]);
			if (expr->operands[i].operand->type == ALG_BINARY_OP)
				printf("(");
			print_numexpr(expr->operands[i].operand);
			if (expr->operands[i].operand->type == ALG_BINARY_OP)
				printf(")");
		}
	}
	else if (expr->type == ALG_SINGLE_OP)
	{
		printf("%s(", sing_operands[expr->operands->operation]);
		print_numexpr(expr->operands->operand);
		printf(")");
	}
}

void	print_num_expr(num_expr *expr)
{
	print_numexpr(expr);
	printf("\n");
}

/*
int find_op(char *input)
{
	int	par_cnt;
	int	i;

	i = strlen(input);
	while (i)
	{
		i--;
		if ((input[i] == '+' || input[i] == '-') && (i != 0 && input[i-1] != '+' && input[i-1] != '-' && input[i-1] != '*' && input[i-1] != '/'))
			return (i);
		else if (input[i] == ')')
		{
			par_cnt = -1;
			while (par_cnt < 0)
			{
				i--;
				if (input[i] == '(')
					par_cnt++;
				else if (input[i] == ')')
					par_cnt--;
			}
		}
	}
	i = strlen(input);
	while (i)
	{
		i--;
		if (input[i] == '*' || input[i] == '/')
			return (i);
		else if (input[i] == ')')
		{
			par_cnt = -1;
			while (par_cnt < 0)
			{
				i--;
				if (input[i] == '(')
					par_cnt++;
				else if (input[i] == ')')
					par_cnt--;
			}
		}
	}
	return (0);
}

t_bin_operation *generate_tree(char *input)
{
	t_bin_operation	*node = malloc(sizeof(t_bin_operation));
	int				i = find_op(input);

	node->left = NULL;
	node->right = NULL;
	if (i)
	{
		node->op = input[i];
		input[i] = 0;
		node->left = generate_tree(input);
		node->right = generate_tree(input + i + 1);
		return (node);
	}
	else if (input[0] == '(')
	{
		node->op = '(';
		input[strlen(input) - 1] = 0;
		node->left = generate_tree(input + 1);
		return (node);
	}
	node->op = 0;
	node->result = itor(atoi(input));
	return (node);
}

void print_node(t_bin_operation *node)
{
	if (node->op == '(')
	{
		if (node->left->op && node->left->op != '(')
			putchar('(');
		print_node(node->left);
		if (node->left->op && node->left->op != '(')
			putchar(')');
	}
	else if (node->op == 0)
	{
		if (node->result[1] != 1)
			printf("{");
		print_rational(node->result);
		if (node->result[1] != 1)
			putchar('}');
	}
	else
	{
		print_node(node->left);
		// if (node->op == '/')
		// 	putchar(':');
		// else
			putchar(node->op);
		print_node(node->right);
	}
}

void print_tree(t_bin_operation *tree, char *comment)
{
	if (comment)
		printf("%s", comment);
	print_node(tree);
	putchar('\n');
}
*/