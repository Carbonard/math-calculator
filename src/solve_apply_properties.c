#include "../includes/solve.h"

static int manage_signs(operand *op_1, operand *op_2)
{
	char	*sgn1 = &op_1->expr->sign, *sgn2 = &op_2->expr->sign;

	#ifdef DEBUG
		printind();
		printf("Managing signs\n");
	#endif
	// Manage signs in sum and subs
	if (op_2->operation == OP_SUBS && *sgn2 == 1)
	{
		op_2->operation = OP_SUM;
		*sgn2 = 0;
	}
	else if ((op_2->operation == OP_SUM && *sgn2 == 1))
	{
		op_2->operation = OP_SUBS;
		*sgn2 = 0;
	}
	// Manage signs in prod and div
	else if ((op_2->operation == OP_PROD || op_2->operation == OP_DIV) && !*sgn1 && *sgn2)
	{
		*sgn1 = 1;
		*sgn2 = 0;
	}
	else if ((op_2->operation == OP_PROD || op_2->operation == OP_DIV) && (*sgn1 == *sgn2) && *sgn1)
	{
		*sgn1 = 0;
		*sgn2 = 0;
	}
	else
		return (0);
	return (1);
}

static void	sqrt_product(operand *op1, operand *op2)
{
	#ifdef DEBUG
		printind();
		printf("Applying sqrt(a)*sqrt(b) = sqrt(a*b)\n");
	#endif
	num_expr *new_expr = create_num_expr(ALG_BINARY_OP, OP_PROD);
	append_operand(new_expr, create_operand(0, op1->expr->operands->expr));
	append_operand(new_expr, create_operand(OP_PROD, op2->expr->operands->expr));
	op1->expr->operands->expr = new_expr;
	op2->expr->operands->expr = NULL;
	delete_operand(op2);
}

static void	fraction_product(operand *op1, operand *op2)
{
	#ifdef DEBUG
		printind();
		printf("Applying product of fraction\n");
	#endif
	op2->prev->next = op2->next;
	if (op2->next)
		op2->next->prev = op2->prev;
	if (op1->expr->subtype != OP_DIV)
	{
		operand_to_bin_op(op1, OP_DIV, op2->expr->operands->next);
		operand_to_bin_op(op1->expr->operands, OP_PROD, op2->expr->operands);
		free(op2->expr);
		free(op2);
	}
	else if (op2->expr->subtype != OP_DIV)
		operand_to_bin_op(op1->expr->operands, OP_PROD, op2);
	else
	{
		operand_to_bin_op(op1->expr->operands->next, OP_PROD, op2->expr->operands->next);
		operand_to_bin_op(op1->expr->operands, OP_PROD, op2->expr->operands);
		free(op2->expr);
		free(op2);
	}
}

static void	fraction_sum(num_expr *expr, operand *op1, operand *op2)
{
	#ifdef DEBUG
		print_debug("Applying sum of fraction\n");
	#endif
	if (op1->expr->subtype != OP_DIV)
	{
		#ifdef DEBUG
			print_debug("transform op1 into fraction\n");
		#endif
		operand_to_bin_op(op1, OP_PROD, dup_operand(op2->expr->operands->next));
		operand_to_bin_op(op1, OP_DIV, dup_operand(op2->expr->operands->next));
		print_num_expr(expr);
	}
	else if (op2->expr->subtype != OP_DIV)
	{
		#ifdef DEBUG
			print_debug("Transform op2 into fraction\n");
		#endif
		operand_to_bin_op(op2, OP_PROD, dup_operand(op1->expr->operands->next));
		operand_to_bin_op(op2, OP_DIV, dup_operand(op1->expr->operands->next));
		print_num_expr(expr);
	}
	else if (expr_cmp(op1->expr->operands->next->expr, op2->expr->operands->next->expr))
	{
		operand *first_denom = dup_operand(op1->expr->operands->next);
		operand_to_bin_op(op1->expr->operands, OP_PROD, dup_operand(op2->expr->operands->next));
		operand_to_bin_op(op1->expr->operands->next, OP_PROD, dup_operand(op2->expr->operands->next));
		operand_to_bin_op(op2->expr->operands, OP_PROD, dup_operand(first_denom));
		operand_to_bin_op(op2->expr->operands->next, OP_PROD, first_denom);
		print_num_expr(expr);
	}
	op2->prev->next = op2->next;
	if (op2->next)
		op2->next->prev = op2->prev;
	free_operand(op2->expr->operands->next);
	op2->expr->operands->next = NULL;
	operand_to_bin_op(op1->expr->operands, OP_SUM, op2->expr->operands);
	op2->expr->operands = NULL;
	free_operand(op2);
}

int	apply_properties(num_expr *expr, operand *op1, operand *op2)
// this joins op1 and op2 in op1 and return previous op2 (or NULL if anything happened)
{
	if (manage_signs(op1, op2))
		print_num_expr(expr);
	#ifdef DEBUG
		printind();
		printf("Managing propieries\n");
	#endif

	if (simplify_identity(op1) || simplify_identity(op2))
		return (1);

	// sqrt(a)*sqrt(b) = sqrt(a*b)
	if (op2->operation == OP_PROD
		&& op1->expr->type == ALG_SINGLE_OP && op1->expr->subtype == OP_SQRT
		&& op2->expr->type == ALG_SINGLE_OP && op2->expr->subtype == OP_SQRT)
		sqrt_product(op1, op2);

	// (a/b)*(c/d) = (ac)/(bd) // (a/b)*c = (ac)/b
	else if (op2->operation == OP_PROD
		&& ((op1->expr->type == ALG_BINARY_OP && op1->expr->subtype == OP_DIV)
		|| (op2->expr->type == ALG_BINARY_OP && op2->expr->subtype == OP_DIV)))
		fraction_product(op1, op2);

	// (a/b)+(c/d) // (a/b)+c
	else if (op2->operation == OP_SUM
		&& ((op1->expr->type == ALG_BINARY_OP && op1->expr->subtype == OP_DIV)
		|| (op2->expr->type == ALG_BINARY_OP && op2->expr->subtype == OP_DIV)))
		fraction_sum(expr, op1, op2);

	else
		return (0);

	return (1);
}

operand	*apply_associative(num_expr *expr, operand *op)
{
	if (op->expr->type == ALG_BINARY_OP
		&& op->operation == op->expr->subtype
		&& (op->operation == OP_SUM || op->operation == OP_PROD))
	{
		#ifdef DEBUG
			printind();
			printf("Applying associative\n");
		#endif
		expand_operand(&op);
		print_num_expr(expr);
	}
	return (op);
}
