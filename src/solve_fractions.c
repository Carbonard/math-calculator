#include "../includes/solve.h"

static int remove_factors(num_expr *frac, operand **op1, operand **op2)
{
	operand *aux;

	#ifdef DEBUG
		print_debug("simplifying one factor\n");
		print_operation(*op1, *op2);
	#endif
	if ((*op1)->prev)
	{
		aux = (*op1)->next;
		delete_operand(*op1);
		*op1 = aux;
	}
	else if ((*op1)->next)
		shift_left(*op1);
	else
	{
		delete_operand(*op1);
		*op1 = NULL;
		frac->operands->expr->type = ALG_NUMBER;
		frac->operands->expr->result = 1;
		frac->operands->expr->operands = NULL;
	}
	if ((*op2)->prev)
	{
		aux = (*op2)->next;
		delete_operand(*op2);
		*op2 = aux;
	}
	else if ((*op2)->next)
		shift_left(*op2);
	else
	{
		delete_operand(*op2);
		pull_first_operand(frac);
		return (1);
	}
	return (0);
}

void	simplify_frac(num_expr *expr, num_expr *frac)
{
	int		op1_advanced;

	if (frac->type != ALG_BINARY_OP || frac->subtype != OP_DIV)
		return;
	else
	if ((frac->operands->expr->type == ALG_BINARY_OP && frac->operands->expr->subtype == OP_PROD)
		&& (frac->operands->next->expr->type == ALG_BINARY_OP && frac->operands->next->expr->subtype == OP_PROD))
	{
		operand *op1 = frac->operands->expr->operands;
		while (op1)
		{
			operand *op2 = frac->operands->next->expr->operands;
			op1_advanced = 0;
			while (op2 && op1)
			{
				#ifdef DEBUG
					print_debug("check factors\n");
					print_operation(op1, op2);
				#endif
				if (!expr_cmp(op1->expr, op2->expr))
				{
					if (remove_factors(frac, &op1, &op2)) // If denominator was completely removed
					{
						print_num_expr(expr);
						return;
					}
					print_num_expr(expr);
					op1_advanced = 1;
				}
				else
					op2 = op2->next;
			}
			if (!op1_advanced)
				op1 = op1->next;
		}
	}
}

void	amplify_frac(num_expr *frac, operand *factor)
{
	operand_to_bin_op(frac->operands, OP_PROD, dup_operand(factor));
	operand_to_bin_op(frac->operands->next, OP_PROD, dup_operand(factor));
}


// (a/b)*(c/d) = (ac)/(bd) // (a/b)*c = (ac)/b
int	fraction_product(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation != OP_PROD)
		return (0);
	if (!(op1->expr->type == ALG_BINARY_OP && op1->expr->subtype == OP_DIV)
	 && !(op2->expr->type == ALG_BINARY_OP && op2->expr->subtype == OP_DIV))
		return (0);

	#ifdef DEBUG
		print_debug("Applying product of fraction\n");
	#endif

	detach_operand(op2);
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
	//				factor->frac->numerator->possible factors
	for (operand *op = op1->expr->operands->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	//				factor->frac->  denominator ->possible factors
	for (operand *op = op1->expr->operands->next->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	return (1);
	(void)expr;
}

// (a/b)/(c/d) = (ad)/(bc) // (a/b)/c = a/(bc) // a/(b/c) = (ac)/b
int	fraction_division(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation != OP_DIV)
		return (0);
	if (!(op1->expr->type == ALG_BINARY_OP && op1->expr->subtype == OP_DIV)
	&&  !(op2->expr->type == ALG_BINARY_OP && op2->expr->subtype == OP_DIV))
		return (0);

	#ifdef DEBUG
		print_debug("Applying division of fraction\n");
	#endif

	detach_operand(op2);
	if (op1->expr->subtype != OP_DIV)
	{
		operand_to_bin_op(op1, OP_PROD, op2->expr->operands->next);
		operand_to_bin_op(op1, OP_DIV, op2->expr->operands);
		free(op2->expr);
		free(op2);
	}
	else if (op2->expr->subtype != OP_DIV)
	{
		operand_to_bin_op(op1->expr->operands->next, OP_PROD, op2);
	}
	else
	{
		operand_to_bin_op(op1->expr->operands, OP_PROD, op2->expr->operands->next);
		operand_to_bin_op(op1->expr->operands->next, OP_PROD, op2->expr->operands);
		free(op2->expr);
		free(op2);
	}
	//				factor->frac->numerator->possible factors
	for (operand *op = op1->expr->operands->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	//				factor->frac->  denominator ->possible factors
	for (operand *op = op1->expr->operands->next->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	return (1);
	(void)expr;
}

// (a/b)+(c/d) // (a/b)+c // (a/b)-(c/d) // (a/b)-c
int	fraction_sum(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation != OP_SUM && op2->operation != OP_SUBS)
		return (0);
	if (!(op1->expr->type == ALG_BINARY_OP && op1->expr->subtype == OP_DIV)
	&&  !(op2->expr->type == ALG_BINARY_OP && op2->expr->subtype == OP_DIV))
		return (0);

	#ifdef DEBUG
		print_debug("Applying sum/subs of fraction\n");
	#endif

	if (op1->expr->subtype != OP_DIV)
	{
		operand_to_bin_op(op1, OP_PROD, dup_operand(op2->expr->operands->next));
		operand_to_bin_op(op1, OP_DIV, dup_operand(op2->expr->operands->next));
		print_num_expr(expr);
	}
	else if (op2->expr->subtype != OP_DIV)
	{
		operand_to_bin_op(op2, OP_PROD, dup_operand(op1->expr->operands->next));
		operand_to_bin_op(op2, OP_DIV, dup_operand(op1->expr->operands->next));
		print_num_expr(expr);
	}
	else if (expr_cmp(op1->expr->operands->next->expr, op2->expr->operands->next->expr))
	{
		operand *first_denom = dup_operand(op1->expr->operands->next);
		amplify_frac(op1->expr, op2->expr->operands->next);
		amplify_frac(op2->expr, first_denom);
		free_operand(first_denom);
		print_num_expr(expr);
	}
	detach_operand(op2);
	free_operand(op2->expr->operands->next);
	op2->expr->operands->next = NULL;
	operand_to_bin_op(op1->expr->operands, op2->operation, op2->expr->operands);
	op2->expr->operands = NULL;
	free_operand(op2);
	//				factor->frac->numerator->possible summands
	for (operand *op = op1->expr->operands->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	//				factor->frac->  denominator ->possible factors
	for (operand *op = op1->expr->operands->next->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	return (1);
}

