#include "../includes/solve.h"

static void	sum_int(operation_context *ctx)
{
	num_expr *a=ctx->op1->expr, *b=ctx->op2->expr;

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
	detach_operand(ctx->op2);
	free_operand(ctx->op2);
	record_step(ctx->data, STEP_SUM_NUMS);
}

static void	subs_int(operation_context *ctx)
{
	num_expr *a=ctx->op1->expr, *b=ctx->op2->expr;

	b->sign = !b->sign;
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
	detach_operand(ctx->op2);
	free_operand(ctx->op2);
	record_step(ctx->data, STEP_SUBS_NUMS);
}

static void	prod_int(operation_context *ctx)
{
	num_expr *a=ctx->op1->expr, *b=ctx->op2->expr;

	if (b->sign)
	{
		a->sign = (a->sign != b->sign);
		record_step(ctx->data, STEP_SIGN);
	}
	a->result = a->result * b->result;
	detach_operand(ctx->op2);
	free_operand(ctx->op2);
	record_step(ctx->data, STEP_MULT_NUMS);
}

static void	simplify_num_div(solving_data *data, num_expr *a, num_expr *b)
{
	integer div = gcd(a->result, b->result);

	if (div < 2)
	{
		data->record = 0;
		return;
	}
	a->result /= div;
	b->result /= div;
	record_step(data, STEP_SIMPLIFY_FRAC);
}

static void	div_int(operation_context *ctx)
{
	num_expr *a=ctx->op1->expr, *b=ctx->op2->expr;

	if (a->result % b->result)
	{
		simplify_num_div(ctx->data, a, b);
		return ;
	}
	if (b->sign)
	{
		a->sign = (a->sign != b->sign);
		record_step(ctx->data, STEP_SIGN);
	}
	a->result = a->result / b->result;
	detach_operand(ctx->op2);
	free_operand(ctx->op2);
	record_step(ctx->data, STEP_DIV_NUMS);
}

int	operate_numbers(solving_data *data, num_expr *node, operand *op1, operand *op2)
{
	operation_context ctx;

	#ifdef DEBUG
		print_debug("Operating numbers\n");
	#endif

	ctx.data = data;
	ctx.node = node;
	ctx.op1 = op1;
	ctx.op2 = op2;
	data->record = 1;

	if (op2->operation == OP_SUM)
		sum_int(&ctx);
	else if (op2->operation == OP_SUBS)
		subs_int(&ctx);
	else if (op2->operation == OP_PROD)
		prod_int(&ctx);
	else if (op2->operation == OP_DIV)
		div_int(&ctx);
	else
		return (0);

	if (!node->operands->next)
		pull_first_operand(node);
	else if (data->record)
		solve_node(data, node);
	return (1);
}
