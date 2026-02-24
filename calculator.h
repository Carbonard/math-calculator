#ifndef CALCULATOR_H
#define CALCULATOR_H

// #define WEB

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "rational_numbers.h"
#include "json.h"

enum operators
{
	OP_NONE,
	OP_SUM,
	OP_SUBS,
	OP_PROD,
	OP_DIV,
	OP_PARENTHESIS
};

enum syntax_status
{
	SE_SUCCESS,
	SE_EXCEDING_PARENTHESIS,
	SE_MISSING_PARENTHESIS,
	SE_EMPTY_OPERATOR,
	SE_MISSING_OPERATOR,
	SE_UNDEFINED,
	SE_TOTAL
};

typedef struct s_bin_operation
{
	struct s_bin_operation	*left;
	struct s_bin_operation	*right;
	char					op;
	rational				result;
}	t_bin_operation;

// Input
json_obj	*check_syntax(char *input);
char		*manage_input(char *input);
// Input errors
void	 print_syntax_error(char *input, int error, int index);
json_obj *json_syntax_error(int error, char *input, int index);
// Tree
t_bin_operation *generate_tree(char *input);
void	print_tree(t_bin_operation *tree, char *comment);
// Solve
void	solve_tree_by_steps(t_bin_operation *tree);
// Utils
void	remove_spaces(char *str);

#endif