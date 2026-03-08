#include "../includes/solve.h"

static int pull_first_operand(num_expr *node)
{
	operand *aux = node->operands;
	int		sign_changed = 0;

	node->type = node->operands->expr->type;
	node->subtype = node->operands->expr->subtype;
	node->result = node->operands->expr->result;
	if (node->operands->expr->sign)
	{
		node->sign = (node->operands->expr->sign != node->sign);
		// sign_changed = 1;
	}
	node->operands = node->operands->expr->operands;
	free(aux->expr);
	free(aux);
	return (sign_changed);
}

static int bin_op_node_didactic(num_expr *expr, num_expr *node)
{
	int		changed = 0;

	#ifdef DEBUG
		printind();
		printf("Solving binary operation\n");
	#endif
	for (operand *op_1 = node->operands; op_1; op_1 = op_1->next)
	{
		solve_node_didactic(expr, op_1->expr);
		op_1 = apply_associative(expr, op_1);
		for (operand* op_2 = op_1->next; op_2; op_2 = op_2->next)
		{
			solve_node_didactic(expr, op_2->expr);
			op_2 = apply_associative(expr, op_2);

			if (apply_properties(expr, op_1, op_2))
			{
				print_num_expr(expr);
				solve_node_didactic(expr, node);
				return (0);
			}
			else if (op_1->expr->type == ALG_NUMBER && op_2->expr->type == ALG_NUMBER)
			{
				#ifdef DEBUG
					print_operation(op_1, op_2);
				#endif
				if (operate_numbers(expr, op_1, op_2))
				{
					op_2 = delete_operand(op_2);
					print_num_expr(expr);
					changed = 1;
				}
				#ifdef DEBUG
				else
					print_debug("Not operated\n");
				#endif
			}
			#ifdef DEBUG
			else
				print_debug("Not operated\n");
			#endif
		}
	}
	if (node->operands && !node->operands->next && node->type == ALG_BINARY_OP)
	{
		#ifdef DEBUG
			print_debug("Simplifying branch\n");
		#endif
		if (pull_first_operand(node))
			print_num_expr(expr);
	}
	else if ((node->subtype == OP_PROD || node->subtype == OP_DIV)
			&& node->operands && node->operands->expr->sign)
	{
		#ifdef DEBUG
			print_debug("Simplifying sign\n");
		#endif
		node->operands->expr->sign = 0;
		node->sign = !node->sign;
		print_num_expr(expr);
	}
	return (changed);
}

static void single_op_node_didactic(num_expr *expr, num_expr *node)
{
	solve_node_didactic(expr, node->operands->expr);
	if (node->operands->expr->type == ALG_NUMBER)
	{
		if (node->operands->operation == OP_ABS)
		{
			node->type = ALG_NUMBER;
			node->result = node->operands->expr->result;
			print_num_expr(expr);
		}
		else if (node->operands->operation == OP_SQRT)
		{
			if (node->operands->expr->result >= 0)
			{
				node->result = int_sqrt(node->operands->expr->result);
				if (node->result >= 0)
				{
					node->type = ALG_NUMBER;
					print_num_expr(expr);
				}
			}
		}
	}
}

void solve_node_didactic(num_expr *expr, num_expr *node)
{
	if (node->type == ALG_NUMBER)
		return;
	#ifdef DEBUG
		indent_spaces++;
		print_debug("Solving ");
		print_num_expr(node);
	#endif
	if (node->type == ALG_BINARY_OP)
		while (bin_op_node_didactic(expr, node))
			(void)expr;
	else if (node->type == ALG_SINGLE_OP)
		single_op_node_didactic(expr, node);
	// node->solved = 1;
	#ifdef DEBUG
		print_debug("Solved: ");
		print_num_expr(expr);
		indent_spaces--;
	#endif
}

void	solve_by_steps(num_expr *expr)
{
	solve_node_didactic(expr, expr);
}
