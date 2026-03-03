#include "calculator.h"

typedef number(*op_function)(number a, number b);

number	sum_int(number a, number b)
{
	number n;

	n.type = N_INTEGER;
	if (a.sgn == b.sgn)
	{
		n.integer = a.integer + b.integer;
		n.sgn = a.sgn;
	}
	else if (a.integer >= b.integer)
	{
		n.integer = a.integer - b.integer;
		n.sgn = a.sgn;
	}
	else
	{
		n.integer = b.integer - a.integer;
		n.sgn = b.sgn;
	}
	return (n);
}

number	subs_int(number a, number b)
{
	b.sgn = !b.sgn;
	return (sum_int(a, b));
}

number	prod_int(number a, number b)
{
	number n;

	n.type = N_INTEGER;
	n.integer = a.integer * b.integer;
	n.sgn = (a.sgn != b.sgn);
	return (n);
}

number	div_int(number a, number b)
{
	number n;

	n.type = N_RATIONAL;
	n.rational[0] = a.integer;
	n.rational[1] = b.integer;
	n.sgn = (a.sgn != b.sgn);
	simplify(n.rational);
	if (n.rational[1] == 1)
	{
		n.type = N_INTEGER;
		n.integer = n.rational[0];
	}
	return (n);
}

number	sum_rat(number a, number b)
{
	number n;
	int		mult;

	if (a.type == N_INTEGER)
	{
		a.rational[0] = a.integer;
		a.rational[1] = 1;
	}
	if (b.type == N_INTEGER)
	{
		b.rational[0] = b.integer;
		b.rational[1] = 1;
	}
	mult = lcm(a.rational[1], b.rational[1]);
	a.rational[0] *= mult / a.rational[1];
	b.rational[0] *= mult / b.rational[1];
	a.rational[1] = mult;
	b.rational[1] = mult;
	// print_num_expr(expr);
	if (a.sgn == b.sgn)
	{
		n.rational[0] = a.rational[0] + b.rational[0];
		n.sgn = a.sgn;
	}
	else if (a.integer >= b.integer)
	{
		n.rational[0] = a.rational[0] - b.rational[0];
		n.sgn = a.sgn;
	}
	else
	{
		n.rational[0] = b.rational[0] - a.rational[0];
		n.sgn = b.sgn;
	}
	n.rational[1] = mult;
	n.type = N_RATIONAL;
	simplify(n.rational);
	if (n.rational[1] == 1)
	{
		n.type = N_INTEGER;
		n.integer = n.rational[0];
	}
	return (n);
}

number	subs_rat(number a, number b)
{
	b.sgn = !b.sgn;
	return (sum_rat(a,b));
}

number	prod_rat(number a, number b)
{
	number n;

	if (a.type == N_INTEGER)
	{
		a.rational[0] = a.integer;
		a.rational[1] = 1;
	}
	if (b.type == N_INTEGER)
	{
		b.rational[0] = b.integer;
		b.rational[1] = 1;
	}
	n.rational[0] = a.rational[0] * b.rational[0];
	n.rational[1] = a.rational[1] * b.rational[1];
	n.sgn = (a.sgn != b.sgn);
	n.type = N_RATIONAL;
	simplify(n.rational);
	if (n.rational[1] == 1)
	{
		n.type = N_INTEGER;
		n.integer = n.rational[0];
	}
	return (n);
}

number	div_rat(number a, number b)
{
	number n;

	if (a.type == N_INTEGER)
	{
		a.rational[0] = a.integer;
		a.rational[1] = 1;
	}
	if (b.type == N_INTEGER)
	{
		b.rational[0] = b.integer;
		b.rational[1] = 1;
	}
	n.rational[0] = a.rational[0] * b.rational[1];
	n.rational[1] = a.rational[1] * b.rational[0];
	n.sgn = (a.sgn != b.sgn);
	n.type = N_RATIONAL;
	simplify(n.rational);
	if (n.rational[1] == 1)
	{
		n.type = N_INTEGER;
		n.integer = n.rational[0];
	}
	return (n);
}

number	prod_root(number a, number b)
{
	number	n;
	integer root;

	if (a.root[1] == b.root[1])
	{
		n.root[0] = a.root[0] * b.root[0];
		n.root[1] = a.root[1];
		n.sgn = (a.sgn != b.sgn);
		n.type = N_ROOT;
		if (n.root[1] == 2)
		{
			root = int_sqrt(n.root[0]);
			if (root >= 0)
			{
				n.type = N_INTEGER;
				n.integer = root;
			}
		}
	}
	return (n);
}

static void solve_node_didactic(num_expr *expr, num_expr *node);

// static int manage_signs_num(operand *op_1, operand *op_2)
// {
// 	int	changed = 0;
// 	// Manage signs in sum and subs
// 	if (op_2->operation == OP_SUBS && op_2->operand->result.sgn == 1)
// 	{
// 		op_2->operation = OP_SUM;
// 		op_2->operand->result.sgn = 0;
// 		changed = 1;
// 	}
// 	else if ((op_2->operation == OP_SUM && op_2->operand->result.sgn == 1))
// 	{
// 		op_2->operation = OP_SUBS;
// 		op_2->operand->result.sgn = 0;
// 		changed = 1;
// 	}
// 	// Manage signs in prod and div
// 	if ((op_2->operation == OP_PROD || op_2->operation == OP_DIV) && (op_1->operand->result.sgn != op_2->operand->result.sgn))
// 	{
// 		op_1->operand->result.sgn = 1;
// 		op_2->operand->result.sgn = 0;
// 		changed = 1;
// 	}
// 	else if ((op_2->operation == OP_PROD || op_2->operation == OP_DIV) && (op_1->operand->result.sgn == op_2->operand->result.sgn))
// 	{
// 		op_1->operand->result.sgn = 0;
// 		op_2->operand->result.sgn = 0;
// 		changed = 1;
// 	}
// 	return (changed);
// }

static int manage_signs(operand *op_1, operand *op_2)
{
	int		changed = 0;
	char	*sgn1, *sgn2;

	if (op_1->operand->type == ALG_NUMBER)
		sgn1 = &op_1->operand->result.sgn;
	else
		sgn1 = &op_1->operand->sign;
	if (op_2->operand->type == ALG_NUMBER)
		sgn2 = &op_2->operand->result.sgn;
	else
		sgn2 = &op_2->operand->sign;
	// Manage signs in sum and subs
	if (op_2->operation == OP_SUBS && *sgn2 == 1)
	{
		op_2->operation = OP_SUM;
		*sgn2 = 0;
		changed = 1;
	}
	else if ((op_2->operation == OP_SUM && *sgn2 == 1))
	{
		op_2->operation = OP_SUBS;
		*sgn2 = 0;
		changed = 1;
	}
	// Manage signs in prod and div
	if ((op_2->operation == OP_PROD || op_2->operation == OP_DIV) && (*sgn1 != *sgn2))
	{
		*sgn1 = 1;
		*sgn2 = 0;
		changed = 1;
	}
	else if ((op_2->operation == OP_PROD || op_2->operation == OP_DIV) && (*sgn1 == *sgn2))
	{
		if (*sgn1)
			changed = 1;
		*sgn1 = 0;
		*sgn2 = 0;
	}
	return (changed);
}

int app_prop(operand *op1, operand *op2)
{
	if (op1->operand->type != op2->operand->type)
		return (0);
	if (op1->operand->type != ALG_SINGLE_OP)
		return (0);
	if (op2->operation == OP_PROD && op1->operand->operands->operation == OP_SQRT && op2->operand->operands->operation == OP_SQRT)
	{
		num_expr *new_expr = malloc(sizeof(num_expr));
		new_expr->operands = malloc(sizeof(operand));
		new_expr->type = ALG_BINARY_OP;
		new_expr->subtype = op2->operation;
		new_expr->resolved = 0;
		new_expr->sign = 0;
		new_expr->operands->operation = OP_PROD;
		new_expr->operands->operand = op1->operand->operands->operand;
		new_expr->operands->prev = NULL;
		new_expr->operands->next = malloc(sizeof(operand));
		new_expr->operands->next->operation = OP_PROD;
		new_expr->operands->next->operand = op2->operand->operands->operand;
		new_expr->operands->next->next = NULL;
		new_expr->operands->next->prev = new_expr->operands;
		op1->operand->operands->operand = new_expr;
		op2->prev->next = op2->next;
		if (op2->next)
			op2->next->prev = op2->prev;
		free(op2->operand);
		free(op2);
		op1->operand->resolved = 0;
		// printf("...\n");
		return (1);
	}
	return (0);
}

static void bin_op_node_didactic(num_expr *expr, num_expr *node)
{
	operand *aux, *aux2;
	op_function functions[5][5][5] = {
		[OP_SUM] = {
					// N_INTEGER,	N_RATIONAL,	N_ROOT,	N_LOG
		[N_INTEGER]	= {sum_int,		sum_rat,	NULL,	NULL},
		[N_RATIONAL]= {sum_rat,		sum_rat, 	NULL,	NULL},
		[N_ROOT]	= {NULL,		NULL,		NULL,	NULL},
		[N_LOG]		= {NULL,		NULL,		NULL,	NULL},
		},
		[OP_SUBS] = {
					// N_INTEGER,	N_RATIONAL,	N_ROOT,	N_LOG
		[N_INTEGER]	= {subs_int,	subs_rat,	NULL,	NULL},
		[N_RATIONAL]= {subs_rat,	subs_rat, 	NULL,	NULL},
		[N_ROOT]	= {NULL,		NULL,		NULL,	NULL},
		[N_LOG]		= {NULL,		NULL,		NULL,	NULL},
		},
		[OP_PROD] = {
					// N_INTEGER,	N_RATIONAL,	N_ROOT,		N_LOG
		[N_INTEGER]	= {prod_int,	prod_rat,	NULL,		NULL},
		[N_RATIONAL]= {prod_rat,	prod_rat, 	NULL,		NULL},
		[N_ROOT]	= {NULL,		NULL,		prod_root,	NULL},
		[N_LOG]		= {NULL,		NULL,		NULL,		NULL},
		},
		[OP_DIV] = {
					// N_INTEGER,	N_RATIONAL,	N_ROOT,		N_LOG
		[N_INTEGER]	= {div_int,		div_rat,	NULL,	NULL},
		[N_RATIONAL]= {div_rat,		div_rat, 	NULL,	NULL},
		[N_ROOT]	= {NULL,		NULL,		NULL,	NULL},
		[N_LOG]		= {NULL,		NULL,		NULL,	NULL},
		}
	};
	op_function op_func;

	for (operand *op_1 = node->operands; op_1; op_1 = op_1->next)
	{
		solve_node_didactic(expr, op_1->operand);
		for (operand* op_2 = op_1->next; op_2; op_2 = op_2->next)
		{
			solve_node_didactic(expr, op_2->operand);
			#ifdef DEBUG
				printf("Managing signs\n");
			#endif
			if (manage_signs(op_1, op_2))
				print_num_expr(expr);
			if (app_prop(op_1, op_2))
			{
				print_num_expr(expr);
				solve_by_steps(expr);
				return;
			}
			if (op_1->operand->type == ALG_NUMBER && op_2->operand->type == ALG_NUMBER)
			{
				#ifdef DEBUG
					printf("Managing bin op with numbers\n");
				#endif
				op_func = functions[op_2->operation][op_1->operand->result.type][op_2->operand->result.type];
				if (op_func)
				{
					op_1->operand->result = op_func(op_1->operand->result, op_2->operand->result);
					op_2->prev->next = op_2->next;
					if (op_2->next)
						op_2->next->prev = op_2->prev;
					aux = op_2;
					op_2 = op_2->prev;
					free(aux->operand);
					free(aux);
					print_num_expr(expr);
				}
			}
			else if (op_2->operand->type == node->type && op_2->operand->subtype == node->subtype)
			{
				#ifdef DEBUG
					printf("Managing bin op with same subnode type\n");
				#endif
				aux = op_2;
				op_2 = op_2->prev;
				aux2 = aux->next;
				op_2->next = aux->operand->operands;
				free(aux->operand);
				op_2->next->operation = aux->operation;
				aux = op_2->next;
				aux->prev = op_2;
				while (aux->next)
					aux = aux->next;
				aux->next = aux2;
				aux2->prev = aux;
			}
		}
	}
	if (!node->operands[0].next)
	{
		if (node->operands->operand->type == ALG_NUMBER)
		{
			#ifdef DEBUG
				printf("Simplifying numeric branch");
			#endif
			node->type = ALG_NUMBER;
			node->result.sgn = (node->result.sgn != node->sign);
			node->sign = 0;
			node->result = node->operands->operand->result;
			free(node->operands->operand);
			free(node->operands);
			node->operands = NULL;
		}
		else
		{
			#ifdef DEBUG
				printf("Simplifying non-numeric branch");
			#endif
			aux = node->operands->operand->operands;
			free(node->operands->operand);
			free(node->operands);
			node->operands = aux;
		}
	}
	else if (node->subtype == OP_PROD)
	{
		if (node->operands->operand->sign)
		{
			node->operands->operand->sign = 0;
			node->sign = !node->sign;
			print_num_expr(expr);
		}
	}
}

static void sing_op_node_didactic(num_expr *expr, num_expr *node)
{
	solve_node_didactic(expr, node->operands->operand);
	if (node->operands->operand->type == ALG_NUMBER && node->operands->operand->result.type == N_INTEGER)
	{
		if (node->operands->operation == OP_SQRT)
		{
			node->result.integer = int_sqrt(node->operands->operand->result.integer);
			if (node->result.integer >= 0)
			{
				node->type = ALG_NUMBER;
				node->result.type = N_INTEGER;
				node->result.sgn = node->sign;
				node->sign = 0;
				print_num_expr(expr);
			}
		}
		// if (node->operands->operation == OP_SQRT)
		// {
		// 	node->type = ALG_NUMBER;
		// 	node->result.integer = int_sqrt(node->operands->operand->result.integer);
		// 	if (node->result.integer < 0)
		// 	{
		// 		node->result.type = N_ROOT;
		// 		node->result.root[0] = node->operands->operand->result.integer;
		// 		node->result.root[1] = 2;
		// 	}
		// 	else
		// 		node->result.type = N_INTEGER;
		// 	node->result.sgn = node->sign;
		// 	node->sign = 0;
		// }
	}
}

static void solve_node_didactic(num_expr *expr, num_expr *node)
{
	if (node->resolved)
		return;
	#ifdef DEBUG
		printf("Solving ");
		print_num_expr(node);
	#endif
	if (node->type == ALG_BINARY_OP)
		bin_op_node_didactic(expr, node);
	else if (node->type == ALG_SINGLE_OP)
		sing_op_node_didactic(expr, node);
	node->resolved = 1;
	#ifdef DEBUG
		printf("Solved\n");
	#endif
}

void	solve_by_steps(num_expr *expr)
{
	solve_node_didactic(expr, expr);
	// printf("The result is ");
	// print_rational(tree->result);
	// putchar('\n');
}

/*
void interactive_sum(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	integer	mult;

	if (left[1] == 1 && right[1] == 1)
	{
		node->result = Q_sum(left, right);
		node->op = 0;
		print_tree(tree, NULL);
		return;
	}
	mult = lcm(left[1], right[1]);
	left[0] *= mult / left[1];
	right[0] *= mult / right[1];
	left[1] = mult;
	right[1] = mult;
	print_tree(tree, NULL);
	node->result = fraction(left[0] + right[0], mult);
	node->op = 0;
	print_tree(tree, NULL);
	simplify(node->result);
	print_tree(tree, NULL);
}

void interactive_subs(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	integer	mult;

	if (left[1] == 1 && right[1] == 1)
	{
		node->result = Q_subs(left, right);
		node->op = 0;
		print_tree(tree, NULL);
		return;
	}
	mult = lcm(left[1], right[1]);
	left[0] *= mult / left[1];
	right[0] *= mult / right[1];
	left[1] = mult;
	right[1] = mult;
	print_tree(tree, NULL);
	node->result = fraction(left[0] - right[0], mult);
	node->op = 0;
	print_tree(tree, NULL);
	simplify(node->result);
	print_tree(tree, NULL);
}

void interactive_prod(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	if (left[1] == 1 || right[1] == 1)
	{
		node->op = 0;
		node->result = Q_prod(left, right);
		print_tree(tree, NULL);
		return;
	}
	node->op = 0;
	node->result = fraction(left[0]*right[0],left[1]*right[1]);
	print_tree(tree, NULL);
	simplify(node->result);
	print_tree(tree, NULL);
}

void interactive_div(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	integer	tmp;

	if (right[1] == 1)
	{
		node->op = 0;
		node->result = Q_div(left, right);
		if (left[0] != node->result[0])
			print_tree(tree, NULL);
		return;
	}
	node->op = '*';
	tmp = right[0];
	right[0] = right[1];
	right[1] = tmp;
	print_tree(tree, NULL);
	interactive_prod(tree, node, left, right);
}

rational solve_node_interactive(t_bin_operation *tree, t_bin_operation *node)
{
	rational left, right;

	if (node->left)
		left = solve_node_interactive(tree, node->left);
	else
		left  = NULL;
	if (node->right)
		right = solve_node_interactive(tree, node->right);
	else
		right = NULL;
	if (node->op == '(')
	{
		node->result = left;
		node->op = 0;
	}
	else if (node->op == '+')
		interactive_sum(tree, node, left, right);
	else if (node->op == '-')
		interactive_subs(tree, node, left, right);
	else if (node->op == '*')
		interactive_prod(tree, node, left, right);
	else if (node->op == '/')
		interactive_div(tree, node, left, right);
	return (node->result);
}

void	solve_tree_by_steps(t_bin_operation *tree)
{
	solve_node_interactive(tree, tree);
	printf("The result is ");
	print_rational(tree->result);
	putchar('\n');
}
*/