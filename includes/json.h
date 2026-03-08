#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum json_values
{
	JV_INT,
	JV_STR,
	JV_NULL,
	// JS_ARR,
	// JS_OBJ
};

typedef struct s_json_item
{
	int		type;
	char	*key;
	union
	{
	char	*str_value;
	int		int_value;
	};
}	json_item;


typedef struct s_json_obj
{
	int			size;
	int			length;
	json_item	*data;
}	json_obj;

void	init_json(json_obj *json, int size);
void	add_str_to_json(json_obj *json, char *key, char *value);
void	add_int_to_json(json_obj *json, char *key, int value);
void	add_null_to_json(json_obj *json, char *key);
void	delete_json(json_obj *json);
int		fprint_json(FILE *file, json_obj *json);
int		sprint_json(json_obj *json, char *buffer);

#endif