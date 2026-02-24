#include "json.h"

void	init_json(json_obj *json, int size)
{
	json->data = malloc(size * sizeof(json_item));
	json->length = 0;
	json->size = size;
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
	json->data[json->length].type = JV_STR;
	json->data[json->length].key = strdup(key);
	json->data[json->length].str_value = strdup(value);
	json->length++;
}

void	add_int_to_json(json_obj *json, char *key, int value)
{
	grant_size(json);
	json->data[json->length].type = JV_INT;
	json->data[json->length].key = strdup(key);
	json->data[json->length].int_value = value;
	json->length++;
}

void	add_null_to_json(json_obj *json, char *key)
{
	grant_size(json);
	json->data[json->length].type = JV_NULL;
	json->data[json->length].key = strdup(key);
	json->length++;
}

void	delete_json(json_obj *json)
{
	for (int i = 0; i < json->length; i++)
	{
		free(json->data[i].key);
		json->data[i].key = NULL;
		if (json->data[i].type == JV_STR)
		{
			free(json->data[i].str_value);
			json->data[i].str_value = NULL;
		}
	}
	free(json->data);
	json->size = 0;
	json->length = 0;
	json->data = NULL;
}

int	fprint_json(FILE *file, json_obj *json)
{
	char	*output = malloc(2000);
	size_t	length = 0;

	output[0] = '{';
	length++;
	for (int i = 0; i < json->length; i++)
	{
		length += sprintf(output + length, "\"%s\":", json->data[i].key);
		if (json->data[i].type == JV_INT)
			length += sprintf(output + length, "%d", json->data[i].int_value);
		if (json->data[i].type == JV_STR)
			length += sprintf(output + length, "\"%s\"", json->data[i].str_value);
		if (json->data[i].type == JV_NULL)
			length += sprintf(output + length, "%s", "null");
		if (i+1 < json->length)
		{
			strcat(output, ",");
			length++;
		}
	}
	strcat(output, "}");
	length++;
	fprintf(file , "%s\n", output);
	return (length);
}

int	sprint_json(json_obj *json, char *buffer)
{
	size_t	length = 0;

	buffer[0] = '{';
	length++;
	for (int i = 0; i < json->length; i++)
	{
		length += sprintf(buffer + length, "\"%s\":", json->data[i].key);
		if (json->data[i].type == JV_INT)
			length += sprintf(buffer + length, "%d", json->data[i].int_value);
		if (json->data[i].type == JV_STR)
			length += sprintf(buffer + length, "\"%s\"", json->data[i].str_value);
		if (json->data[i].type == JV_NULL)
			length += sprintf(buffer + length, "%s", "null");
		if (i+1 < json->length)
		{
			strcat(buffer, ",");
			length++;
		}
	}
	strcat(buffer, "}");
	length++;
	return (length);
}
