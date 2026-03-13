#include "../includes/solve.h"

typedef int (*property)(operation_context*);

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

// sqrt(a)*sqrt(b) = sqrt(a*b)
static int	sqrt_product(operation_context *ctx)
{
	if (ctx->op2->operation != OP_PROD)
		return (0);
	if 	(!(ctx->op1->expr->type == ALG_SINGLE_OP && ctx->op1->expr->subtype == OP_SQRT
		&& ctx->op2->expr->type == ALG_SINGLE_OP && ctx->op2->expr->subtype == OP_SQRT))
		return (0);

	#ifdef DEBUG
		print_debug("Applying sqrt(a)*sqrt(b) = sqrt(a*b)\n");
	#endif

	detach_operand(ctx->op2);
	operand_to_bin_op(ctx->op1->expr->operands, OP_PROD, ctx->op2->expr->operands);
	free(ctx->op2->expr);
	free(ctx->op2);
	record_step(ctx->data, STEP_MULT_SQRT);
	return (1);
}

// a/sqrt(b) = a*sqrt(b)/b
static int	sqrt_denominator(operation_context *ctx)
{
	if (ctx->op2->operation != OP_DIV)
		return (0);
	if (!(ctx->op2->expr->type == ALG_SINGLE_OP && ctx->op2->expr->subtype == OP_SQRT))
		return (0);

	#ifdef DEBUG
		print_debug("Solving sqrt in denominator\n");
	#endif

	amplify_frac(ctx->node, ctx->op2);
	// operand_to_bin_op(ctx->op1, OP_PROD, dup_operand(ctx->op2));
	// operand_to_bin_op(ctx->op2, OP_PROD, dup_operand(ctx->op2));
	record_step(ctx->data, STEP_AMPLIFY_FRAC);
	return (1);
}

static int	apply_properties(solving_data *data, num_expr *node, operand *op1, operand *op2, property prop[])
{
	operation_context ctx;

	if (manage_signs(op1, op2))
		record_step(data, STEP_SIGN);

	#ifdef DEBUG
		printind();
		printf("Managing properies\n");
	#endif

	ctx.data = data;
	ctx.node = node;
	ctx.op1 = op1;
	ctx.op2 = op2;
	
	while (*prop)
	{
		if ((*prop)(&ctx))
		{
			if (!node->operands->next)
				pull_first_operand(node);
			solve_node(data, node);
			return (1);
		}
		prop++;
	}

	return (0);
}

int	apply_properties_1(solving_data *data, num_expr *node, operand *op1, operand *op2)
{
	property properties[3] = 
	{
		sqrt_product,
		sqrt_denominator,
		NULL
	};
	
	return (apply_properties(data, node, op1, op2, properties));
}

int	apply_properties_2(solving_data *data, num_expr *node, operand *op1, operand *op2)
{
	property properties[4] = 
	{
		fraction_product,
		fraction_division,
		fraction_sum,
		NULL
	};
	
	return (apply_properties(data, node, op1, op2, properties));
}

operand	*apply_associative(solving_data *data, operand *op)
{
	if (op->expr->type == ALG_BINARY_OP
		&& op->operation == op->expr->subtype
		&& (op->operation == OP_SUM || op->operation == OP_PROD))
	{
		#ifdef DEBUG
			printind();
			printf("Applying associative\n");
		#endif
		op = expand_operand(op);
		if (data->full_expr)
			record_step(data, STEP_ASSOCIATIVE);
	}
	return (op);
}
