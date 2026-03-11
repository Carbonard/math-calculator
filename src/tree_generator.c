#include "../includes/tree.h"

static alg_token_t *slice_tokens(alg_token_t *tokens, int start, int end)
{
	alg_token_t *slice = NULL;
	
	if (end < start)
		return (NULL);
	slice = malloc((end - start + 2) * sizeof(alg_token_t));
	for (int i = 0; i < end - start; i++)
	{
		slice[i].type = tokens[start + i].type;
		slice[i].sub_type = tokens[start + i].sub_type;
		if (tokens[start + i].type == ALG_NUMBER)
			slice[i].number = tokens[start + i].number;
	}
	slice[end-start].type = ALG_END;
	return (slice);
}

static alg_token_t *dup_tokens(alg_token_t *tokens)
{
	int	i = 0;

	while (tokens[i].type != ALG_END)
		i++;
	return (slice_tokens(tokens, 0, i));
}

static int	skip_parenthesis(alg_token_t *tokens, int i)
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

static operand	*create_operand_from_tokens(int op, alg_token_t *tokens, int start, int end)
{
	operand		*new_operand = malloc(sizeof(operand));
	alg_token_t	*slice;

	slice = slice_tokens(tokens, start, end);
	if (slice)
		new_operand->expr = generate_num_expr(slice);
	else
		new_operand->expr = NULL;
	free(slice);
	new_operand->operation = op;
	new_operand->next = NULL;
	new_operand->prev = NULL;
	return (new_operand);
}

static num_expr	*split_num_expr_bin_op(alg_token_t *tokens, int op1, int op2)
{
	num_expr	*expr = NULL;
	int			i = 0, break_point = 0, op = 0;

	while (tokens[i].type != ALG_END)
	{
		i = skip_parenthesis(tokens, i);
		if (tokens[i].type == ALG_BINARY_OP && (tokens[i].sub_type == op1 || tokens[i].sub_type == op2))
		{
			if (!expr)
				expr = create_num_expr(ALG_BINARY_OP, op1);
			if (op == OP_DIV)
				operand_to_bin_op(last_operand(expr), OP_DIV, create_operand_from_tokens(op, tokens, break_point, i));
			else
				append_operand(expr, create_operand_from_tokens(op, tokens, break_point, i));
			op = tokens[i].sub_type;
			break_point = i + 1;
		}
		if (tokens[i].type != ALG_END)
			i++;
	}
	if (expr)
	{
		expr->operands->operation = op1;
		if (op == OP_DIV)
			operand_to_bin_op(last_operand(expr), OP_DIV, create_operand_from_tokens(op, tokens, break_point, i));
		else
			append_operand(expr, create_operand_from_tokens(op, tokens, break_point, i));
		if (!expr->operands->next && op == OP_DIV)
		{
			pull_first_operand(expr);
		}
	}
	return (expr);
}

static alg_token_t	*slice_sing_operand(alg_token_t *tokens)
{
	alg_token_t	*slice = NULL;
	int			sgn;

	if (tokens[0].type == ALG_SIGN)
		sgn = 1;
	else
		sgn = 0;
	if (tokens[sgn].type == ALG_PARENTHESIS)
		slice = slice_tokens(tokens, 1 - sgn, skip_parenthesis(tokens + sgn, 0) - 1 + sgn);
	else if (tokens[sgn].type == ALG_NUMBER)
		slice = slice_tokens(tokens, 0, sgn+1);
	else if (tokens[sgn].type == ALG_SINGLE_OP)
		slice = dup_tokens(tokens);
	return (slice);
}

static num_expr	*generate_num_expr_sing_op(alg_token_t *tokens)
{
	num_expr	*expr = NULL;
	alg_token_t	*slice = NULL;

	if (tokens[0].type == ALG_SINGLE_OP)
	{
		expr = create_num_expr(ALG_SINGLE_OP, tokens[0].sub_type);
		slice = slice_sing_operand(tokens + 1);
		expr->operands = create_operand(tokens[0].sub_type, generate_num_expr(slice));
		free(slice);
	}
	return (expr);
}

num_expr	*generate_num_expr(alg_token_t *tokens)
{
	num_expr	*expr = NULL;

	#ifdef DEBUG
		printf("generating from:\n");
		print_tokens(tokens);
	#endif
	if (tokens[0].type == ALG_END)
		return (NULL);
	if (!expr)
		expr = split_num_expr_bin_op(tokens, OP_SUM, OP_SUBS);
	if (!expr)
		expr = split_num_expr_bin_op(tokens, OP_PROD, OP_DIV);
	if (!expr)
		expr = split_num_expr_bin_op(tokens, OP_POW, OP_POW);
	if (!expr && tokens[0].type == ALG_SIGN)
	{
		expr = generate_num_expr(tokens + 1);
		if (expr->sign)
			expr->sign = !tokens[0].sub_type;
		else
			expr->sign = tokens[0].sub_type;
		return (expr);
	}
	if (!expr && tokens[0].type == ALG_PARENTHESIS && tokens[skip_parenthesis(tokens, 0)].type == ALG_END)
	{
		alg_token_t	*slice = slice_tokens(tokens, 1, skip_parenthesis(tokens, 0) - 1);
		expr = generate_num_expr(slice);
		free(slice);
		return (expr);
	}
	if (!expr)
		expr = generate_num_expr_sing_op(tokens);
	if (!expr)
	{
		expr = create_num_expr(ALG_NUMBER, 0);
		expr->result = tokens->number;
		// expr->solved = 1;
		if (tokens[1].type != ALG_END)
			printf("Please, report issue (unexpected distribution of operands)\n");
	}
	// else
	// 	expr->solved = 0;
	return (expr);
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