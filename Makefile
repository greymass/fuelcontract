fuel.wasm: fuel.cpp fuel.contracts.md
	eosio-cpp -R contract -abigen -contract=fuel fuel.cpp \
	-O3 -o fuel.wasm


.PHONY: format
format:
	clang-format --style=WebKit -i fuel.cpp
