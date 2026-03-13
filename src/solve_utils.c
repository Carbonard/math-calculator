#include "../includes/solve.h"

void	insert_operands(operand *op, int operation, operand *operands)
// insert operands between op and op->next
{
	operand *last;

	last = operands;
	while (last->next)
		last = last->next;
	last->next = op->next;
	if (op->next)
		op->next->prev = last;
	op->next = operands;
	operands->prev = op;
	operands->operation = operation;
}

operand	*expand_operand(operand *op)
// Substitute op by op->expr->operands
{
	operand *operands = op->expr->operands;

	insert_operands(op, op->operation, operands);
	if (op->prev)
	{
		detach_operand(op);
		free(op->expr);
		free(op);
		return (operands);
	}
	else
	{
		op->expr->operands = NULL;
		shift_left(op);
		return (op);
	}
}

num_expr	*dup_expr(num_expr *original)
{
	num_expr *copy = malloc(sizeof(num_expr));

	copy->type = original->type;
	copy->subtype = original->subtype;
	copy->sign = original->sign;
	copy->result = original->result;
	copy->operands = NULL;
	for (operand *op = original->operands; op; op = op->next)
	{
		append_operand(copy, dup_operand(op));
	}
	return (copy);
}

operand	*dup_operand(operand *original)
{
	operand *copy = malloc(sizeof(operand));

	copy->expr = dup_expr(original->expr);
	copy->next = NULL;
	copy->prev = NULL;
	copy->operation = original->operation;
	return (copy);
}

int	expr_cmp(num_expr *expr1, num_expr *expr2)
// returns positive if expr1 > expr2, negative if expr1 < expr2 and 0 id equal
{
	if (expr1->type	!= expr2->type)
		return (expr1->type - expr2->type);
	if (expr1->subtype	!= expr2->subtype)
		return (expr1->subtype - expr2->subtype);
	if (expr1->sign		!= expr2->sign)
		return (expr1->sign - expr2->sign);
	if (expr1->result	!= expr2->result)
		return (expr1->result - expr2->result);

	operand *op1 = expr1->operands, *op2 = expr2->operands;
	int		aux;
	while (op1 && op2)
	{
		if (op1->operation != op2->operation)
			return (op1->operation - op2->operation);
		aux = expr_cmp(op1->expr, op2->expr);
		if (aux)
			return (aux);
		op1 = op1->next;
		op2 = op2->next;
	}
	if (op1)
		return (1);
	if (op2)
		return (-1);
	return (0);
}

int	expr_is_num(num_expr *expr, integer n)
{
	return (expr->type == ALG_NUMBER && expr->result == n);
}

operand	*replace_operand(operand *copy, operand *original)
{
	free_num_expr(copy->expr);
	copy->expr = original->expr;
	copy->next = original->next;
	copy->prev = original->prev;
	copy->operation = original->operation;
	return (copy);
}

void	shift_left(operand *op)
{
	operand *original_next = op->next, *original_prev = op->prev;

	if (!op->next)
	{
		op->expr = NULL;
		op->operation = OP_ERR;
		return;
	}
	replace_operand(op, op->next);
	op->prev = original_prev;
	if (op->next)
		op->next->prev = op;
	free(original_next);
}

void	detach_operand(operand *op)
{
	if (op->prev)
		op->prev->next = op->next;
	if (op->next)
		op->next->prev = op->prev;
}

operand	*simplify_identity(solving_data *data, operand *op)
{
	operand *aux, *current_operand = op;

	if (!op->prev)
	{
		if (expr_is_num(op->expr, 0) && op->next->operation == OP_SUM)
		{
			shift_left(op);
			record_step(data, STEP_SUM_ZERO);
		}
		if (expr_is_num(op->expr, 1) && op->next->operation == OP_PROD)
		{
			shift_left(op);
			record_step(data, STEP_MULT_ONE);
		}
		else if (expr_is_num(op->expr, 0) && op->next->operation == OP_PROD)
		{
			delete_operand(op->next);
			record_step(data, STEP_MULT_ZERO);
		}
		else return (op);
	}
	else
	{
		if (expr_is_num(op->expr, 0) && op->operation == OP_SUM)
		{
			current_operand = op->next;
			delete_operand(op);
			record_step(data, STEP_SUM_ZERO);
		}
		if (expr_is_num(op->expr, 1) && op->operation == OP_PROD)
		{
			current_operand = op->next;
			delete_operand(op);
			record_step(data, STEP_MULT_ONE);
		}
		else if (expr_is_num(op->expr, 0) && op->operation == OP_PROD)
		{
			current_operand = op->next;
			aux = op->prev->prev;
			replace_operand(op->prev, op);
			op->prev->prev = aux;
			detach_operand(op);
			free(op);
			record_step(data, STEP_MULT_ZERO);
		}
		else return (op);
	}
	return (current_operand);
}

void	swap_operands(operand *op1, operand *op2)
{
	num_expr	*aux_expr;
	int			aux_op;

	#ifdef DEBUG
		print_debug("swapping operands:");
		printind();
		print_num_expr(op1->expr);
		printind();
		print_num_expr(op2->expr);
	#endif
	aux_expr = op1->expr;
	op1->expr = op2->expr;
	op2->expr = aux_expr;
	aux_op = op1->operation;
	op1->operation = op2->operation;
	if (aux_op)
		op2->operation = aux_op;
}

operand	*delete_operand(operand *op)
{
	operand *aux;

	aux = op->prev;
	detach_operand(op);
	free_operand(op);
	return (aux);
}
