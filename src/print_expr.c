#include "../includes/calculator.h"

static int parenthesis_needed(num_expr *current, num_expr *next)
{
	// return (1);
	if (next->sign)
		return (1);
	if (next->type == ALG_BINARY_OP)
	{
		// return (1);
		if (!next->operands->next)
			return (0);
		if (current->subtype == OP_PROD && next->subtype == OP_SUM)
			return (1);
		if (current->subtype == OP_POW)
			return (1);
	}
	return (0);
}

static void	print_numexpr(num_expr *expr)
{
	char *bin_operands[] = {"","+","-","\\cdot","/","^"};
	char sing_operands[][10] =
	{
		[OP_SQRT] = "sqrt",
		[OP_ROOT] = "sqrt[n]",
		[OP_LN] = "ln",
		[OP_LOG] = "log",
		[OP_EXP] = "exp",
		[OP_ABS] = "abs"
	};

	#ifdef DEBUG
	if (expr->type == ALG_BINARY_OP)
		printf("sorting: type: %s, subtype: %s\n", algtypes[expr->type], binop[expr->subtype]);
	else if (expr->type == ALG_SINGLE_OP)
		printf("[%s]", sing_operands[expr->subtype]);
	#endif
	if (expr->sign)
		printf("-");
	if (expr->type == ALG_NUMBER)
		printf("%d", expr->result);
	else if (expr->type == ALG_BINARY_OP && expr->subtype == OP_DIV && expr->operands->next)
	{
		printf("\\frac{");
		if (expr->operands->expr->type == ALG_BINARY_OP && expr->operands->expr->subtype == OP_DIV)
			printf("\\left(");
		// printf("[%d,%d]", expr->operands->expr->type, expr->operands->expr->subtype);
		print_numexpr(expr->operands->expr);
		if (expr->operands->expr->type == ALG_BINARY_OP && expr->operands->expr->subtype == OP_DIV)
			printf("\\right)");
		printf("}{");
		if (expr->operands->next->expr->type == ALG_BINARY_OP && expr->operands->next->expr->subtype == OP_DIV)
			printf("\\left(");
		print_numexpr(expr->operands->next->expr);
		if (expr->operands->next->expr->type == ALG_BINARY_OP && expr->operands->next->expr->subtype == OP_DIV)
			printf("\\right)");
		printf("}");
	}
	else if (expr->type == ALG_BINARY_OP)
	{
		if (expr->sign)
			printf("\\left(");
		if (expr->operands->expr->type == ALG_BINARY_OP)
			if (parenthesis_needed(expr, expr->operands[0].expr))
				printf("\\left(");
		print_numexpr(expr->operands[0].expr);
		if (expr->operands->expr->type == ALG_BINARY_OP)
			if (parenthesis_needed(expr, expr->operands[0].expr))
				printf("\\right)");
		for (operand *op = expr->operands->next; op; op = op->next)
		{
			printf("%s",bin_operands[op->operation]);
			if (parenthesis_needed(expr, op->expr))
				printf("\\left(");
			print_numexpr(op->expr);
			if (parenthesis_needed(expr, op->expr))
				printf("\\right)");
		}
		if (expr->sign)
			printf("\\right)");
	}
	else if (expr->type == ALG_SINGLE_OP)
	{
		printf("\\%s{", sing_operands[expr->operands->operation]);
		print_numexpr(expr->operands->expr);
		printf("}");
	}
}

void	print_num_expr(num_expr *expr)
{
	print_numexpr(expr);
	printf("\n");
}

// void	print_number(number n)
// {
// 	if (n.sgn)
// 		printf("-");
// 	if (n.type == N_INTEGER)
// 	{
// 		printf("%d", n.integer);
// 	}
// 	if (n.type == N_RATIONAL)
// 		printf("\\frac{%d}{%d}", n.rational[0], n.rational[1]);
// 	if (n.type == N_ROOT)
// 	{
// 		if (n.root[1] == 2)
// 			printf("\\sqrt{%d}", n.root[0]);
// 		else
// 			printf("\\sqrt[%d]{%d}", n.root[1], n.root[0]);
// 	}
// 	if (n.type == N_LOG)
// 		printf("\\log_%d{%d}", n.log[1], n.log[0]);
// }
