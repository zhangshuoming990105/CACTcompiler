#include <iostream>
#include <fstream>
#include "antlr4-runtime.h"
#include "CACTLexer.h"
#include "CACTParser.h"
#include "CACTBaseListener.h"
#include "IRListener.h"
#include "symbolTable.h"
#include "debug.h"

using namespace antlr4;

int main(int argc, const char *argv[])
{
	const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	// Get file name and strip rest of string
	std::string ProgramName = argv[1];
	ProgramName = ProgramName.substr(0, ProgramName.find_last_of("."));
	ProgramName = ProgramName.substr(ProgramName.find_last_of("/") + 1);
	// Input Stream
	std::ifstream stream;
	stream.open(argv[1]);
	// ANLTR Input Stream
	ANTLRInputStream input(stream);
	// Lexer
	CACTLexer lexer(&input);
	// Tokenizer
	CommonTokenStream tokens(&lexer);
	// Parser
	CACTParser parser(&tokens);
	// Parse Tree
	tree::ParseTree *tree = parser.start();
#ifdef DEBUG
	//print token stream from lexer
	/*std::cout << "DEBUG: token stream:" << std::endl;
    for (auto token : tokens.getTokens()) {
        std::cout << token->toString() << std::endl;
    }
	*/
    //std::cout << tree->toStringTree(&parser) << std::endl << std::endl;
#endif

	IRListener listener;
	if (parser.getNumberOfSyntaxErrors() > 0 || lexer.getNumberOfSyntaxErrors() > 0)
	{
		std::cout << "input .cact file has syntax error" << std::endl;
		return 1;
	}
	//semantic analysis: pass1
	tree::ParseTreeWalker walker;
	IRListener IR_listener;
	try {
        walker.walk(&IR_listener, tree);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
		std::cout << "\nerror in cact file ,compilation failed" << std::endl;
		return 1;
    }
	//pass first gen IRcode

	//could add extra passes to do IRcode optimisation later on

	//pass last gen RISCVcode
	std::string outName = "asm";	//out risc-v assembly file
	IR_listener.RISCV_Gen(outName);

	/**************main process ends**********************/
#ifdef DEBUG
	std::ofstream out("./SymbolTable.txt");
	IR_listener.ir.printIMC(out);
	IR_listener.sym_table.printSymbol(out);
#endif
	/****************DEBUG**********************************/
	
	//runtime counter
	const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	const auto elapsed_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	const auto elapsed_time_sec = (elapsed_time_us.count() / 1000000.0);
	std::cout << "Compilation Time :" << elapsed_time_sec << " seconds\n";
	return 0;
}