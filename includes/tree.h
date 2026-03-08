#ifndef TREE_H
#define TREE_H

#include "calculator.h"

//			Tree utils
num_expr	*create_num_expr(int type, int subtype);
num_expr	*create_num_num_expr(int sign, int result);
operand		*create_operand(int operation, num_expr *expr);
void		append_operand(num_expr *node, operand *new_operand);
void		operand_to_bin_op(operand *left, int operation, operand *right);
operand		*last_operand(num_expr *expr);
void		free_num_expr(num_expr *expr);
void		free_operand(operand *op);

//			Print tree
void		print_num_expr(num_expr *expr);

#endif