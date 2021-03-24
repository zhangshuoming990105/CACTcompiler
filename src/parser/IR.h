/**
 * 创建IR
*/
#ifndef IR_H
#define IR_H

#include "headers.h"

//中间代码使用四元式（返回值、操作、操作数1、操作数2后面使用rst, op, num1, num2进行缩写）
//   rst Op num1 num2 
//目前没有做成SSA的必要
//即形如：
//	t1 ADD 0  0		t1 = 0
//	t1 ADD t2 3		t1 = t2 + 3
//	t2 MUL t2 t1	t2 = t2 * t1
struct IRCode{
	std::string rst;
	std::string op;
	std::string num1;
	std::string num2;
	int cls;	
};

// IR ops
namespace OP {
	constexpr auto ASSIGN = ":=";
	constexpr auto PARAM = "SAVE PARAM";
	constexpr auto MOV = "MOV";

	constexpr auto ADD = "add";	//x + a  b
	constexpr auto SUB = "sub";
	constexpr auto MUL = "mul";	
	constexpr auto DIV = "div";	
	constexpr auto MOD = "mod";	

	constexpr auto EQU = "==";	
	constexpr auto NEQ = "!=";
	constexpr auto GT = ">";
	constexpr auto GE = ">=";
	constexpr auto LT = "<";
	constexpr auto LE = "<=";
	constexpr auto NOT = "!";
	constexpr auto NEG = "-";
	constexpr auto AND = "&&";
	constexpr auto OR = "||";

	constexpr auto LOAD = "LOAD";
	constexpr auto LI = "li";	//only int constant or bool
	constexpr auto STORE = "STORE";

	constexpr auto SAVE_ARR = "[]=";	
	constexpr auto READ_ARR = "=[]";	
	
	//constexpr auto SCAN = "scanf";		
	constexpr auto PRINT = "print";		

	constexpr auto PUSH_PARA = "push";	
	constexpr auto CALL = "call";		
	constexpr auto RET = "ret";			
	constexpr auto FUNC_BEGIN = "func_begin";	
	constexpr auto FUNC_END = "func_end";	

	//跳转指令
	constexpr auto GOTO = "goto";	
	constexpr auto BEQ = "beq";
	constexpr auto BEQZ = "beqz";
	constexpr auto BNE = "bne";

	
	constexpr auto LABEL = "label";	
	constexpr auto EXIT = "exit";	

    /* temporary var head, #1, #2, ... */
	constexpr auto TEMP_VAR_HEAD = "#";
	/* string head, ~xxx, ~sakura, ~19260817 */
	//constexpr auto STR_HEAD = "~";
	/* label head, %1, %2, ... */
	constexpr auto LABEL_HEAD = "LABEL: ";
	/* return register */
	constexpr auto RETURN_REG = "a0";

	constexpr auto GLOBAL_ASSIGN = "G-ASSIGN";
	constexpr auto GLOBAL_ASSIGN_ARR = "G-ASSIGN-A";
	constexpr auto GLOBAL_ASSIGN_IDX = "G-ASSIGN-IDX";
}

class IR {
public:
    std::vector<IRCode> ir_codes;

    int temp_var_cnt;	//#i
    int label_cnt;		//%i
    //int str_cnt;		//~i
public:
    IR() {
        temp_var_cnt = 0;
        label_cnt = 0;
        //str_cnt = 0;
    }

    std::string gen_temp(int lc, int cls, SymbolTable& st);
	std::string gen_temp_array(int lc, int cls, int size, SymbolTable& st);
	std::string gen_label();

	void addIMC(std::string rst, const std::string &op, std::string op1, std::string op2,int cls);
	IRCode popIMR();
	void printIMC(std::ofstream&o);
};

#endif