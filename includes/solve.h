#ifndef SOLVE_H
#define SOLVE_H

#include "tree.h"

//		Basic functions
integer gcd(integer a, integer b);
integer lcm(integer a, integer b);
integer	int_sqrt(integer n);

//		Solve
void	solve_node_didactic(num_expr *expr, num_expr *node);

//	Operate Numbers
int	operate_numbers(num_expr *expr, operand *op1, operand *op2);

//	Properties
int		apply_properties(num_expr *expr, operand *op1, operand *op2);
operand	*apply_associative(num_expr *expr, operand *op);

//			Utils
void		insert_operands(operand *op, int operation, operand *operands);
operand		*expand_operand(operand *op);
int			pull_first_operand(num_expr *node);
num_expr	*dup_expr(num_expr *original);
operand		*dup_operand(operand *original);
int			expr_cmp(num_expr *expr1, num_expr *expr2);
int			expr_is_num(num_expr *expr, integer n);
operand		*replace_operand(operand *copy, operand *original);
void		shift_left(operand *op);
void		detach_operand(operand *op);
operand		*simplify_identity(num_expr *expr, operand *op);
operand		*delete_operand(operand *op);

#endif
