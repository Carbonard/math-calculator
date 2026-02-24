#include "calculator.h"

int find_op(char *input)
{
	int	par_cnt;
	int	i;

	i = strlen(input);
	while (i)
	{
		i--;
		if ((input[i] == '+' || input[i] == '-') && (i != 0 && input[i-1] != '+' && input[i-1] != '-' && input[i-1] != '*' && input[i-1] != '/'))
			return (i);
		else if (input[i] == ')')
		{
			par_cnt = -1;
			while (par_cnt < 0)
			{
				i--;
				if (input[i] == '(')
					par_cnt++;
				else if (input[i] == ')')
					par_cnt--;
			}
		}
	}
	i = strlen(input);
	while (i)
	{
		i--;
		if (input[i] == '*' || input[i] == '/')
			return (i);
		else if (input[i] == ')')
		{
			par_cnt = -1;
			while (par_cnt < 0)
			{
				i--;
				if (input[i] == '(')
					par_cnt++;
				else if (input[i] == ')')
					par_cnt--;
			}
		}
	}
	return (0);
}

t_bin_operation *generate_tree(char *input)
{
	t_bin_operation	*node = malloc(sizeof(t_bin_operation));
	int				i = find_op(input);

	node->left = NULL;
	node->right = NULL;
	if (i)
	{
		node->op = input[i];
		input[i] = 0;
		node->left = generate_tree(input);
		node->right = generate_tree(input + i + 1);
		return (node);
	}
	else if (input[0] == '(')
	{
		node->op = '(';
		input[strlen(input) - 1] = 0;
		node->left = generate_tree(input + 1);
		return (node);
	}
	node->op = 0;
	node->result = itor(atoi(input));
	return (node);
}

void print_node(t_bin_operation *node)
{
	if (node->op == '(')
	{
		if (node->left->op && node->left->op != '(')
			putchar('(');
		print_node(node->left);
		if (node->left->op && node->left->op != '(')
			putchar(')');
	}
	else if (node->op == 0)
	{
		if (node->result[1] != 1)
			printf("{");
		print_rational(node->result);
		if (node->result[1] != 1)
			putchar('}');
	}
	else
	{
		print_node(node->left);
		// if (node->op == '/')
		// 	putchar(':');
		// else
			putchar(node->op);
		print_node(node->right);
	}
}

void print_tree(t_bin_operation *tree, char *comment)
{
	if (comment)
		printf("%s", comment);
	print_node(tree);
	putchar('\n');
}
