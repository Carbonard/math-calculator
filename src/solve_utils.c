#include "../includes/solve.h"

void	insert_operands(operand *op, int operation, operand *operands)
// insert operands between op and op->next
{
	operand *last;

	last = operands;
	while (last->next)
		last = last->next;
	last->next = op->next;
	op->next = operands;
	operands->operation = operation;
	operands->prev = op;
}

void	expand_operand(operand **op)
// Substitute op by operands. Assumes op->operand doesn't need to be liberated
{
	operand *aux, *operands = (*op)->expr->operands;

	// insert_operands(*op, operands);
	// aux = delete_operand(*op);
	// if (aux)
	// 	*op = aux;
	// else
	// 	*op = operands;
	if ((*op)->prev)
		(*op)->prev->next = operands;
	aux = operands;
	while (aux->next)
		aux = aux->next;
	aux->next = (*op)->next;
	operands->operation = (*op)->operation;
	operands->prev = (*op)->prev;
	free((*op)->expr);
	free(*op);
	*op = operands;
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

void	detach_operand(operand *op)
{
	if (op->prev)
		op->prev->next = op->next;
	if (op->next)
		op->next->prev = op->prev;
}

int simplify_identity(operand *op)
{
	operand *aux;

	if (!op->prev)
	{
		if (expr_is_num(op->expr, 0) && op->next->operation == OP_SUM)
		{
			aux = op->next;
			replace_operand(op, op->next);
			op->prev = NULL;
			detach_operand(aux);
			free(aux);
		}
		else if (expr_is_num(op->expr, 0) && op->next->operation == OP_PROD)
			delete_operand(op->next);
		else
				return (0);
	}
	else
	{
		if (expr_is_num(op->expr, 0) && op->operation == OP_SUM)
			delete_operand(op);
		else if (expr_is_num(op->expr, 0) && op->operation == OP_PROD)
		{
			aux = op->prev->prev;
			replace_operand(op->prev, op);
			op->prev->prev = aux;
			detach_operand(op);
			free(op);
		}
		else
			return (0);
	}
	return (1);
}

operand	*delete_operand(operand *op)
{
	operand *aux;

	aux = op->prev;
	detach_operand(op);
	free_operand(op);
	return (aux);
}
