CONTRACT ?= fuel
CONTRACT_ACCOUNT ?= fueltest.gm
NODE_URL ?= https://jungle3.greymass.com

contract: ${CONTRACT}.abi

%.abi: %.cpp %.contracts.md
	eosio-cpp -R contract -contract=$(notdir $(basename $<)) -abigen -abigen_output=$@ -o $(basename $<).wasm -O3 $<

.PHONY: format
format:
	clang-format --style=WebKit -i fuel.cpp

.PHONY: deploy-contract
deploy-contract: contract
	cleos -u $(NODE_URL) set contract \
		$(CONTRACT_ACCOUNT) . ${CONTRACT}.wasm ${CONTRACT}.abi

.PHONY: clean
clean:
	rm *.wasm *.abi
