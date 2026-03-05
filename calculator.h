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
	SE_CLOS_PAR,
	SE_OPEN_PAR,
	SE_EMPTY_OP,
	SE_MISSING_OP,
	SE_UNDEFINED,
	SE_SPACE,
	SE_TOTAL
};

// typedef struct s_bin_operation
// {
// 	struct s_bin_operation	*left;
// 	struct s_bin_operation	*right;
// 	char					op;
// 	rational				result;
// }	t_bin_operation;

enum e_alg_type
{
	ALG_END,
	ALG_SIGN,// +, -
	ALG_NUMBER,
	ALG_BINARY_OP,// +, -, *, /, ^
	ALG_PARENTHESIS,// (, )
	ALG_SINGLE_OP// sqrt, root, exp, abs, ln, log
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
	OP_NONE,
	OP_SUM,
	OP_SUBS,
	OP_PROD,
	OP_DIV,
	OP_POW
};

enum e_single_operators
{
	OP_SNONE,
	OP_SQRT,
	OP_ROOT,
	OP_LN,
	OP_LOG,
	OP_EXP,
	OP_ABS,
	OP_ERR
};

// enum e_numbers
// {
// 	N_INTEGER,
// 	N_PI,
// 	N_E
// };

// typedef rational (*op_func)(rational a, rational b);

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

enum e_numbers
{
	N_INTEGER,
	N_RATIONAL,
	N_ROOT,
	N_LOG
};

// typedef struct s_number
// {
// 	int 	type;
// 	char	sgn;
// 	union
// 	{
// 		int	integer;
// 		int	rational[2];
// 		int	root[2];
// 		int	log[2];
// 	};
// }	number;

typedef struct s_operand operand;

typedef struct s_num_expr
{
	char	sign;
	int		type;
	int		subtype;
	operand	*operands;
	integer	result;
	int		solved;
}	num_expr;

struct s_operand
{
	int			operation;
	num_expr	*operand;
	operand		*next;
	operand		*prev;
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
num_expr	*create_num_expr(int type, int subtype);
operand		*create_operand(int operation, num_expr *expr);
void		append_operand(num_expr *node, operand *new_operand);
num_expr	*generate_num_expr(alg_token_t *tokens);
num_expr	*create_num_num_expr(int sign, int result);
void		print_num_expr(num_expr *expr);
void		free_num_expr(num_expr *expr);
void		free_operand(operand *op);
operand		*del_and_back(operand *op);
void		operand_to_bin_op(operand *left, int operation, operand *right);
// Solve
// void	solve_tree_by_steps(t_bin_operation *tree);
void	solve_by_steps(num_expr *expr);
// Utils
void	remove_spaces(char *str);

#endif