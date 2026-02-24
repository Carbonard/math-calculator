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

// static char states[S_TOTAL][20] = {
// 	[S_INITIAL] = "S_INITIAL",
// 	[S_NUMBER] = "S_NUMBER",
// 	[S_OPERATOR] = "S_OPERATOR",
// 	[S_OPEN_PAR] = "S_OPEN_PAR",
// 	[S_CLOS_PAR] = "S_CLOS_PAR",
// 	[S_ERROR] = "S_ERROR"
// };

// empezar con -n
static const int state_conversor[S_TOTAL][E_TOTAL] = {
//					 E_SPACE,		E_NUMBER,	E_OPEN_PAR,	E_CLOS_PAR,	E_SIGN,		E_OPERATOR,	E_SING_OP,	E_OTHER
	[S_INITIAL]		= {S_INITIAL,	S_NUMBER,	S_OPEN_PAR,	S_CLOS_PAR,	S_SIGN,		S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_NUMBER]		= {S_NUMBER,	S_NUMBER,	S_ERROR,	S_CLOS_PAR,	S_OPERATOR,	S_OPERATOR,	S_ERROR,	S_ERROR},
	[S_OPERATOR]	= {S_OPERATOR,	S_NUMBER,	S_OPEN_PAR,	S_ERROR,	S_SIGN,		S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_OPEN_PAR]	= {S_OPEN_PAR,	S_NUMBER,	S_OPEN_PAR,	S_CLOS_PAR,	S_SIGN,		S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_CLOS_PAR]	= {S_CLOS_PAR,	S_ERROR,	S_ERROR,	S_CLOS_PAR,	S_SIGN,		S_OPERATOR,	S_ERROR,	S_ERROR},
	[S_SIGN]		= {S_SIGN,		S_NUMBER,	S_OPEN_PAR,	S_ERROR,	S_ERROR,	S_ERROR,	S_OPERATOR,	S_ERROR},
	[S_ERROR]		= {S_ERROR,		S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR,	S_ERROR}
};

static const int get_error[S_TOTAL][E_TOTAL] = {
//					 E_SPACE,	E_NUMBER,			E_OPEN_PAR,			E_CLOS_PAR,				E_SIGN,				E_OPERATOR,			E_SING_OP,			E_OTHER
	[S_INITIAL]	= {SE_SUCCESS, SE_SUCCESS,			SE_SUCCESS,			SE_EXCEDING_PARENTHESIS,SE_SUCCESS,			SE_EMPTY_OPERATOR,	SE_SUCCESS,			SE_UNDEFINED},
	[S_NUMBER]	= {SE_SUCCESS, SE_SUCCESS,			SE_MISSING_OPERATOR,SE_SUCCESS,				SE_SUCCESS,			SE_SUCCESS,			SE_MISSING_OPERATOR,SE_UNDEFINED},
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

json_obj	*check_syntax(char *input)
{
	int event, state = S_INITIAL, error = SE_SUCCESS;
	int	parenthesis = 0;
	int	i = 0;

	while (input[i] && state != S_ERROR)
	{
		event = get_event(input[i]);
		if (event == E_OPEN_PAR)
			parenthesis++;
		else if (event == E_CLOS_PAR)
			parenthesis--;
		if (parenthesis < 0)
		{
			state = S_ERROR;
			error = SE_EXCEDING_PARENTHESIS;
		}
		else
			error = get_error[state][event];
		state = state_conversor[state][event];
		// printf("reading %c the state is %d and the error %d\n", input[i], state, error);
		// fprintf(stdout, "reading %c end with state %s and error %d\n", input[i], states[state], error);
		i++;
	}
	i--;
	if (parenthesis && state != S_ERROR)
	{
		error = SE_MISSING_PARENTHESIS;
	}
	return (json_syntax_error(error, input, i));
}
