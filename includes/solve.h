#ifndef SOLVE_H
#define SOLVE_H

#include "tree.h"
#include "prints.h"

#define STEP_LIST \
	X(STEP_INITIAL, "STEP_INITIAL") \
	X(STEP_SIGN, "STEP_SIGN") \
	X(STEP_ASSOCIATIVE, "STEP_ASSOCIATIVE") \
	X(STEP_SIMPLIFY_FRAC, "STEP_SIMPLIFY_FRAC") \
	X(STEP_MULT_FRAC_NOFRAC, "STEP_MULT_FRAC_NOFRAC") \
	X(STEP_MULT_FRACS, "STEP_MULT_FRACS") \
	X(STEP_SUBS_TO_SUM, "STEP_SUBS_TO_SUM") \
	X(STEP_ABS, "STEP_ABS") \
	X(STEP_SQRT, "STEP_SQRT") \
	X(STEP_SORT_OP, "STEP_SORT_OP") \
	X(STEP_AMPLIFY_FRAC, "STEP_AMPLIFY_FRAC") \
	X(STEP_AMPLIFY_FRACS, "STEP_AMPLIFY_FRACS") \
	X(STEP_SUM_FRACS, "STEP_SUM_FRACS") \
	X(STEP_DIV_FRACS, "STEP_DIV_FRACS") \
	X(STEP_DIV_FRAC_BY_NUM, "STEP_DIV_FRAC_BY_NUM") \
	X(STEP_DIV_NUM_BY_FRAC, "STEP_DIV_NUM_BY_FRAC") \
	X(STEP_SUM_ZERO, "STEP_SUM_ZERO") \
	X(STEP_MULT_ONE, "STEP_MULT_ONE") \
	X(STEP_MULT_ZERO, "STEP_MULT_ZERO") \
	X(STEP_MULT_SQRT, "STEP_MULT_SQRT") \
	X(STEP_SUM_NUMS, "STEP_SUM_NUMS") \
	X(STEP_SUBS_NUMS, "STEP_SUBS_NUMS") \
	X(STEP_MULT_NUMS, "STEP_MULT_NUMS") \
	X(STEP_DIV_NUMS, "STEP_DIV_NUMS")

typedef enum {
#define X(a,b) a,
STEP_LIST
#undef X
} step_code;

typedef struct
{
	int		step_code;
	char	*latex;
} step;

typedef struct
{
	step	*array;
	size_t	length;
	size_t	capacity;
} step_list;

typedef struct
{
	const num_expr	*full_expr;
	step_list		*steps;
	int				record;
} solving_data;

typedef struct
{
	solving_data	*data;
	num_expr		*node;
	operand			*op1;
	operand			*op2;
} operation_context;

//		Basic functions
integer gcd(integer a, integer b);
integer lcm(integer a, integer b);
integer	int_sqrt(integer n);

//		Solve
void	solve_node(solving_data *data, num_expr *node);
step	*solve_by_steps(num_expr *expr);

//	Operate Numbers
int	operate_numbers(solving_data *data, num_expr *node, operand *op1, operand *op2);

//	Properties
int		apply_properties_1(solving_data *data, num_expr *node, operand *op1, operand *op2);
int		apply_properties_2(solving_data *data, num_expr *node, operand *op1, operand *op2);
operand	*apply_associative(solving_data *data, operand *op);

//			Utils
void		insert_operands(operand *op, int operation, operand *operands);
operand		*expand_operand(operand *op);
num_expr	*dup_expr(num_expr *original);
operand		*dup_operand(operand *original);
int			expr_cmp(num_expr *expr1, num_expr *expr2);
int			expr_is_num(num_expr *expr, integer n);
operand		*replace_operand(operand *copy, operand *original);
void		shift_left(operand *op);
void		detach_operand(operand *op);
operand		*simplify_identity(solving_data *data, operand *op);
void		swap_operands(operand *op1, operand *op2);
operand		*delete_operand(operand *op);

// Solve fractions
void	simplify_frac(solving_data *data, num_expr *frac);
void	amplify_frac(num_expr *frac, operand *factor);
int		fraction_product(operation_context *ctx);
int		fraction_division(operation_context *ctx);
int		fraction_sum(operation_context *ctx);

void	 record_step(solving_data *data, int step_code);
json_obj *steps_to_json_list(step *steps);
json_obj *steps_json(step *steps);

#endif
