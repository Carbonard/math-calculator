#include "../includes/debug.h"
#include "../includes/tree.h"

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
