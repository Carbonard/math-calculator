#include "../includes/solve.h"
#include "../includes/json.h"

static char syntax_error_str[SE_TOTAL][50] = {
	[SE_CLOS_PAR] = "SE_CLOS_PAR",
	[SE_OPEN_PAR] = "SE_OPEN_PAR",
	[SE_EMPTY_OP] = "SE_EMPTY_OP",
	[SE_MISSING_OP] = "SE_MISSING_OP",
	[SE_SPACE] = "SE_SPACE",
	[SE_UNDEFINED] = "SE_UNDEFINED"
};

json_obj *json_syntax_error(int error, char *input, int index)
{
	json_obj *json;

	if (error == SE_SUCCESS)
	{
		json = new_json(1);
		add_str_to_json(json, "status", "ok");
		return (json);
	}
	json = new_json(5);
	add_str_to_json(json, "status", "syntax_error");
	add_str_to_json(json, "code", syntax_error_str[error]);
	add_str_to_json(json, "input", input);
	add_int_to_json(json, "index", index);
	return (json);
}

const char *step_str[] = {
#define X(a,b) b,
STEP_LIST
#undef X
};

json_obj *steps_to_json_list(step *steps)
{
	json_obj	*list;
	json_obj	*obj_item;

	list = malloc(sizeof(json_obj));
	list->length = 0;
	list->size = 1;
	list->data = malloc(sizeof(json_item) * list->size);
	while(steps->step_code >= 0)
	{
		obj_item = malloc(sizeof(json_obj));
		
		obj_item->length = 2;
		obj_item->size = 2;
		obj_item->data = malloc(2*sizeof(json_item));
		
		obj_item->data[0].key = strdup("latex");
		obj_item->data[0].type = JS_STR;
		obj_item->data[0].str_value = steps->latex;
		
		obj_item->data[1].key = strdup("code");
		obj_item->data[1].type = JS_STR;
		obj_item->data[1].str_value = strdup(step_str[steps->step_code]);

		if (list->length >= list->size)
		{
			while (list->length >= list->size)
				list->size *= 2;
			list->data = realloc(list->data, list->size * sizeof(json_item));
		}
		list->data[list->length].type = JS_OBJ;
		list->data[list->length].key = NULL;
		list->data[list->length].object_value = obj_item;
		list->length++;
		steps++;
	}
	return (list);
}

json_obj *steps_json(step *steps)
{
	json_obj *json = new_json(2);

	add_str_to_json(json, "status", "ok");
	add_array_to_json(json, "steps", steps_to_json_list(steps));
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