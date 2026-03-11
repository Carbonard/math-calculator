#include "../includes/solve.h"

typedef int (*property)(num_expr*, operand*, operand*);

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
static int	sqrt_product(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation != OP_PROD)
		return (0);
	if 	(!(op1->expr->type == ALG_SINGLE_OP && op1->expr->subtype == OP_SQRT
		&& op2->expr->type == ALG_SINGLE_OP && op2->expr->subtype == OP_SQRT))
		return (0);

	#ifdef DEBUG
		print_debug("Applying sqrt(a)*sqrt(b) = sqrt(a*b)\n");
	#endif

	detach_operand(op2);
	operand_to_bin_op(op1->expr->operands, OP_PROD, op2->expr->operands);
	free(op2->expr);
	free(op2);
	return (1);
	(void)expr;
}

// a/sqrt(b) = a*sqrt(b)/b
static int	sqrt_denominator(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation != OP_DIV)
		return (0);
	if (!(op2->expr->type == ALG_SINGLE_OP && op2->expr->subtype == OP_SQRT))
		return (0);

	#ifdef DEBUG
		print_debug("Solving sqrt in denominator\n");
	#endif

	operand_to_bin_op(op1, OP_PROD, dup_operand(op2));
	operand_to_bin_op(op2, OP_PROD, dup_operand(op2));
	return (1);
	(void)expr;
}

static int	apply_properties(num_expr *expr, num_expr *node, operand *op1, operand *op2, property prop[])
{
	if (manage_signs(op1, op2))
		print_num_expr(expr);

	#ifdef DEBUG
		printind();
		printf("Managing properies\n");
	#endif
	
	while (*prop)
	{
		if ((*prop)(expr, op1, op2))
		{
			if (!node->operands->next)
				pull_first_operand(node);
			print_num_expr(expr);
			solve_node_didactic(expr, node);
			return (1);
		}
		prop++;
	}

	return (0);
}

int	apply_properties_1(num_expr *expr, num_expr *node, operand *op1, operand *op2)
{
	property properties[3] = 
	{
		sqrt_product,
		sqrt_denominator,
		NULL
	};
	
	return (apply_properties(expr, node, op1, op2, properties));
}

int	apply_properties_2(num_expr *expr, num_expr *node, operand *op1, operand *op2)
{
	property properties[4] = 
	{
		fraction_product,
		fraction_division,
		fraction_sum,
		NULL
	};
	
	return (apply_properties(expr, node, op1, op2, properties));
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
		op = expand_operand(op);
		if (expr)
			print_num_expr(expr);
	}
	return (op);
}
