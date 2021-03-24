#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "headers.h"
enum
{
	CLASS_INT,
	CLASS_DOUBLE,
	CLASS_FLOAT,
	CLASS_BOOLEAN,
	CLASS_VOID
};
enum
{
	TYPE_CONST,
	TYPE_VAR,
	TYPE_ARRAY,
	TYPE_FUNC,
	TYPE_PARAM,
	TYPE_CONST_ARRAY
};

// 变量信息
struct VarInfo
{
	int cls;	   // class: int=0 double=1 float=2 bool=3 void=4
	int type;	   // type: const var array fun param
	int length;	   // const->value []->length, func->num_of_params
	int def_line;  // Line No.
	int addr = -1; // addr （用于RAM分配空间
	int offset = 0;// use offset instead of static addr
	int global = 0;// if is global var, set this flag
	std::string name;
	std::string asm_name;	//marked with line_num to identify different var with same name
	std::string literal_val;	
};

// Block Table
struct BlockTable
{
	int line;
	// child block with its starting line number
	std::map<int, struct BlockTable *> children_blocks;
	// local symbols
	std::map<std::string, VarInfo> local_symbols;
};

// Function Info
struct FuncInfo
{
	int cls;		// class of return value
	int num_params;	// number of params
	int def_line;	// line no.
	int size_temp = 0;	// number of temp
	int base = 0;
	int stack_size = 0;
	int temp_cnt = 0;
	BlockTable base_block;	// Base block for func
	std::string name;
};

//　符号表
class SymbolTable
{
public:
	// 全局符号表
	std::map<std::string, VarInfo> global_symbols;	
	
	// 函数符号表 以函数名为索引
	// 存储函数参数，局部变量，
	std::map<std::string, FuncInfo> func_symbols;
	// 字符串符号表
	std::map<std::string, int> str_symbols;
	// 用于字符串编号
	int str_sym_counter;
	// current function
	std::string cur_func;
	// stack of line no. of blocks
	std::vector<BlockTable *> pseudo_stack;

	int stack_frame = 0;

public:
	SymbolTable()
	{
		str_sym_counter = 0;
		cur_func = "$";
		pseudo_stack.push_back(nullptr);
	}
	// 查找符号表
	VarInfo *lookup(const std::string &name);

	std::string parse_local_to_offset(VarInfo *info);

	VarInfo *asm_lookup(const std::string &name);

	//重命名符号，通过添加行号后缀作为唯一标识，存在符号表中
	//std::string rename_var(std::string name, int line_num);


	// 查找函数符号表  函数只会在global中被定义
	FuncInfo *lookup_func(const std::string &func_name);
	// 查找函数参数
	VarInfo *lookup_param(FuncInfo &func_symtab, int para_idx);
	// 添加符号 length位多路复用 const->val array->width
	void addSymbol(std::string name, int _class, int type, int length, int line);
	// add function
	void addFunc(std::string name, int rtn_class, int num_param, int line);
	void addBlock(int line);
	// 添加字符串
	int addStr(std::string &s);

	void printSymbol(std::ofstream &o);
};

//can design an ad-hoc stack top for all temp 

//int MallocAddr(int class);

int MallocArrayAddr(int cls,int size);
int MallocAddr(int cls);
int CalculateOffset(int cls,int &cur_stack);
int CalculateArrayOffset(int cls,int len, int&cur_stack);

//int stack_frame = 0;



#endif