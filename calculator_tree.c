#include "calculator.h"

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

static void	add_operand(num_expr *expr, int op, alg_token_t *tokens, int start, int end)
{
	operand		*new_operand = malloc(sizeof(operand));
	operand		*aux;
	alg_token_t	*slice;

	slice = slice_tokens(tokens, start, end);
	if (slice)
		new_operand->operand = generate_num_expr(slice);
	else
		new_operand->operand = NULL;
	free(slice);
	new_operand->operation = op;
	new_operand->next = NULL;
	new_operand->prev = NULL;
	if (!(expr->operands))
	{
		expr->operands = new_operand;
		return;
	}
	aux = expr->operands;
	while (aux->next)
		aux = aux->next;
	aux->next = new_operand;
	new_operand->prev = aux;
	return;
	// expr->operands = realloc(expr->operands, (op_i+1) * sizeof(operand));
}

static num_expr	*split_num_expr_bin_op(alg_token_t *tokens, int op1, int op2)
{
	num_expr	*expr = NULL;
	int			i = 0, break_point = 0, op = OP_ERR;

	while (tokens[i].type != ALG_END)
	{
		i = skip_parenthesis(tokens, i);
		if (tokens[i].type == ALG_BINARY_OP && (tokens[i].sub_type == op1 ||  tokens[i].sub_type == op2))
		{
			if (!expr)
			{
				expr = malloc(sizeof(num_expr));
				expr->type = ALG_BINARY_OP;
				expr->operands = NULL;
			}
			add_operand(expr, op, tokens, break_point, i);
			op = tokens[i].sub_type;
			break_point = i + 1;
		}
		if (tokens[i].type != ALG_END)
			i++;
	}
	if (expr)
	{
		add_operand(expr, op, tokens, break_point, i);
		expr->subtype = op1;
		// add_operand(expr, OP_ERR, tokens, 0, -1);
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
		expr = malloc(sizeof(num_expr));
		expr->type = ALG_SINGLE_OP;
		expr->operands = malloc(sizeof(operand));
		expr->operands->operation = tokens[0].sub_type;
		slice = slice_sing_operand(tokens + 1);
		if (slice)
			expr->operands->operand = generate_num_expr(slice);
		else
			printf("please, report issue (undefined behaviour after single operator)\n");
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
	if (!expr && tokens[0].type == ALG_SIGN && tokens[1].type != ALG_NUMBER)
	{
		expr = generate_num_expr(tokens + 1);
		if (expr->sign)
			expr->sign = !tokens[0].sub_type;
		else
			expr->sign = tokens[0].sub_type;
		if (expr->type == ALG_NUMBER && expr->sign)
		{
			expr->result.sgn = !expr->result.sgn;
			expr->sign = 0;
		}
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
		expr = malloc(sizeof(num_expr));
		if (tokens[0].type == ALG_SIGN)
		{
			expr->result.sgn = tokens[0].sub_type;
			tokens++;
			// printf("Sign: %d, new token:%d\n", expr->sign, tokens->number);
		}
		else
			expr->result.sgn = 0;
		expr->sign = 0;
		expr->type = ALG_NUMBER;
		expr->result.integer = tokens->number;
		expr->result.type = N_INTEGER;
		// if (expr->sign == 1)
		// {
		// 	expr->sign = 0;
		// 	expr->result.integer *= -1;
		// }
		expr->resolved = 1;
		if (tokens[1].type != ALG_END)
			printf("Please, report issue (unexpected distribution of operands)\n");
	}
	else
	{
		expr->resolved = 0;
	}
	expr->sign = 0;
	return (expr);
}

void	print_number(number n)
{
	if (n.sgn)
		printf("-");
	if (n.type == N_INTEGER)
	{
		printf("%d", n.integer);
	}
	if (n.type == N_RATIONAL)
		printf("\\frac{%d}{%d}", n.rational[0], n.rational[1]);
	if (n.type == N_ROOT)
	{
		if (n.root[1] == 2)
			printf("\\sqrt{%d}", n.root[0]);
		else
			printf("\\sqrt[%d]{%d}", n.root[1], n.root[0]);
	}
	if (n.type == N_LOG)
		printf("\\log_%d{%d}", n.log[1], n.log[0]);
}

static int parenthesis_needed(num_expr *current, num_expr *next)
{
	// if (!current->operands->next)
	// 	return (0);
	if (next->type == ALG_BINARY_OP)
	{
		if (!next->operands->next)
			return (0);
		return (1);
		if (current->subtype == OP_PROD && next->subtype == OP_SUM)
			return (1);
		if (current->subtype == OP_POW)
			return (1);
	}
	else if (next->type == ALG_NUMBER && next->result.sgn)
	{
		return (1);
	}
	return (0);
}

static void	print_numexpr(num_expr *expr)
{
	char *bin_operands[] = {"+","-","\\cdot","/","^"};
	char sing_operands[][5] =
	{
		[OP_SQRT] = "sqrt",
		[OP_ROOT] = "root",
		[OP_LN] = "ln",
		[OP_LOG] = "log",
		[OP_EXP] = "exp",
		[OP_ABS] = "abs"
	};

	if (expr->sign)
	{
		printf("-");
	}
	if (expr->type == ALG_NUMBER)
		print_number(expr->result);
	else if (expr->type == ALG_BINARY_OP)
	{
		if (!expr->operands->next || expr->operands->next->operation != OP_DIV)
		{
			if (expr->sign)
				printf("(");
			if (parenthesis_needed(expr, expr->operands[0].operand) && expr->operands->operand->type != ALG_NUMBER)
				printf("(");
			print_numexpr(expr->operands[0].operand);
			if (parenthesis_needed(expr, expr->operands[0].operand) && expr->operands->operand->type != ALG_NUMBER)
				printf(")");
		}
		for (operand *op = expr->operands->next; op; op = op->next)
		{
			if (op->operation == OP_DIV)
			{
				printf("\\frac{");
				print_numexpr(op->prev->operand);
				printf("}{");
				print_numexpr(op->operand);
				printf("}");
			}
			else
			{
				printf("%s",bin_operands[op->operation]);
				if (!(op->next && op->next->operation == OP_DIV))
				{
					if (parenthesis_needed(expr, op->operand))
						printf("(");
					print_numexpr(op->operand);
					if (parenthesis_needed(expr, op->operand))
						printf(")");
				}
			}
		}
		if (expr->sign)
			printf(")");
	}
	else if (expr->type == ALG_SINGLE_OP)
	{
		printf("\\%s{", sing_operands[expr->operands->operation]);
		print_numexpr(expr->operands->operand);
		printf("}");
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