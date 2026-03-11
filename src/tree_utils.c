#include "../includes/tree.h"

num_expr	*create_num_expr(int type, int subtype)
{
	num_expr *expr = malloc(sizeof(num_expr));

	expr->type = type;
	expr->subtype = subtype;
	// expr->solved = 0;
	expr->sign = 0;
	expr->operands = NULL;
	expr->result = 0;
	return (expr);
}

num_expr	*create_num_num_expr(int sign, int result)
{
	num_expr *expr = malloc(sizeof(num_expr));

	expr->type = ALG_NUMBER;
	expr->subtype = 0;
	// expr->solved = 1;
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
	new_op->expr = expr;
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

void	operand_to_bin_op(operand *left, int operation, operand *right)
/* previous right->prev and right->next are lost */
{
	num_expr	*original_left;

	// Save original
	original_left = left->expr;
	// Create the binary operation structure
	left->expr = create_num_expr(ALG_BINARY_OP, operation);
	// Add left operand as first binary operand
	left->expr->operands = create_operand(operation, original_left);
	left->expr->operands->next = right;
	// Extract right operand from its operands list
	if (right->prev)
		right->prev->next = right->next;
	if (right->next)
		right->next->prev = right->prev;
	// Insert right operand to binary operation
	right->next = NULL;
	right->prev = left->expr->operands;
	right->operation = operation;
}

int pull_first_operand(num_expr *node)
{
	operand *aux = node->operands;
	int		sign_changed = 0;

	#ifdef DEBUG
		print_debug("Pulling first op\n");
	#endif
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

operand	*last_operand(num_expr *expr)
{
	operand *op;

	op = expr->operands;
	while (op && op->next)
		op = op->next;
	return (op);
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
	free_num_expr(op->expr);
	op->expr = NULL;
	free(op);
}
