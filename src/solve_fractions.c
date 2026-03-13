#include "../includes/solve.h"

static int remove_factors(num_expr *frac, operand **op_1, operand **op_2)
{
	operand *aux;

	#ifdef DEBUG
		print_debug("simplifying one factor\n");
		print_operation(*op_1, *op_2);
	#endif
	if ((*op_1)->prev)
	{
		aux = (*op_1)->next;
		delete_operand(*op_1);
		*op_1 = aux;
	}
	else if ((*op_1)->next)
		shift_left(*op_1);
	else
	{
		delete_operand(*op_1);
		*op_1 = NULL;
		frac->operands->expr->type = ALG_NUMBER;
		frac->operands->expr->result = 1;
		frac->operands->expr->operands = NULL;
	}
	if ((*op_2)->prev)
	{
		aux = (*op_2)->next;
		delete_operand(*op_2);
		*op_2 = aux;
	}
	else if ((*op_2)->next)
		shift_left(*op_2);
	else
	{
		delete_operand(*op_2);
		pull_first_operand(frac);
		return (1);
	}
	return (0);
}

void	simplify_frac(solving_data *data, num_expr *frac)
{
	int		op1_advanced;

	if (frac->type != ALG_BINARY_OP || frac->subtype != OP_DIV)
		return;
	else
	if ((frac->operands->expr->type == ALG_BINARY_OP && frac->operands->expr->subtype == OP_PROD)
		&& (frac->operands->next->expr->type == ALG_BINARY_OP && frac->operands->next->expr->subtype == OP_PROD))
	{
		operand *op_1 = frac->operands->expr->operands;
		while (op_1)
		{
			operand *op_2 = frac->operands->next->expr->operands;
			op1_advanced = 0;
			while (op_2 && op_1)
			{
				#ifdef DEBUG
					print_debug("check factors\n");
					print_operation(op_1, op_2);
				#endif
				if (!expr_cmp(op_1->expr, op_2->expr))
				{
					if (remove_factors(frac, &op_1, &op_2)) // If denominator was completely removed
					{
						record_step(data, STEP_SIMPLIFY_FRAC);
						return;
					}
					record_step(data, STEP_SIMPLIFY_FRAC);
					op1_advanced = 1;
				}
				else
					op_2 = op_2->next;
			}
			if (!op1_advanced)
				op_1 = op_1->next;
		}
	}
}

void	amplify_frac(num_expr *frac, operand *factor)
{
	operand_to_bin_op(frac->operands, OP_PROD, dup_operand(factor));
	operand_to_bin_op(frac->operands->next, OP_PROD, dup_operand(factor));
}

static int is_fraction(num_expr *expr)
{
	if (expr->type != ALG_BINARY_OP)
		return (0);
	if (expr->subtype != OP_DIV)
		return (0);
	return (1);
}

// (a/b)*(c/d) = (ac)/(bd) // (a/b)*c = (ac)/b
int	fraction_product(operation_context *ctx)
{
	if (ctx->op2->operation != OP_PROD)
		return (0);
	if (!is_fraction(ctx->op1->expr) && !is_fraction(ctx->op2->expr))
		return (0);

	#ifdef DEBUG
		print_debug("Applying product of fraction\n");
	#endif

	detach_operand(ctx->op2);
	if (!is_fraction(ctx->op1->expr))
	{
		operand_to_bin_op(ctx->op1, OP_DIV, ctx->op2->expr->operands->next);
		operand_to_bin_op(ctx->op1->expr->operands, OP_PROD, ctx->op2->expr->operands);
		free(ctx->op2->expr);
		free(ctx->op2);
		record_step(ctx->data, STEP_MULT_FRAC_NOFRAC);
	}
	else if (!is_fraction(ctx->op2->expr))
	{
		operand_to_bin_op(ctx->op1->expr->operands, OP_PROD, ctx->op2);
		record_step(ctx->data, STEP_MULT_FRAC_NOFRAC);
	}
	else
	{
		operand_to_bin_op(ctx->op1->expr->operands->next, OP_PROD, ctx->op2->expr->operands->next);
		operand_to_bin_op(ctx->op1->expr->operands, OP_PROD, ctx->op2->expr->operands);
		free(ctx->op2->expr);
		free(ctx->op2);
		record_step(ctx->data, STEP_MULT_FRACS);
	}
	//				factor->frac->numerator->possible factors
	for (operand *op = ctx->op1->expr->operands->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	//				factor->frac->  denominator ->possible factors
	for (operand *op = ctx->op1->expr->operands->next->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	return (1);
}

// (a/b)/(c/d) = (ad)/(bc) // (a/b)/c = a/(bc) // a/(b/c) = (ac)/b
int	fraction_division(operation_context *ctx)
{
	if (ctx->op2->operation != OP_DIV)
		return (0);
	if (!is_fraction(ctx->op1->expr) && !is_fraction(ctx->op2->expr))
		return (0);

	#ifdef DEBUG
		print_debug("Applying division of fraction\n");
	#endif

	detach_operand(ctx->op2);
	if (!is_fraction(ctx->op1->expr))
	{
		operand_to_bin_op(ctx->op1, OP_PROD, ctx->op2->expr->operands->next);
		operand_to_bin_op(ctx->op1, OP_DIV, ctx->op2->expr->operands);
		free(ctx->op2->expr);
		free(ctx->op2);
		record_step(ctx->data, STEP_DIV_NUM_BY_FRAC);
	}
	else if (!is_fraction(ctx->op2->expr))
	{
		operand_to_bin_op(ctx->op1->expr->operands->next, OP_PROD, ctx->op2);
		record_step(ctx->data, STEP_DIV_FRAC_BY_NUM);
	}
	else
	{
		operand_to_bin_op(ctx->op1->expr->operands, OP_PROD, ctx->op2->expr->operands->next);
		operand_to_bin_op(ctx->op1->expr->operands->next, OP_PROD, ctx->op2->expr->operands);
		free(ctx->op2->expr);
		free(ctx->op2);
		record_step(ctx->data, STEP_DIV_FRACS);
	}
	//				factor->frac->numerator->possible factors
	for (operand *op = ctx->op1->expr->operands->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	//				factor->frac->  denominator ->possible factors
	for (operand *op = ctx->op1->expr->operands->next->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	return (1);
}

// (a/b)+(c/d) // (a/b)+c // (a/b)-(c/d) // (a/b)-c
int	fraction_sum(operation_context *ctx)
{
	if (ctx->op2->operation != OP_SUM && ctx->op2->operation != OP_SUBS)
		return (0);
	if (!is_fraction(ctx->op1->expr) && !is_fraction(ctx->op2->expr))
		return (0);

	#ifdef DEBUG
		print_debug("Applying sum/subs of fraction\n");
	#endif

	if (!is_fraction(ctx->op1->expr))
	{
		operand_to_bin_op(ctx->op1, OP_PROD, dup_operand(ctx->op2->expr->operands->next));
		operand_to_bin_op(ctx->op1, OP_DIV, dup_operand(ctx->op2->expr->operands->next));
		record_step(ctx->data, STEP_MULT_FRAC_NOFRAC);
	}
	else if (!is_fraction(ctx->op2->expr))
	{
		operand_to_bin_op(ctx->op2, OP_PROD, dup_operand(ctx->op1->expr->operands->next));
		operand_to_bin_op(ctx->op2, OP_DIV, dup_operand(ctx->op1->expr->operands->next));
		record_step(ctx->data, STEP_MULT_FRAC_NOFRAC);
	}
	else if (expr_cmp(ctx->op1->expr->operands->next->expr, ctx->op2->expr->operands->next->expr))
	{
		operand *first_denom = dup_operand(ctx->op1->expr->operands->next);
		amplify_frac(ctx->op1->expr, ctx->op2->expr->operands->next);
		amplify_frac(ctx->op2->expr, first_denom);
		free_operand(first_denom);
		record_step(ctx->data, STEP_AMPLIFY_FRACS);
	}
	detach_operand(ctx->op2);
	free_operand(ctx->op2->expr->operands->next);
	ctx->op2->expr->operands->next = NULL;
	operand_to_bin_op(ctx->op1->expr->operands, ctx->op2->operation, ctx->op2->expr->operands);
	ctx->op2->expr->operands = NULL;
	free_operand(ctx->op2);
	//				factor->frac->numerator->possible summands
	for (operand *op = ctx->op1->expr->operands->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	//				factor->frac->  denominator ->possible factors
	for (operand *op = ctx->op1->expr->operands->next->expr->operands; op; op = op->next)
		apply_associative(NULL, op);
	record_step(ctx->data, STEP_SUM_FRACS);
	return (1);
}
