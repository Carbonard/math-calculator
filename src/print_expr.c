#include "../includes/calculator.h"
#include "../includes/prints.h"

#define save_text()

static int parenthesis_needed(const num_expr *current, num_expr *next)
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
		if (current->subtype == next->subtype)
			return (1);
	}
	return (0);
}

static void	dsprint_numexpr(const num_expr *expr, dynamic_str *output)
{
	char *bin_operands[] = {"","+","-","\\\\cdot","/","^"};
	char sing_operands[][10] =
	{
		[OP_SQRT] = "sqrt",
		[OP_ROOT] = "sqrt[n]",
		[OP_LN] = "ln",
		[OP_LOG] = "log",
		[OP_EXP] = "exp",
		[OP_ABS] = "abs"
	};
	// #ifdef DEBUG
	// if (expr->type == ALG_BINARY_OP)
	// 	dynamic_strcatf(output, "sorting: type: %s, subtype: %s\n", algtypes[expr->type], binop[expr->subtype]);
	// else if (expr->type == ALG_SINGLE_OP)
	// 	dynamic_strcatf(output, "[%s]", sing_operands[expr->subtype]);
	// #endif

	if (expr->sign)
		dynamic_strcatf(output, "-");
	if (expr->type == ALG_NUMBER)
		dynamic_strcatf(output, "%d", expr->result);
	else if (expr->type == ALG_BINARY_OP && expr->subtype == OP_DIV && expr->operands->next)
	{
		dynamic_strcatf(output, "\\\\frac{");
		if (expr->operands->expr->type == ALG_BINARY_OP && expr->operands->expr->subtype == OP_DIV)
			dynamic_strcatf(output, "\\\\left(");
		// dynamic_strcatf(output, "[%d,%d]", expr->operands->expr->type, expr->operands->expr->subtype);
		dsprint_numexpr(expr->operands->expr, output);
		if (expr->operands->expr->type == ALG_BINARY_OP && expr->operands->expr->subtype == OP_DIV)
			dynamic_strcatf(output, "\\\\right)");
		dynamic_strcatf(output, "}{");
		if (expr->operands->next->expr->type == ALG_BINARY_OP && expr->operands->next->expr->subtype == OP_DIV)
			dynamic_strcatf(output, "\\\\left(");
		dsprint_numexpr(expr->operands->next->expr, output);
		if (expr->operands->next->expr->type == ALG_BINARY_OP && expr->operands->next->expr->subtype == OP_DIV)
			dynamic_strcatf(output, "\\\\right)");
		dynamic_strcatf(output, "}");
	}
	else if (expr->type == ALG_BINARY_OP)
	{
		if (expr->sign)
			dynamic_strcatf(output, "\\\\left(");
		if (expr->operands->expr->type == ALG_BINARY_OP)
			if (parenthesis_needed(expr, expr->operands[0].expr))
				dynamic_strcatf(output, "\\\\left(");
		dsprint_numexpr(expr->operands[0].expr, output);
		if (expr->operands->expr->type == ALG_BINARY_OP)
			if (parenthesis_needed(expr, expr->operands[0].expr))
				dynamic_strcatf(output, "\\\\right)");
		for (operand *op = expr->operands->next; op; op = op->next)
		{
			dynamic_strcatf(output, "%s",bin_operands[op->operation]);
			if (parenthesis_needed(expr, op->expr))
				dynamic_strcatf(output, "\\\\left(");
			dsprint_numexpr(op->expr, output);
			if (parenthesis_needed(expr, op->expr))
				dynamic_strcatf(output, "\\\\right)");
		}
		if (expr->sign)
			dynamic_strcatf(output, "\\\\right)");
	}
	else if (expr->type == ALG_SINGLE_OP)
	{
		dynamic_strcatf(output, "\\\\%s{", sing_operands[expr->operands->operation]);
		dsprint_numexpr(expr->operands->expr, output);
		dynamic_strcatf(output, "}");
	}
}

char *num_expr_to_str(const num_expr* expr)
{
	dynamic_str str;
	str.str = calloc(1,1);
	str.length = 0;
	str.size = 1;
	dsprint_numexpr(expr, &str);
	return(str.str);
}

void	print_num_expr(const num_expr *expr)
{
	char *str = num_expr_to_str(expr);
	puts(str);
	free(str);
}

// void	print_number(number n)
// {
// 	if (n.sgn)
// 		dynamic_strcatf(output, "-");
// 	if (n.type == N_INTEGER)
// 	{
// 		dynamic_strcatf(output, "%d", n.integer);
// 	}
// 	if (n.type == N_RATIONAL)
// 		dynamic_strcatf(output, "\\\\frac{%d}{%d}", n.rational[0], n.rational[1]);
// 	if (n.type == N_ROOT)
// 	{
// 		if (n.root[1] == 2)
// 			dynamic_strcatf(output, "\\\\sqrt{%d}", n.root[0]);
// 		else
// 			dynamic_strcatf(output, "\\\\sqrt[%d]{%d}", n.root[1], n.root[0]);
// 	}
// 	if (n.type == N_LOG)
// 		dynamic_strcatf(output, "\\\\log_%d{%d}", n.log[1], n.log[0]);
// }
