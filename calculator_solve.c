#include "calculator.h"

void interactive_sum(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	integer	mult;

	if (left[1] == 1 && right[1] == 1)
	{
		node->result = Q_sum(left, right);
		node->op = 0;
		print_tree(tree, NULL);
		return;
	}
	mult = lcm(left[1], right[1]);
	left[0] *= mult / left[1];
	right[0] *= mult / right[1];
	left[1] = mult;
	right[1] = mult;
	print_tree(tree, NULL);
	node->result = fraction(left[0] + right[0], mult);
	node->op = 0;
	print_tree(tree, NULL);
	simplify(node->result);
	print_tree(tree, NULL);
}

void interactive_subs(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	integer	mult;

	if (left[1] == 1 && right[1] == 1)
	{
		node->result = Q_subs(left, right);
		node->op = 0;
		print_tree(tree, NULL);
		return;
	}
	mult = lcm(left[1], right[1]);
	left[0] *= mult / left[1];
	right[0] *= mult / right[1];
	left[1] = mult;
	right[1] = mult;
	print_tree(tree, NULL);
	node->result = fraction(left[0] - right[0], mult);
	node->op = 0;
	print_tree(tree, NULL);
	simplify(node->result);
	print_tree(tree, NULL);
}

void interactive_prod(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	if (left[1] == 1 || right[1] == 1)
	{
		node->op = 0;
		node->result = Q_prod(left, right);
		print_tree(tree, NULL);
		return;
	}
	node->op = 0;
	node->result = fraction(left[0]*right[0],left[1]*right[1]);
	print_tree(tree, NULL);
	simplify(node->result);
	print_tree(tree, NULL);
}

void interactive_div(t_bin_operation *tree, t_bin_operation *node, rational left, rational right)
{
	integer	tmp;

	if (right[1] == 1)
	{
		node->op = 0;
		node->result = Q_div(left, right);
		if (left[0] != node->result[0])
			print_tree(tree, NULL);
		return;
	}
	node->op = '*';
	tmp = right[0];
	right[0] = right[1];
	right[1] = tmp;
	print_tree(tree, NULL);
	interactive_prod(tree, node, left, right);
}

rational solve_node_interactive(t_bin_operation *tree, t_bin_operation *node)
{
	rational left, right;

	if (node->left)
		left = solve_node_interactive(tree, node->left);
	if (node->right)
		right = solve_node_interactive(tree, node->right);
	// if (node->op == '+' || node->op == '-' || node->op == '*' || node->op == '/')
	// {
	// 	print_rational(left);
	// 	printf("%c", node->op);
	// 	print_rational(right);
	// 	putchar('\n');
	// }
	if (node->op == '(')
	{
		node->result = left;
		node->op = 0;
		// print_tree(tree, NULL);
	}
	else if (node->op == '+')
		interactive_sum(tree, node, left, right);
	else if (node->op == '-')
		interactive_subs(tree, node, left, right);
	else if (node->op == '*')
		interactive_prod(tree, node, left, right);
	else if (node->op == '/')
		interactive_div(tree, node, left, right);
	// if (node->op)
	// {
	// 	node->op = 0;
	// 	print_tree(tree, NULL);
	// }
	return (node->result);
}

void	solve_tree_by_steps(t_bin_operation *tree)
{
	solve_node_interactive(tree, tree);
	printf("The result is ");
	print_rational(tree->result);
	putchar('\n');
}
