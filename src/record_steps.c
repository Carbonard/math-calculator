#include "../includes/solve.h"

void record_step(solving_data *data, int step_code)
{
	#ifdef DEBUG
		print_debug("recording step...\n");
	#endif
	if (data->steps->length >= data->steps->capacity)
	{
		while (data->steps->length >= data->steps->capacity)
			data->steps->capacity *= 2;
		data->steps->array = realloc(data->steps->array, (data->steps->capacity)*sizeof(step));
	}
	if (step_code < 0)
		data->steps->array[data->steps->length].latex = NULL;
	else
		data->steps->array[data->steps->length].latex = num_expr_to_str(data->full_expr);
	data->steps->array[data->steps->length].step_code = step_code;
	if (step_code >= 0)
		puts(data->steps->array[data->steps->length].latex);
	data->steps->length++;
	#ifdef DEBUG
		print_debug("step recorded...\n");
	#endif
}
