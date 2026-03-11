#include "../includes/debug.h"
#include "../includes/tree.h"

char algtypes[10][30] = {
	"ALG_END",
	"ALG_SIGN",
	"ALG_NUMBER",
	"ALG_BINARY_OP",
	"ALG_PARENTHESIS",
	"ALG_SINGLE_OP"
};

char binop[10][30] = {
	"OP_NONE",
	"OP_SUM",
	"OP_SUBS",
	"OP_PROD",
	"OP_DIV",
	"OP_POW"
};

int	indent_spaces = 0;

void printind()
{
	for (int i = 0; i < indent_spaces; i++)
		printf("\t");
}

void print_debug(const char *str)
{
	printind();
	printf("%s", str);
}

void print_operation(operand *op1, operand *op2)
{
	printind();
	printf("Managing bin op with numbers: ");
	print_num_expr(op1->expr);
	char *bin_operands[] = {"","+","-","\\cdot","/","^"};
	printind();
	printf("%s", bin_operands[op2->operation]);
	print_num_expr(op2->expr);
}
