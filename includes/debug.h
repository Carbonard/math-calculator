#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

extern char algtypes[10][30];
extern char binop[10][30];

typedef struct s_operand operand;

extern int indent_spaces;

void printind(void);
void print_debug(const char *str);
void print_operation(operand *op1, operand *op2);

#endif