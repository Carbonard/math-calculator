#include "../includes/solve.h"

static operand	*solve_operand(solving_data *data, operand *op)
{
	if (!op->prev && !op->next)
		return (op);
	// #ifdef DEBUG
	// 	print_debug("Associative?\n");
	// #endif
	op = apply_associative(data, op);
	// #ifdef DEBUG
	// 	print_debug("Identity?\n");
	// #endif
	op = simplify_identity(data, op);
	return (op);
}

static int bin_op_node(solving_data *data, num_expr *node)
{
	int			changed = 0;

	#ifdef DEBUG
		printind();
		printf("Solving binary operation\n");
	#endif
	operand *op1 = node->operands;
	while (op1)
	{
		if (op1->next && apply_properties_1(data, node, op1, op1->next))
			return (0);
		solve_node(data, op1->expr);
		op1 = solve_operand(data, op1);
		operand* op2 = op1->next;
		while (op2)
		{
			if (apply_properties_1(data, node, op1, op2))
				return (0);
			solve_node(data, op2->expr);
			op2 = solve_operand(data, op2);
			if (apply_properties_2(data, node, op1, op2))
				return (0);
			else if (op1->expr->type == ALG_NUMBER && op2->expr->type == ALG_NUMBER)
			{
				#ifdef DEBUG
					print_operation(op1, op2);
				#endif
				if (operate_numbers(data, node, op1, op2))
					return (0);
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
			record_step(data, STEP_SIGN);
		#ifdef DEBUG
			printind();
			printf("Now the type is %s and the subtyoe %s\n", algtypes[node->type], binop[node->subtype]);
		#endif
	}
	else if ((node->subtype == OP_PROD)
			&& node->operands && node->operands->expr->sign)
	{
		#ifdef DEBUG
			print_debug("Simplifying sign\n");
		#endif
		node->operands->expr->sign = 0;
		node->sign = !node->sign;
		record_step(data, STEP_SIGN);
	}
	else if ((node->subtype == OP_DIV)
			&& node->operands && node->sign)
	{
		#ifdef DEBUG
			print_debug("Simplifying sign\n");
		#endif
		node->sign = 0;
		node->operands->expr->sign = !node->sign;
		record_step(data, STEP_SIGN);
	}
	return (changed);
}

static void single_op_node(solving_data *data, num_expr *node)
{
	solve_node(data, node->operands->expr);
	if (node->operands->expr->type == ALG_NUMBER)
	{
		if (node->operands->operation == OP_ABS)
		{
			node->type = ALG_NUMBER;
			node->result = node->operands->expr->result;
			record_step(data, STEP_ABS);
		}
		else if (node->operands->operation == OP_SQRT)
		{
			if (node->operands->expr->result >= 0)
			{
				node->result = int_sqrt(node->operands->expr->result);
				if (node->result >= 0)
				{
					node->type = ALG_NUMBER;
					record_step(data, STEP_SQRT);
				}
				else node->result = 0;
			}
		}
	}
}

void rearrange_operands(solving_data *data, num_expr *node)
{
	operand *op = node->operands;
	data->record = 0;

	#ifdef DEBUG
		print_debug("sorting operands\n");
	#endif
	if (node->type == ALG_BINARY_OP && node->subtype == OP_DIV)
		return ;
	while (op && op->next)
	{
		if (expr_cmp(op->expr, op->next->expr) > 0)
		{
			swap_operands(op, op->next);
			data->record = 1;
			op = node->operands;
		}
		else
			op = op->next;
	}
	if (data->record)
		record_step(data, STEP_SORT_OP);
}

void solve_node(solving_data *data, num_expr *node)
{
	if (node->type == ALG_NUMBER)
		return;
	#ifdef DEBUG
		indent_spaces++;
		print_debug("Solving ");
		print_num_expr(node);
	#endif
	while (node->type == ALG_BINARY_OP && bin_op_node(data, node))
		(void)data;
	if (node->type == ALG_SINGLE_OP)
		single_op_node(data, node);
	rearrange_operands(data, node);
	simplify_frac(data, node);
	#ifdef DEBUG
		print_debug("Solved: ");
		print_num_expr(data->full_expr);
		indent_spaces--;
	#endif
}

step	*solve_by_steps(num_expr *expr)
{
	step_list		steps;
	solving_data	data;

	steps.capacity = 1;
	steps.length = 0;
	steps.array = malloc(sizeof(step));
	data.full_expr = expr;
	data.steps = &steps;
	record_step(&data, STEP_INITIAL);
	solve_node(&data, expr);
	record_step(&data, -1);
	return (steps.array);
}
