#include "calculator.h"

// typedef number(*op_function)(number a, number b);

#ifdef DEBUG
	static int indent = 0;
	void printind()
	{
		for (int i = 0; i < indent; i++)
			printf("\t");
	}
#endif

static void	sum_int(num_expr *a, num_expr *b)
{
	if (a->sign == b->sign)
	{
		a->result = a->result + b->result;
		a->sign = a->sign;
	}
	else if (a->result >= b->result)
	{
		a->result = a->result - b->result;
		a->sign = a->sign;
	}
	else
	{
		a->result = b->result - a->result;
		a->sign = b->sign;
	}
	// print_num_expr(expr);
}

static void	prod_int(num_expr *expr, num_expr *a, num_expr *b)
{
	if (a->sign || b->sign)
	{
		a->sign = (a->sign != b->sign);
		print_num_expr(expr);
	}
	a->result = a->result * b->result;
	// print_num_expr(expr);
}

static int	simplify_num_div(num_expr *a, num_expr *b)
{
	integer div = gcd(a->result, b->result);

	if (div < 2)
		return (0);
	a->result /= div;
	b->result /= div;
	return (1);
}

static int	div_int(num_expr *expr, num_expr *a, num_expr *b)
{
	if (a->result % b->result)
	{
		if (simplify_num_div(a, b))
			print_num_expr(expr);
		return (0);
	}
	a->sign = (a->sign != b->sign);
	print_num_expr(expr);
	a->result = a->result / b->result;
	return (1);
	// print_num_expr(expr);
}

static void solve_node_didactic(num_expr *expr, num_expr *node);

int manage_signs(operand *op_1, operand *op_2)
{
	int		changed = 0;
	char	*sgn1, *sgn2;

	sgn1 = &op_1->operand->sign;
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

operand* apply_propierties(operand *op1, operand *op2)
// this joins op1 and op2 in op1 and return previous op2 (or NULL if anything happened)
{
	operand	*ret_value = NULL;

	// sqrt(a)*sqrt(b) = sqrt(a*b)
	if (op2->operation == OP_PROD
		&& op1->operand->type == ALG_SINGLE_OP && op1->operand->subtype == OP_SQRT
		&& op2->operand->type == ALG_SINGLE_OP && op2->operand->subtype == OP_SQRT)
	{
		#ifdef DEBUG
			printind();
			printf("Applying sqrt(a)*sqrt(b) = sqrt(a*b)\n");
		#endif
		num_expr *new_expr = create_num_expr(ALG_BINARY_OP, OP_PROD);
		append_operand(new_expr, create_operand(0, op1->operand->operands->operand));
		append_operand(new_expr, create_operand(OP_PROD, op2->operand->operands->operand));
		op1->operand->operands->operand = new_expr;
		op2->operand->operands->operand = NULL;
		ret_value = del_and_back(op2);
	}

	// (a/b)*(c/d) = (ac)/(bd) // (a/b)*c = (ac)/b
	else if (op2->operation == OP_PROD
		&& ((op1->operand->type == ALG_BINARY_OP && op1->operand->subtype == OP_DIV)
		|| (op2->operand->type == ALG_BINARY_OP && op2->operand->subtype == OP_DIV)))
	{
		#ifdef DEBUG
			printind();
			printf("Applying (a/b)*(c/d) = (ac)/(bd) // (a/b)*c = (ac)/b\n");
		#endif
		if (op1->operand->subtype != OP_DIV)
			// Transform c into c/1
			operand_to_bin_op(op1, OP_DIV, create_operand(OP_DIV, create_num_num_expr(0,1)));
		if (op2->operand->subtype != OP_DIV)
			// Transform c into c/1
			operand_to_bin_op(op2, OP_DIV, create_operand(OP_DIV, create_num_num_expr(0,1)));
		operand_to_bin_op(op1->operand->operands->next, OP_PROD, op2->operand->operands->next);
		operand_to_bin_op(op1->operand->operands, OP_PROD, op2->operand->operands);
		op2->operand->operands = NULL;
		ret_value = del_and_back(op2);
	}

	// (a/b)+(c/d) // (a/b)+c
	else if (op2->operation == OP_PROD
		&& ((op1->operand->type == ALG_BINARY_OP && op1->operand->subtype == OP_DIV)
		|| (op2->operand->type == ALG_BINARY_OP && op2->operand->subtype == OP_DIV)))
	{
		#ifdef DEBUG
			printind();
			printf("Applying (a/b)+(c/d) // (a/b)+c\n");
		#endif
		if (op1->operand->subtype != OP_DIV)
			// Transform c into c/1
			operand_to_bin_op(op1, OP_DIV, create_operand(OP_DIV, create_num_num_expr(0,1)));
		if (op2->operand->subtype != OP_DIV)
			// Transform c into c/1
			operand_to_bin_op(op2, OP_DIV, create_operand(OP_DIV, create_num_num_expr(0,1)));
		
		operand_to_bin_op(op1->operand->operands->next, OP_PROD, op2->operand->operands->next);
		operand_to_bin_op(op1->operand->operands, OP_PROD, op2->operand->operands);
		op2->operand->operands = NULL;
		ret_value = del_and_back(op2);
	}

	return (ret_value);
}

static operand	*apply_associative(num_expr *expr, operand *op)
{
	if (op->operand->type == ALG_BINARY_OP
		&& op->operation == op->operand->subtype
		&& (op->operation == OP_SUM || op->operation == OP_PROD))
	{
		#ifdef DEBUG
			printind();
			printf("Applying associative\n");
		#endif
		expand_operand(&op, op->operand->operands);
		print_num_expr(expr);
	}
	return (op);
}

static int	operate_numbers(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation == OP_SUM)
		sum_int(op1->operand, op2->operand);
	else if (op2->operation == OP_SUBS)
	{
		if (op2->operand->sign)
		{
			op2->operand->sign = 0;
			op2->operation = OP_SUM;
			print_num_expr(expr);
		}
		else
			op2->operand->sign = 1;
		sum_int(op1->operand, op2->operand);
	}
	else if (op2->operation == OP_PROD)
		prod_int(expr, op1->operand, op2->operand);
	else if (op2->operation == OP_DIV)
		return (div_int(expr, op1->operand, op2->operand));
	else
		return (0);
	return (1);
}

static int bin_op_node_didactic(num_expr *expr, num_expr *node)
{
	int		changed = 0;
	operand	*aux;

	#ifdef DEBUG
		printind();
		printf("Solving binary operation\n");
	#endif
	for (operand *op_1 = node->operands; op_1; op_1 = op_1->next)
	{
		solve_node_didactic(expr, op_1->operand);
		op_1 = apply_associative(expr, op_1);
		for (operand* op_2 = op_1->next; op_2; op_2 = op_2->next)
		{
			solve_node_didactic(expr, op_2->operand);
			op_2 = apply_associative(expr, op_2);
			#ifdef DEBUG
				printind();
				printf("Managing signs\n");
			#endif
			if (manage_signs(op_1, op_2))
				print_num_expr(expr);
			#ifdef DEBUG
				printind();
				printf("Managing propieries\n");
			#endif
			if ((aux = apply_propierties(op_1, op_2)) != NULL)
			{
				op_2 = aux;
				print_num_expr(expr);
				// op_1->operand->solved = 0;
				solve_node_didactic(expr, node);
				return (0);
			}
			// this else asumes that apply_propierties joins op1 and op2 in op1
			else if (op_1->operand->type == ALG_NUMBER && op_2->operand->type == ALG_NUMBER)
			{
				#ifdef DEBUG
					printind();
					printf("Managing bin op with numbers: ");
					print_num_expr(op_1->operand);
					char *bin_operands[] = {"","+","-","\\cdot","/","^"};
					printind();
					printf("%s", bin_operands[op_2->operation]);
					print_num_expr(op_2->operand);
				#endif
				if (operate_numbers(expr, op_1, op_2))
				{
					op_2 = del_and_back(op_2);
					print_num_expr(expr);
					changed = 1;
				}
				#ifdef DEBUG
				else
				{
					printind();
					printf("Not operated\n");
				}
				#endif
			}
			#ifdef DEBUG
			else
			{
				printind();
				printf("Not operated\n");
			}
			#endif
		}
	}
	if (node->operands && !node->operands->next && node->type == ALG_BINARY_OP)
	{
		if (node->operands->operand->type == ALG_NUMBER)
		{
			#ifdef DEBUG
				printind();
				printf("Simplifying numeric branch\n");
			#endif
			node->type = ALG_NUMBER;
			node->result = node->operands->operand->result;
			if (node->operands->operand->sign)
			{
				node->sign = !node->sign;
				if (!node->sign)
						print_num_expr(expr);
			}
			free_operand(node->operands);
			node->operands = NULL;
		}
		else
		{
			#ifdef DEBUG
				printind();
				printf("Simplifying non-numeric branch\n");
			#endif
			operand *aux;
			aux = node->operands->operand->operands;
			node->type = node->operands->operand->type;
			node->result = node->operands->operand->result;
			if (node->operands->operand->sign)
			{
				node->sign = !node->sign;
				node->operands->operand->sign = 0;
				print_num_expr(expr);
			}
			node->subtype = node->operands->operand->subtype;
			free(node->operands->operand);
			free(node->operands);
			node->operands = aux;
		}
	}
	else if (node->subtype == OP_PROD || node->subtype == OP_DIV)
	{
		if (node->operands && node->operands->operand->sign)
		{
			#ifdef DEBUG
				printind();
				printf("Simplifying sign\n");
			#endif
			node->operands->operand->sign = 0;
			node->sign = !node->sign;
			print_num_expr(expr);
		}
	}
	return (changed);
}

static void sing_op_node_didactic(num_expr *expr, num_expr *node)
{
	solve_node_didactic(expr, node->operands->operand);
	if (node->operands->operand->type == ALG_NUMBER)
	{
		if (node->operands->operation == OP_ABS)
		{
			node->type = ALG_NUMBER;
			node->result = node->operands->operand->result;
			print_num_expr(expr);
		}
		else if (node->operands->operation == OP_SQRT)
		{
			if (node->operands->operand->result >= 0)
			{
				node->result = int_sqrt(node->operands->operand->result);
				if (node->result >= 0)
				{
					node->type = ALG_NUMBER;
					print_num_expr(expr);
				}
			}
		}
	}
}

static void solve_node_didactic(num_expr *expr, num_expr *node)
{
	if (node->type == ALG_NUMBER)
		return;
	#ifdef DEBUG
		indent++;
		printind();
		printf("Solving ");
		print_num_expr(node);
	#endif
	if (node->type == ALG_BINARY_OP)
		while (bin_op_node_didactic(expr, node))
			(void)expr;
	else if (node->type == ALG_SINGLE_OP)
		sing_op_node_didactic(expr, node);
	// node->solved = 1;
	#ifdef DEBUG
		printind();
		printf("Solved: ");
		print_num_expr(expr);
		indent--;
	#endif
}

void	solve_by_steps(num_expr *expr)
{
	solve_node_didactic(expr, expr);
	// printf("The result is ");
	// print_rational(tree->result);
	// putchar('\n');
}
