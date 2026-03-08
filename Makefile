.PHONY: force

CFLAGS = -Werror -Wextra -Wall -O3
DEBUGFLAGS = -g3 -fsanitize=address

OTHER_SRC = basic_functions.c

CALCULATOR_C = check_input manage_input\
solve solve_apply_properties solve_operate_numbers solve_utils basic_functions\
tree_generator print_expr tree_utils\
errors json
CALCULATOR_SRC = $(CALCULATOR_C:%=src/%.c)
CALCULATOR_WEB_SRC = $(CALCULATOR_SRC)

CALCULATOR = calculator
DEBUG = debug
WEB = docs/calculator.js docs/calculator.wasm

$(CALCULATOR): src/main.c $(CALCULATOR_SRC)
	cc $(CFLAGS) -DCMDLINE $^ -lreadline -o $@

$(DEBUG): src/main.c src/debug.c $(CALCULATOR_SRC)
	cc $(DEBUGFLAGS) -DDEBUG -DCMDLINE $^ -lreadline -o $@

web: $(WEB)

$(WEB): $(CALCULATOR_WEB_SRC) docs/index.html
# 	rm -f docs/calculator.js docs/calculator.wasm
	emcc -DWEB $(CALCULATOR_WEB_SRC) -s EXPORTED_FUNCTIONS='["_manage_input", "_free"]' -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" -o docs/calculator.js
