#include "calculator.h"


char *manage_input(char *input)
{
	t_bin_operation	*op_tree;
	int				syntax_status;
	json_obj		*json;
	char			json_str[100];

	if (!input || !input[0])
		return (NULL);
	json = check_syntax(input);
	if (!strcmp(json->data[0].str_value, "ok"))
	{
		remove_spaces(input);
		op_tree = generate_tree(input);
		print_tree(op_tree, NULL);
		solve_tree_by_steps(op_tree);
	}
	sprint_json(json, json_str);
	return (strdup(json_str));
}