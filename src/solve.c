#include "../includes/solve.h"

static operand	*solve_operand_didactic(num_expr *expr, operand *op)
{
	if (!op->prev && !op->next)
		return (op);
	// #ifdef DEBUG
	// 	print_debug("Associative?\n");
	// #endif
	op = apply_associative(expr, op);
	// #ifdef DEBUG
	// 	print_debug("Identity?\n");
	// #endif
	op = simplify_identity(expr, op);
	return (op);
}

static int bin_op_node_didactic(num_expr *expr, num_expr *node)
{
	int		changed = 0;

	#ifdef DEBUG
		printind();
		printf("Solving binary operation\n");
	#endif
	operand *op1 = node->operands;
	while (op1)
	{
		solve_node_didactic(expr, op1->expr);
		op1 = solve_operand_didactic(expr, op1);
		operand* op2 = op1->next;
		while (op2)
		{
			solve_node_didactic(expr, op2->expr);
			op2 = solve_operand_didactic(expr, op2);
			if (apply_properties(expr, op1, op2))
			{
				print_num_expr(expr);
				solve_node_didactic(expr, node);
				return (0);
			}
			else if (op1->expr->type == ALG_NUMBER && op2->expr->type == ALG_NUMBER)
			{
				#ifdef DEBUG
					print_operation(op1, op2);
				#endif
				if (operate_numbers(expr, op1, op2))
				{
					op2 = delete_operand(op2);
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
			op2 = op2->next;
		}
		op1 = op1->next;
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
				else node->result = 0;
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
