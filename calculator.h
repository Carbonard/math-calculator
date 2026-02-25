#ifndef CALCULATOR_H
#define CALCULATOR_H

// #define WEB

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "rational_numbers.h"
#include "json.h"


#define PI_NUM 3.14159265359;
#define E_NUM  2.71828182845;
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

enum e_type
{
	ALG_NUMBER,
	ALG_BINARY_OP,
	ALG_PARENTHESIS,
	ALG_SINGLE_OP,
	ALG_END
};

// #ifdef DEBUG
// char algtypes[10][30] = {
// 	"ALG_NUMBER",
// 	"ALG_BINARY_OP",
// 	"ALG_PARENTHESIS",
// 	"ALG_SINGLE_OP",
// 	"ALG_END"};
// #endif

enum e_binary_operators
{
	OP_SUM,
	OP_SUBS,
	OP_PROD,
	OP_DIV,
	OP_POW
};

enum e_single_operators
{
	OP_SQRT,
	OP_ROOT,
	OP_LN,
	OP_LOG,
	OP_EXP,
	OP_ABS,
	OP_ERR
};

enum e_numbers
{
	E_GENERIC,
	E_PI,
	E_E
};

typedef rational (*op_func)(rational a, rational b);

typedef struct alg_token_s
{
	int	type;
	int	sub_type;
	int	number;
}	alg_token_t;

typedef struct alg_token_array_s
{
	alg_token_t	*array;
	int			size;
	int			length;
}	alg_token_array_t;




typedef struct s_operand operand;

typedef struct s_num_expr
{
	int		type;
	int		result;
	operand	*operands;
	int		resolved;
}	num_expr;

struct s_operand
{
	int			operation;
	num_expr	*operand;
};

// Input
alg_token_t	*check_syntax(char *input, json_obj **json);
char		*manage_input(char *input);
// Input errors
void	 print_syntax_error(char *input, int error, int index);
json_obj *json_syntax_error(int error, char *input, int index);
void	 print_tokens(alg_token_t *tokens);
// Tree
// t_bin_operation *generate_tree(char *input);
// void	print_tree(t_bin_operation *tree, char *comment);
num_expr	*generate_num_expr(alg_token_t *tokens);
void	print_num_expr(num_expr *expr);
// Solve
void	solve_tree_by_steps(t_bin_operation *tree);
// Utils
void	remove_spaces(char *str);

#endif