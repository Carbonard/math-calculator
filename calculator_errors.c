#include "calculator.h"

static char error_str[SE_TOTAL][50] = {
	[SE_CLOS_PAR] = "SE_CLOS_PAR",
	[SE_OPEN_PAR] = "SE_OPEN_PAR",
	[SE_EMPTY_OP] = "SE_EMPTY_OP",
	[SE_MISSING_OP] = "SE_MISSING_OP",
	[SE_SPACE] = "SE_SPACE",
	[SE_UNDEFINED] = "SE_UNDEFINED"
};

json_obj *json_syntax_error(int error, char *input, int index)
{
	json_obj *json = malloc(sizeof(json_obj));

	if (error == SE_SUCCESS)
	{
		init_json(json, 1);
		add_str_to_json(json, "status", "ok");
		return (json);
	}
	init_json(json, 5);
	add_str_to_json(json, "status", "syntax_error");
	add_str_to_json(json, "code", error_str[error]);
	add_str_to_json(json, "input", input);
	add_int_to_json(json, "index", index);
	return (json);
}

// void print_syntax_error(char *input, int error, int index)
// {
// 	fprintf(stderr, "Syntax error: %s\n", error_msg[error]);
// 	for (int i = 0; input[i]; i++)
// 	{
// 		if (i == index)
// 			fprintf(stderr, "\e[4m");
// 		fputc(input[i], stderr);
// 		if (i == index)
// 			fprintf(stderr, "\e[24m");
// 	}
// 	fputc('\n', stderr);
// }