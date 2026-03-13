#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_json_item json_item;
typedef struct s_json_obj json_obj;

enum json_values
{
	JS_NULL,
	JS_INT,
	JS_STR,
	JS_ARR,
	JS_OBJ
};

typedef struct s_json_item
{
	int		type;
	char	*key;
	union
	{
	char		*str_value;
	int			int_value;
	json_obj	*array_value;
	json_obj	*object_value;
	};
}	json_item;


typedef struct s_json_obj
{
	int			size;
	int			length;
	json_item	*data;
}	json_obj;

json_obj	*new_json(int size);
void		add_str_to_json(json_obj *json, char *key, char *value);
void		add_int_to_json(json_obj *json, char *key, int value);
void		add_array_to_json(json_obj *json, char *key, json_obj *array);
void		add_json_to_json(json_obj *json, char *key, json_obj *object);
void		add_null_to_json(json_obj *json, char *key);
void		delete_json(json_obj *json);
int			fprint_json(FILE *file, json_obj *json);
char		*json_to_str(json_obj *json);
void		 print_json_str(char *json);
// int		sprint_json(json_obj *json, char *buffer);

#endif