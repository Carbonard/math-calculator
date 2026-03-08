#include "../includes/calculator.h"

static int parenthesis_needed(num_expr *current, num_expr *next)
{
	if (next->sign)
		return (1);
	if (next->type == ALG_BINARY_OP)
	{
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
	char sing_operands[][5] =
	{
		[OP_SQRT] = "sqrt",
		[OP_ROOT] = "root",
		[OP_LN] = "ln",
		[OP_LOG] = "log",
		[OP_EXP] = "exp",
		[OP_ABS] = "abs"
	};

	if (expr->sign)
		printf("-");
	if (expr->type == ALG_NUMBER)
		printf("%d", expr->result);
	else if (expr->type == ALG_BINARY_OP
			&& (expr->subtype != OP_DIV || !expr->operands->next))
	{
		if (expr->sign)
			printf("(");
		if (expr->operands->expr->type == ALG_BINARY_OP)
			if (parenthesis_needed(expr, expr->operands[0].expr))
				printf("(");
		print_numexpr(expr->operands[0].expr);
		if (expr->operands->expr->type == ALG_BINARY_OP)
			if (parenthesis_needed(expr, expr->operands[0].expr))
				printf(")");
		for (operand *op = expr->operands->next; op; op = op->next)
		{
			printf("%s",bin_operands[op->operation]);
			if (parenthesis_needed(expr, op->expr))
				printf("(");
			print_numexpr(op->expr);
			if (parenthesis_needed(expr, op->expr))
				printf(")");
		}
		if (expr->sign)
			printf(")");
	}
	else if (expr->type == ALG_BINARY_OP && expr->subtype == OP_DIV)
	{
		printf("\\frac{");
		print_numexpr(expr->operands->expr);
		printf("}{");
		print_numexpr(expr->operands->next->expr);
		printf("}");
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
