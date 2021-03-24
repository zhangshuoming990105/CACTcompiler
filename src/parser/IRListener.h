#ifndef __IRLISTENER_H
#define __IRLISTENER_H
#include "headers.h"
#include "CACTBaseListener.h"
#include "symbolTable.h"
#include "IR.h"

class IRListener : public CACTBaseListener
{
public:
	SymbolTable sym_table;
	IR ir;
	std::string curr_func; // 当前所在的func, ""为global
	bool in_func;		   // 在函数定义中
	//-------version 0------, 用以简单栈来传值
	std::stack<std::string> u_stack;	//type stack
	std::vector<std::string> label_stack;	//label stack
	//bool in_assign_stmt=false;	//lval whether gencode

	// 每一节点临时变量类型
	antlr4::tree::ParseTreeProperty<int> temp_class;
	// 每一节点临时变量名字
	antlr4::tree::ParseTreeProperty<std::string> temp_var;

	antlr4::tree::ParseTreeProperty<std::string> temp_value;

	// 临时变量为array时的idx
	antlr4::tree::ParseTreeProperty<std::string> temp_idx;
	// if语句用到的label
	antlr4::tree::ParseTreeProperty<std::vector<std::string>> if_labels;

	// while语句用到的label
	antlr4::tree::ParseTreeProperty<std::vector<std::string>> while_labels;


	int end_func_label = 0;
	int float_tmp_cnt = 0;

public:
	IRListener()
	{
		curr_func = "";
		in_func = false;
	};
	~IRListener(){};

	void enterStart(CACTParser::StartContext * /*ctx*/) override;
	void exitStart(CACTParser::StartContext * /*ctx*/) override;
	//func def
	void enterFuncDef(CACTParser::FuncDefContext * /*ctx*/) override;
	void exitFuncDef(CACTParser::FuncDefContext * /*ctx*/) override;
	//func params at def stage
	void enterFuncFParam(CACTParser::FuncFParamContext * /*ctx*/) override;
	void exitFuncFParam(CACTParser::FuncFParamContext * /*ctx*/) override;
	// statement blocks
	void enterBlock(CACTParser::BlockContext * /*ctx*/) override;
	void exitBlock(CACTParser::BlockContext * /*ctx*/) override;

	//const decl
	void enterConstDecl(CACTParser::ConstDeclContext * /*ctx*/) override;
	void exitConstDecl(CACTParser::ConstDeclContext * /*ctx*/) override;

	void enterConstDefVal(CACTParser::ConstDefValContext * /*ctx*/) override;
	void exitConstDefVal(CACTParser::ConstDefValContext * /*ctx*/) override;

	void enterConstDefArray(CACTParser::ConstDefArrayContext * /*ctx*/) override;
	void exitConstDefArray(CACTParser::ConstDefArrayContext * /*ctx*/) override;
	//var decl
	void enterVarDecl(CACTParser::VarDeclContext * /*ctx*/) override;
	void exitVarDecl(CACTParser::VarDeclContext * /*ctx*/) override;

	void enterDefVal(CACTParser::DefValContext * /*ctx*/) override;
	void exitDefVal(CACTParser::DefValContext * /*ctx*/) override;

	void enterDefArray(CACTParser::DefArrayContext * /*ctx*/) override;
	void exitDefArray(CACTParser::DefArrayContext * /*ctx*/) override;

	void enterDefInitVal(CACTParser::DefInitValContext * /*ctx*/) override;
	void exitDefInitVal(CACTParser::DefInitValContext * /*ctx*/) override;

	void enterDefInitArray(CACTParser::DefInitArrayContext * /*ctx*/) override;
	void exitDefInitArray(CACTParser::DefInitArrayContext * /*ctx*/) override;

	void enterAssignStmt(CACTParser::AssignStmtContext * /*ctx*/) override;
	void exitAssignStmt(CACTParser::AssignStmtContext * /*ctx*/) override;

	//expression
	void enterExp(CACTParser::ExpContext * /*ctx*/) override;
	void exitExp(CACTParser::ExpContext * /*ctx*/) override;

	void enterAddExp(CACTParser::AddExpContext * /*ctx*/) override;
	void exitAddExp(CACTParser::AddExpContext * /*ctx*/) override;

	void enterMulExp(CACTParser::MulExpContext * /*ctx*/) override;
	void exitMulExp(CACTParser::MulExpContext * /*ctx*/) override;

	void enterPrimary(CACTParser::PrimaryContext * /*ctx*/) override;
	void exitPrimary(CACTParser::PrimaryContext * /*ctx*/) override;

	void enterFuncall(CACTParser::FuncallContext * /*ctx*/) override;
	void exitFuncall(CACTParser::FuncallContext * /*ctx*/) override;

	void enterUnary(CACTParser::UnaryContext * /*ctx*/) override;
	void exitUnary(CACTParser::UnaryContext * /*ctx*/) override;

	void enterLVal(CACTParser::LValContext * /*ctx*/) override;
	void exitLVal(CACTParser::LValContext * /*ctx*/) override;

	void enterReturnStmt(CACTParser::ReturnStmtContext * /*ctx*/) override;
	void exitReturnStmt(CACTParser::ReturnStmtContext * /*ctx*/) override;

	void enterRelExp(CACTParser::RelExpContext * /*ctx*/) override;
	void exitRelExp(CACTParser::RelExpContext * /*ctx*/) override;

	void enterEqExp(CACTParser::EqExpContext * /*ctx*/) override;
	void exitEqExp(CACTParser::EqExpContext * /*ctx*/) override;

	void enterLAndExp(CACTParser::LAndExpContext * /*ctx*/) override;
	void exitLAndExp(CACTParser::LAndExpContext * /*ctx*/) override;

	void enterLOrExp(CACTParser::LOrExpContext * /*ctx*/) override;
	void exitLOrExp(CACTParser::LOrExpContext * /*ctx*/) override;

	void enterCond(CACTParser::CondContext * /*ctx*/) override;
	void exitCond(CACTParser::CondContext * /*ctx*/) override;

	void enterIfCond(CACTParser::IfCondContext * /*ctx*/) override;
	void exitIfCond(CACTParser::IfCondContext * /*ctx*/) override;

	void enterWhileCond(CACTParser::WhileCondContext * /*ctx*/) override;
	void exitWhileCond(CACTParser::WhileCondContext * /*ctx*/) override;

	void enterIfStmt(CACTParser::IfStmtContext * /*ctx*/) override;
	void exitIfStmt(CACTParser::IfStmtContext * /*ctx*/) override;

	void enterIfElseStmt(CACTParser::IfElseStmtContext * /*ctx*/) override;
	void exitIfElseStmt(CACTParser::IfElseStmtContext * /*ctx*/) override;

	void enterElseStmt(CACTParser::ElseStmtContext * /*ctx*/) override;
	void exitElseStmt(CACTParser::ElseStmtContext * /*ctx*/) override;

	void enterWhileStmt(CACTParser::WhileStmtContext * /*ctx*/) override;
	void exitWhileStmt(CACTParser::WhileStmtContext * /*ctx*/) override;

	void enterBreakStmt(CACTParser::BreakStmtContext * /*ctx*/) override;
  	void exitBreakStmt(CACTParser::BreakStmtContext * /*ctx*/) override;

 	void enterContinueStmt(CACTParser::ContinueStmtContext * /*ctx*/) override;
	void exitContinueStmt(CACTParser::ContinueStmtContext * /*ctx*/) override;
public:
	//code gen↓↓↓↓↓↓↓↓
	// 生成的目标代码
	std::vector<std::string> riscv_codes;

	void RISCV_Gen(std::string out_file);

	void RISCV_FuncBegin(IRCode & );
	void RISCV_FuncEnd(IRCode & );
	void RISCV_Label(IRCode & );
	void RISCV_Goto(IRCode & );
	void RISCV_Return(IRCode & );
	void RISCV_Call(IRCode & );
	void RISCV_Move(IRCode & );
	void RISCV_Arithmetic(IRCode & );
	void RISCV_Load(IRCode & );
	void RISCV_Store(IRCode & );
	void RISCV_READARR(IRCode & );
	void RISCV_SAVEARR(IRCode & );
	void RISCV_Li(IRCode & );
	void RISCV_Beqz(IRCode & );
	void RISCV_GLOBAL(IRCode & );
	void RISCV_GLOBAL_ARR(IRCode & );
	void RISCV_GLOBAL_ARR_IDX(IRCode & );
};

#define INTEGER 0
#define REAL 1
#define BOOLEAN 2
#define UNINITIALIZED 0

#endif //__IRLISTENER_H