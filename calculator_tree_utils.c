#include "calculator.h"

num_expr	*create_num_expr(int type, int subtype)
{
	num_expr *expr = malloc(sizeof(num_expr));

	expr->type = type;
	expr->subtype = subtype;
	expr->solved = 0;
	expr->sign = 0;
	expr->operands = NULL;
	return (expr);
}

num_expr	*create_num_num_expr(int sign, int result)
{
	num_expr *expr = malloc(sizeof(num_expr));

	expr->type = ALG_NUMBER;
	expr->subtype = 0;
	expr->solved = 1;
	expr->sign = sign;
	expr->result = result;
	expr->operands = NULL;
	return (expr);
}

operand	*create_operand(int operation, num_expr *expr)
{
	operand *new_op = malloc(sizeof(operand));

	new_op->next = NULL;
	new_op->prev = NULL;
	new_op->operation = operation;
	new_op->operand = expr;
	return (new_op);
}

void	append_operand(num_expr *node, operand *new_operand)
{
	operand		*aux;

	if (!(node->operands))
	{
		node->operands = new_operand;
		return;
	}
	aux = node->operands;
	while (aux->next)
		aux = aux->next;
	aux->next = new_operand;
	new_operand->prev = aux;
}

void	free_num_expr(num_expr *expr)
{
	if (!expr)
		return;
	operand *aux, *op = expr->operands;

	while (op)
	{
		aux = op;
		op = op->next;
		free_operand(aux);
	}
	expr->operands = NULL;
	free(expr);
}

void	free_operand(operand *op)
{
	if (!op)
		return;
	free_num_expr(op->operand);
	op->operand = NULL;
	free(op);
}

operand	*del_and_back(operand *op)
{
	operand *aux;

	aux = op;
	if (op->next)
		op->next->prev = op->prev;
	op = op->prev;
	op->next = op->next->next;
	free_operand(aux);
	return (op);
}
