#include "../includes/solve.h"

static void	sum_int(num_expr *a, num_expr *b)
{
	if (a->sign == b->sign)
	{
		a->result = a->result + b->result;
		a->sign = a->sign;
	}
	else if (a->result >= b->result)
	{
		a->result = a->result - b->result;
		a->sign = a->sign;
	}
	else
	{
		a->result = b->result - a->result;
		a->sign = b->sign;
	}
}

static void	prod_int(num_expr *expr, num_expr *a, num_expr *b)
{
	if (b->sign)
	{
		a->sign = (a->sign != b->sign);
		print_num_expr(expr);
	}
	a->result = a->result * b->result;
}

static int	simplify_num_div(num_expr *a, num_expr *b)
{
	integer div = gcd(a->result, b->result);

	if (div < 2)
		return (0);
	a->result /= div;
	b->result /= div;
	return (1);
}

static int	div_int(num_expr *expr, num_expr *a, num_expr *b)
{
	if (a->result % b->result)
	{
		if (simplify_num_div(a, b))
			print_num_expr(expr);
		return (0);
	}
	if (a->sign || b->sign)
	{
		a->sign = (a->sign != b->sign);
		print_num_expr(expr);
	}
	a->result = a->result / b->result;
	return (1);
}

int	operate_numbers(num_expr *expr, operand *op1, operand *op2)
{
	if (op2->operation == OP_SUM)
		sum_int(op1->expr, op2->expr);
	else if (op2->operation == OP_SUBS)
	{
		if (op2->expr->sign)
		{
			op2->expr->sign = 0;
			op2->operation = OP_SUM;
			print_num_expr(expr);
		}
		else
			op2->expr->sign = 1;
		sum_int(op1->expr, op2->expr);
	}
	else if (op2->operation == OP_PROD)
		prod_int(expr, op1->expr, op2->expr);
	else if (op2->operation == OP_DIV)
		return (div_int(expr, op1->expr, op2->expr));
	else
		return (0);
	return (1);
}
