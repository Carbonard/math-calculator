.PHONY: force

CFLAGS = -Werror -Wextra -Wall -O3
DEBUGFLAGS = -g3 -fsanitize=address

RATIONALS_SRC = rational_numbers.c

OTHER_SRC = basic_functions.c

CALCULATOR_SRC = $(RATIONALS_SRC) $(OTHER_SRC) calculator_check_input.c calculator_manage_input.c calculator_solve.c calculator_tree.c calculator_utils.c calculator_errors.c
WEB_SRC = json.c
CALCULATOR_WEB_SRC = $(CALCULATOR_SRC) $(WEB_SRC)

CALCULATOR = calculator
DEBUG = debug
WEB = docs/calculator.js docs/calculator.wasm

$(CALCULATOR): calculator_main.c $(CALCULATOR_SRC)
	cc $(CFLAGS) -DCMDLINE $^ json.c -lreadline -o $@

$(DEBUG): calculator_main.c $(CALCULATOR_SRC)
	cc $(DEBUGFLAGS) -DDEBUG -DCMDLINE $^ json.c -lreadline -o $@

web: $(WEB)

$(WEB): $(CALCULATOR_WEB_SRC) docs/index.html
# 	rm -f docs/calculator.js docs/calculator.wasm
	emcc -DWEB $(CALCULATOR_WEB_SRC) -s EXPORTED_FUNCTIONS='["_manage_input", "_free"]' -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" -o docs/calculator.js
