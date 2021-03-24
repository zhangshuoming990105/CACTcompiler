ANTLR4 = java -Xmx500M org.antlr.v4.Tool
ANTLR4_RUNTIME=./runtime

BUILD_DIR = ./build
SRC_DIR = ./src
OUT_DIR = ./out
OUT_FILE_DIR = ./out_file
OUT_REF_DIR = ./out_ref

all: parser
	mkdir -p $(OUT_DIR) $(OUT_FILE_DIR) $(OUT_REF_DIR)

GRAMMAR = CACT
GRAM_DIR = ./grammar
GRAM_SRC = $(abspath $(GRAM_DIR)/$(GRAMMAR).g4)
GRAM_GEN_DIR = $(abspath $(GRAM_DIR)/generated)

grammar: $(GRAM_GEN_DIR)

$(GRAM_GEN_DIR): $(GRAM_SRC)
	make -C $(GRAM_DIR)

PARSER = $(BUILD_DIR)/cact_compiler
PARSER_SRC_DIR = $(SRC_DIR)/parser
PARSER_CPP = $(shell find $(PARSER_SRC_DIR) -name "*.cpp")
GRAM_GEN_CPP = $(shell find $(GRAM_GEN_DIR) -name "*.cpp")

parser: grammar $(PARSER_SRC_DIR)/*
	mkdir -p $(BUILD_DIR)
	g++ $(PARSER_CPP) $(GRAM_GEN_CPP) -I$(PARSER_SRC_DIR) -I$(GRAM_GEN_DIR) \
		-L$(ANTLR4_RUNTIME)/run/usr/local/lib -lantlr4-runtime -I$(ANTLR4_RUNTIME)/run/usr/local/include/antlr4-runtime \
		-I$(ANTLR4_RUNTIME)/runtime/src -I$(ANTLR4_RUNTIME)/runtime/src/misc -I$(ANTLR4_RUNTIME)/runtime/src/atn \
		-I$(ANTLR4_RUNTIME)/runtime/src/dfa -I$(ANTLR4_RUNTIME)/runtime/src/tree -I$(ANTLR4_RUNTIME)/runtime/src/support \
		-std=c++11 -w -o $(PARSER) -g


ASM_FILE = $(OUT_DIR)/asm.S
EXEC_FILE = $(OUT_FILE_DIR)/a.out

TARGET_CC = riscv64-unknown-elf-gcc
LINK_CACT = -L./libcact -lcact

EMU = spike pk

run: $(EXEC_FILE)
	$(EMU) $<

$(EXEC_FILE): $(ASM_FILE)
	$(TARGET_CC) $< -o $@ $(LINK_CACT)


.PHONY: clean

clean:
	rm -rf $(GRAM_GEN_DIR)
	rm -rf $(BUILD_DIR)
	rm -f SymbolTable.txt main
	rm -rf $(OUT_DIR) $(OUT_FILE_DIR) $(OUT_REF_DIR)