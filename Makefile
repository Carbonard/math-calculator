.PHONY: force

RATIONALS_SRC = rational_numbers.c

OTHER_SRC = basic_functions.c

CALCULATOR_SRC = $(RATIONALS_SRC) $(OTHER_SRC) calculator_check_input.c calculator_manage_input.c calculator_solve.c calculator_tree.c calculator_utils.c calculator_errors.c
WEB_SRC = json.c
CALCULATOR_WEB_SRC = $(CALCULATOR_SRC) $(WEB_SRC)

CALCULATOR = calculator
WEB = web/calculator.js web/calculator.wasm

$(CALCULATOR): calculator_main.c $(CALCULATOR_SRC)
	cc -DCMDLINE $^ json.c -lreadline -o $@

web: $(WEB)

$(WEB): $(CALCULATOR_WEB_SRC) web/index.html
# 	rm -f web/calculator.js web/calculator.wasm
	emcc -DWEB $(CALCULATOR_WEB_SRC) -s EXPORTED_FUNCTIONS='["_manage_input", "_free"]' -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" -o web/calculator.js
