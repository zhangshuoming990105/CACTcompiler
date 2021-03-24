ANTLR4 = java -Xmx500M org.antlr.v4.Tool

GRAMMAR = CACT
GRAM_SRC = $(GRAMMAR).g4
GRAM_GEN_DIR = ./generated

all: $(GRAM_SRC)
	$(ANTLR4) -visitor -o $(GRAM_GEN_DIR) -Dlanguage=Cpp $(GRAM_SRC)
