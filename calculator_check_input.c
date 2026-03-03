#include "calculator.h"

enum events
{
	E_SPACE,
	E_NUMBER,
	E_OPEN_PAR,
	E_CLOS_PAR,
	E_SIGN,
	E_OPERATOR,
	E_SING_OP,
	E_OTHER,
	E_TOTAL
};

enum states
{
	S_INITIAL,
	S_SIGN,
	S_NUMBER,
	S_OPERATOR,
	S_OPEN_PAR,
	S_CLOS_PAR,
	S_ERROR,
	S_TOTAL
};

#ifdef DEBUG
static char states[S_TOTAL][20] = {
	[S_INITIAL] = "S_INITIAL",
	[S_SIGN] = "S_SIGN",
	[S_NUMBER] = "S_NUMBER",
	[S_OPERATOR] = "S_OPERATOR",
	[S_OPEN_PAR] = "S_OPEN_PAR",
	[S_CLOS_PAR] = "S_CLOS_PAR",
	[S_ERROR] = "S_ERROR"
};

static char events[E_TOTAL][20] = {
	[E_SPACE] = {"E_SPACE"},
	[E_NUMBER] = {"E_NUMBER"},
	[E_OPEN_PAR] = {"E_OPEN_PAR"},
	[E_CLOS_PAR] = {"E_CLOS_PAR"},
	[E_SIGN] = {"E_SIGN"},
	[E_OPERATOR] = {"E_OPERATOR"},
	[E_SING_OP] = {"E_SING_OP"},
	[E_OTHER] = {"E_OTHER"}
};

static char errors[SE_TOTAL][50] = {
	[SE_SUCCESS] = "SE_SUCCESS",
	[SE_EXCEDING_PARENTHESIS] = "SE_EXCEDING_PARENTHESIS",
	[SE_MISSING_PARENTHESIS] = "SE_MISSING_PARENTHESIS",
	[SE_EMPTY_OPERATOR] = "SE_EMPTY_OPERATOR",
	[SE_MISSING_OPERATOR] = "SE_MISSING_OPERATOR",
	[SE_UNDEFINED] = "SE_UNDEFINED",
};
#endif

static const int state_conversor[S_TOTAL][E_TOTAL] = {
//					 E_SPACE,		E_NUMBER,	E_OPEN_PAR,	E_CLOS_PAR,	E_SIGN,		E_OPERATOR,	E_SING_OP,	E_OTHER
	[S_INITIAL]		= {S_INITIAL,	S_NUMBER,	S_OPEN_PAR,	S_CLOS_PAR,	S_SIGN,		S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_NUMBER]		= {S_NUMBER,	S_ERROR,	S_ERROR,	S_CLOS_PAR,	S_OPERATOR,	S_OPERATOR,	S_ERROR,	S_ERROR},
	[S_OPERATOR]	= {S_OPERATOR,	S_NUMBER,	S_OPEN_PAR,	S_ERROR,	S_SIGN,		S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_OPEN_PAR]	= {S_OPEN_PAR,	S_NUMBER,	S_OPEN_PAR,	S_CLOS_PAR,	S_SIGN,		S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_CLOS_PAR]	= {S_CLOS_PAR,	S_ERROR,	S_ERROR,	S_CLOS_PAR,	S_OPERATOR,	S_OPERATOR,	S_ERROR,	S_ERROR},
	[S_SIGN]		= {S_SIGN,		S_NUMBER,	S_OPEN_PAR,	S_ERROR,	S_ERROR,	S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_ERROR]		= {S_ERROR,		S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR}
};

static const int get_error[S_TOTAL][E_TOTAL] = {
//					 E_SPACE,	E_NUMBER,			E_OPEN_PAR,			E_CLOS_PAR,				E_SIGN,				E_OPERATOR,			E_SING_OP,			E_OTHER
	[S_INITIAL]	= {SE_SUCCESS, SE_SUCCESS,			SE_SUCCESS,			SE_EXCEDING_PARENTHESIS,SE_SUCCESS,			SE_EMPTY_OPERATOR,	SE_SUCCESS,			SE_UNDEFINED},
	[S_NUMBER]	= {SE_SUCCESS, SE_SPACE,		SE_MISSING_OPERATOR,SE_SUCCESS,				SE_SUCCESS,			SE_SUCCESS,			SE_MISSING_OPERATOR,SE_UNDEFINED},
	[S_OPERATOR]= {SE_SUCCESS, SE_SUCCESS,			SE_SUCCESS,			SE_EMPTY_OPERATOR,		SE_SUCCESS,			SE_EMPTY_OPERATOR,	SE_SUCCESS,			SE_UNDEFINED},
	[S_OPEN_PAR]= {SE_SUCCESS, SE_SUCCESS,			SE_SUCCESS,			SE_SUCCESS,				SE_SUCCESS,			SE_EMPTY_OPERATOR,	SE_SUCCESS,			SE_UNDEFINED},
	[S_CLOS_PAR]= {SE_SUCCESS, SE_MISSING_OPERATOR, SE_MISSING_OPERATOR,SE_SUCCESS,				SE_SUCCESS,			SE_SUCCESS,			SE_MISSING_OPERATOR,SE_UNDEFINED},
	[S_SIGN]	= {SE_SUCCESS, SE_SUCCESS,			SE_SUCCESS,			SE_EMPTY_OPERATOR,		SE_EMPTY_OPERATOR,	SE_EMPTY_OPERATOR,	SE_SUCCESS,			S_ERROR},
	// [S_ERROR] =		{, , , , , , }
};

static int get_event(char c)
{
	if (isdigit(c))
		return E_NUMBER;
	else if (c == '*' || c == '/')
		return E_OPERATOR;
	else if (c == '+'|| c == '-')
		return E_SIGN;
	else if (c == '(')
		return E_OPEN_PAR;
	else if (c == ')')
		return E_CLOS_PAR;
	else if (c == ' ')
		return E_SPACE;
	else
		return E_OTHER;
}

int get_single_op(char *input)
{
	if (!strncmp(input, "sqrt", 4))
		return (OP_SQRT);
	if (!strncmp(input, "root", 4))
		return (OP_ROOT);
	if (!strncmp(input, "ln", 2))
		return (OP_LN);
	if (!strncmp(input, "log", 3))
		return (OP_LOG);
	if (!strncmp(input, "exp", 3))
		return (OP_EXP);
	if (!strncmp(input, "abs", 3))
		return (OP_ABS);
	return (OP_ERR);
}

int sing_op_length[] = {
	[OP_SQRT]	= 4,
	[OP_ROOT]	= 4,
	[OP_LN]		= 2,
	[OP_LOG]	= 3,
	[OP_EXP]	= 3,
	[OP_ABS]	= 3,
};

static void add_token(alg_token_array_t *tokens, int type, int sub_type, int num)
{
	if (tokens->length >= tokens->size)
	{
		tokens->array = realloc(tokens->array, tokens->size * 2 * sizeof(alg_token_t));
		tokens->size *= 2;
	}
	#ifdef DEBUG
		printf("Add token %d, %d, %d in %d\n", type, sub_type, num, tokens->length);
	#endif
	tokens->array[tokens->length].type = type;
	tokens->array[tokens->length].sub_type = sub_type;
	tokens->array[tokens->length].number = num;
	tokens->length++;
}

static int get_token(alg_token_array_t *tokens, int event, int state, char *input, int index, int single_op)
{
	if (event == E_SING_OP) 
		add_token(tokens, ALG_SINGLE_OP, single_op, 0);
	else if (state == S_SIGN)
		add_token(tokens, ALG_SIGN, (input[index] == '-'), 0);
	else if ((state == S_NUMBER && (tokens->length == 0 || tokens->array[tokens->length - 1].type != ALG_NUMBER)) || state == S_SIGN)
		add_token(tokens, ALG_NUMBER, N_INTEGER, atoi(input + index));
	else if (state == S_OPERATOR)
		add_token(tokens, ALG_BINARY_OP, (input[index] == '+') * OP_SUM + (input[index] == '-') * OP_SUBS + (input[index] == '*') * OP_PROD + (input[index] == '/') * OP_DIV, 0);
	else if (state == S_CLOS_PAR || state == S_OPEN_PAR)
		add_token(tokens, ALG_PARENTHESIS, (input[index] == ')'), 0);
	else if (state != S_NUMBER)
	{
		printf("Please, report issue (token not found)\n");
		return (1);
	}
	return (0);
}

alg_token_t	*check_syntax(char *input, json_obj **json)
{
	int					event, state = S_INITIAL, error = SE_SUCCESS;
	int					parenthesis = 0, sing_op = 0;
	int					i = 0;
	alg_token_array_t	tokens;

	tokens.array = malloc(1 * sizeof(alg_token_t));
	tokens.size = 1;
	tokens.length = 0;
	while (input[i] && state != S_ERROR)
	{
		#ifdef DEBUG
			printf("-----------\n");
		#endif
		event = get_event(input[i]);
		// Manage single operators
		if (event == E_OTHER)
		{
			sing_op = get_single_op(input + i);
			if (sing_op != OP_ERR)
				event = E_SING_OP;
		}
		// Manage parenthesis
		if (event == E_OPEN_PAR)
			parenthesis++;
		else if (event == E_CLOS_PAR)
			parenthesis--;
		if (parenthesis < 0)
		{
			state = S_ERROR;
			error = SE_EXCEDING_PARENTHESIS;
			i++;
			break;
		}
		error = get_error[state][event];
		state = state_conversor[state][event];
		#ifdef DEBUG
			printf("%c: event=%s, state=%s, error=%s\n", input[i], events[event], states[state], errors[error]);
		#endif
		if (event != E_SPACE && state != S_ERROR)
			get_token(&tokens, event, state, input, i, sing_op);
		if (event == E_NUMBER && error == SE_SUCCESS)
		{
			while (input[i] && get_event(input[i]) == E_NUMBER)
				i++;
			while (input[i] && get_event(input[i]) == E_SPACE)
				i++;
		}
		else if (event == E_SING_OP)
			i += sing_op_length[sing_op];
		else if (error != SE_SPACE)
			i++;
	}
	i--;
	if (error == SE_SUCCESS && (state == S_OPERATOR || state == S_SIGN))
		error = SE_EMPTY_OPERATOR;
	else if (parenthesis && error == SE_SUCCESS)
		error = SE_MISSING_PARENTHESIS;
	add_token(&tokens, ALG_END, 0, 0);
	*json = json_syntax_error(error, input, i);
	if (error != SE_SUCCESS)
	{
		#ifdef DEBUG
			printf("ending parsing with error %d\n", error);
		#endif
		free(tokens.array);
		return (NULL);
	}
	return (tokens.array);
}
