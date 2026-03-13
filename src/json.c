#include "../includes/json.h"
#include "../includes/prints.h"

json_obj	*new_json(int size)
{
	json_obj *json = malloc (size * sizeof(json_obj));
	json->data = malloc(size * sizeof(json_item));
	json->length = 0;
	json->size = size;
	return (json);
}

static void	grant_size(json_obj *json)
{
	if (json->length >= json->size)
	{
		json->data = realloc(json->data, json->size*2*sizeof(json_item));
		json->size *= 2;
	}
}

void	add_str_to_json(json_obj *json, char *key, char *value)
{
	grant_size(json);
	json->data[json->length].type = JS_STR;
	json->data[json->length].key = strdup(key);
	json->data[json->length].str_value = strdup(value);
	json->length++;
}

void	add_int_to_json(json_obj *json, char *key, int value)
{
	grant_size(json);
	json->data[json->length].type = JS_INT;
	json->data[json->length].key = strdup(key);
	json->data[json->length].int_value = value;
	json->length++;
}

void	add_null_to_json(json_obj *json, char *key)
{
	grant_size(json);
	json->data[json->length].type = JS_NULL;
	json->data[json->length].key = strdup(key);
	json->length++;
}

void	add_array_to_json(json_obj *json, char *key, json_obj *array)
{
	grant_size(json);
	json->data[json->length].type = JS_ARR;
	json->data[json->length].key = strdup(key);
	json->data[json->length].array_value = array;
	json->length++;
}

void	add_json_to_json(json_obj *json, char *key, json_obj *object)
{
	grant_size(json);
	json->data[json->length].type = JS_OBJ;
	json->data[json->length].key = strdup(key);
	json->data[json->length].object_value = object;
	json->length++;
}

void	delete_json(json_obj *json)
{
	for (int i = 0; i < json->length; i++)
	{
		free(json->data[i].key);
		json->data[i].key = NULL;
		if (json->data[i].type == JS_STR)
		{
			free(json->data[i].str_value);
			json->data[i].str_value = NULL;
		}
		else if (json->data[i].type == JS_ARR)
		{
			delete_json(json->data[i].array_value);
			json->data[i].str_value = NULL;
		}
		else if (json->data[i].type == JS_OBJ)
		{
			delete_json(json->data[i].object_value);
			json->data[i].str_value = NULL;
		}
	}
	free(json->data);
	json->size = 0;
	json->length = 0;
	json->data = NULL;
	free(json);
}

// int	fprint_json(FILE *file, json_obj *json)
// {
// 	char	*output = malloc(3000);
// 	size_t	length = 0;

// 	output[0] = '{';
// 	length++;
// 	for (int i = 0; i < json->length; i++)
// 	{
// 		length += sprintf(output + length, "\"%s\":", json->data[i].key);
// 		if (json->data[i].type == JS_INT)
// 			length += sprintf(output + length, "%d", json->data[i].int_value);
// 		if (json->data[i].type == JS_STR)
// 			length += sprintf(output + length, "\"%s\"", json->data[i].str_value);
// 		if (json->data[i].type == JS_NULL)
// 			length += sprintf(output + length, "%s", "null");
// 		if (i+1 < json->length)
// 		{
// 			strcat(output, ",");
// 			length++;
// 		}
// 	}
// 	strcat(output, "}");
// 	length++;
// 	fprintf(file , "%s\n", output);
// 	return (length);
// }

// int	sprint_json(json_obj *json, char *buffer)
// {
// 	size_t	length = 0;

// 	buffer[0] = '{';
// 	length++;
// 	for (int i = 0; i < json->length; i++)
// 	{
// 		length += sprintf(buffer + length, "\"%s\":", json->data[i].key);
// 		if (json->data[i].type == JS_INT)
// 			length += sprintf(buffer + length, "%d", json->data[i].int_value);
// 		if (json->data[i].type == JS_STR)
// 			length += sprintf(buffer + length, "\"%s\"", json->data[i].str_value);
// 		if (json->data[i].type == JS_NULL)
// 			length += sprintf(buffer + length, "%s", "null");
// 		if (i+1 < json->length)
// 		{
// 			strcat(buffer, ",");
// 			length++;
// 		}
// 	}
// 	strcat(buffer, "}");
// 	length++;
// 	return (length);
// }

char	*json_to_str(json_obj *json)
{
	dynamic_str	jstr;

	jstr.length = 0;
	jstr.size = 32;
	jstr.str = calloc(32,1);
	jstr.str[0] = '{';
	jstr.length++;
	for (int i = 0; i < json->length; i++)
	{
		if (json->data[i].key)
			dynamic_strcatf(&jstr, "\"%s\":", json->data[i].key);
		if (json->data[i].type == JS_INT)
			dynamic_strcatf(&jstr, "%d", json->data[i].int_value);
		if (json->data[i].type == JS_STR)
			dynamic_strcatf(&jstr, "\"%s\"", json->data[i].str_value);
		if (json->data[i].type == JS_ARR)
		{
			dynamic_strcatf(&jstr, "[");
			char *tmp = json_to_str(json->data[i].array_value);
			tmp[strlen(tmp)-1] = 0;
			dynamic_strcatf(&jstr, tmp+1);
			free(tmp);
			dynamic_strcatf(&jstr, "]");
		}
		if (json->data[i].type == JS_OBJ)
		{
			char *tmp = json_to_str(json->data[i].object_value);
			dynamic_strcatf(&jstr, tmp);
			free(tmp);
		}
		if (json->data[i].type == JS_NULL)
			dynamic_strcatf(&jstr, "%s", "null");
		if (i+1 < json->length)
		{
			dynamic_strcatf(&jstr, ",");
		}
	}
	dynamic_strcatf(&jstr, "}");
	return (jstr.str);
}

void print_json_str(char *json)
{
	static int tabs = 0;

	for (int i = 0; json[i]; i++)
	{
		if (json[i] == ',')
		{
			putchar(',');
			putchar('\n');
			for (int i = 0; i < tabs; i++)
				putchar('\t');
		}
		else if (json[i] == '{' || json[i] == '[')
		{
			putchar(json[i]);
			putchar('\n');
			tabs++;
			for (int i = 0; i < tabs; i++)
				putchar('\t');
		}
		else if (json[i] == '}' || json[i] == ']')
		{
			tabs--;
			putchar('\n');
			for (int i = 0; i < tabs; i++)
				putchar('\t');
			putchar(json[i]);
			// putchar('\n');
			// for (int i = 0; i < tabs; i++)
			// 	putchar('\t');
		}
		else
			putchar(json[i]);
	}
}
