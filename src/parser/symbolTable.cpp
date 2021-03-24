#include "symbolTable.h"
#include "debug.h"

#define SYM_CLS(x) ((x.cls == CLASS_INT ? "int" : x.cls == CLASS_DOUBLE ? "double" : x.cls == CLASS_FLOAT ? "float" : x.cls == CLASS_BOOLEAN ? "bool" : "void"))

#define SYM_TYP(x) ((x.type == TYPE_CONST ? "const" : x.type == TYPE_VAR ? "var" : x.type == TYPE_ARRAY ? "array" : x.type == TYPE_FUNC ? "func" : x.type == TYPE_CONST_ARRAY ? "const_array" : "para"))

int MallocAddr(int cls) //for variable
{
	void *entry;
	switch (cls)
	{
	case CLASS_INT:
		entry = (int *)malloc(sizeof(int));
		break;
	case CLASS_DOUBLE:
		entry = (double *)malloc(sizeof(double));
		break;
	case CLASS_FLOAT:
		entry = (float *)malloc(sizeof(float));
		break;
	case CLASS_BOOLEAN:
		entry = (bool *)malloc(sizeof(bool));
		break;
	default:
		entry = (int *)malloc(sizeof(int));
		break;
	}
	return (int &)entry;
}

int CalculateOffset(int cls, int &cur_stack)
{
	int adder = (cls == CLASS_DOUBLE) ? 8 : 4;
	cur_stack += adder;
	return cur_stack;
}

int CalculateArrayOffset(int cls, int len, int &cur_stack)
{
	int adder = (cls == CLASS_DOUBLE) ? 8 : 4;
	cur_stack += adder * len;
	return cur_stack;
}

int MallocArrayAddr(int cls, int size) //for array
{
	void *entry;
	switch (cls)
	{
	case CLASS_INT:
		entry = (int *)malloc(size * sizeof(int));
		break;
	case CLASS_DOUBLE:
		entry = (double *)malloc(size * sizeof(double));
		break;
	case CLASS_FLOAT:
		entry = (float *)malloc(size * sizeof(float));
		break;
	case CLASS_BOOLEAN:
		entry = (bool *)malloc(size * sizeof(bool));
		break;
	default:
		entry = 0x0; //fail
	}
	return (int &)entry;
}

VarInfo *SymbolTable::lookup(const std::string &name)
{
	int depth = this->pseudo_stack.size();
	while (depth > 1) //search pseudo_stack for func  name
	{
		BlockTable *block = pseudo_stack[depth - 1];
		if (block->local_symbols.count(name) > 0)
			return &block->local_symbols[name];
		depth--;
	}
	// global
	if (this->global_symbols.count(name) > 0)
	{
		return &global_symbols[name];
	}
	else
	{
		return nullptr;
	}
}

std::string SymbolTable::parse_local_to_offset(VarInfo *info)
{
	if (!info->global) //local
	{
		int offset_to_sp = info->offset + 16; //16  ra:8 + sp:8
		std::string neg = "-";
		std::string ans = neg + std::to_string(offset_to_sp) + "(s0)";
		return ans;
	}
	else //GLOBAL
	{
		return (info->name) + ".g";
	}
}

VarInfo *SymbolTable::asm_lookup(const std::string &name)
{
}

/*std::string SymbolTable::rename_var(std::string &name, int line_num)
{

}*/

FuncInfo *SymbolTable::lookup_func(const std::string &func_name)
{
	if (this->func_symbols.count(func_name) == 1) // exists
		return &this->func_symbols[func_name];
	else
		return nullptr;
}

VarInfo *SymbolTable::lookup_param(FuncInfo &func, int para_idx)
{
	for (auto iter = func.base_block.local_symbols.begin(); iter != func.base_block.local_symbols.end(); iter++)
	{
		if (iter->second.length == para_idx && iter->second.type == TYPE_PARAM)
		{
			return &(iter->second);
		}
	}
	return nullptr;
}

void SymbolTable::addSymbol(std::string name, int cls, int type, int length, int line)
{
	VarInfo temp;
	temp.name = name;
	temp.cls = cls;
	temp.type = type;
	temp.length = length;
	temp.def_line = line;
	temp.asm_name = name + std::to_string(line);
	if (type == TYPE_CONST || type == TYPE_VAR || type == TYPE_PARAM)
	{
		temp.addr = MallocAddr(cls);
		//stack frame only work for local variables
		//when enter or exit func, set to zero
#ifdef DEBUG
		printf("\nbefore: %d ", stack_frame);
#endif
		stack_frame += (cls == CLASS_DOUBLE) ? 8 : 4;
		temp.offset = stack_frame;
#ifdef DEBUG
		printf("\nafter: %d ", stack_frame);
#endif
	}
	else if (type == TYPE_ARRAY || type == TYPE_CONST_ARRAY)
	{
		temp.addr = MallocArrayAddr(cls, length);
		stack_frame += ((cls == CLASS_DOUBLE) ? 8 : 4) * length;
		temp.offset = stack_frame;
	}

	int depth = this->pseudo_stack.size();
	if (cur_func == "$")
	{ //global
		if (this->global_symbols.count(name) == 0)
		{
			temp.global = 1;
			this->global_symbols.insert(std::pair<std::string, VarInfo>(name, temp));
		}

		else
			throw std::runtime_error("\nMultiple definition.\n");
	}
	else //直接加在栈顶block符号表中
	{
		BlockTable *top = pseudo_stack[depth - 1];
		if (top->local_symbols.count(name) != 0)
			throw std::runtime_error("\nmultiple definition\n");

		// If at function level, check 2 levels
		BlockTable *param_block = &func_symbols[cur_func].base_block;
		if (pseudo_stack[depth - 2] == param_block && param_block->local_symbols.count(name) != 0)
			throw std::runtime_error("\nFunction variable collide with parameter.\n");
		top->local_symbols.insert(std::pair<std::string, VarInfo>(name, temp));
	}
}

void SymbolTable::addFunc(std::string name, int rtn_class, int num_param, int line)
{
	FuncInfo *func = new FuncInfo;
	func->name = name;
	func->cls = rtn_class;
	func->num_params = num_param;
	func->def_line = line;
	if (cur_func != "$" || pseudo_stack.size() != 1)
	{
		// Nested definition; illegal
		throw std::runtime_error("\nNested function definition\n");
	}
	func->base_block.line = line;
	if (func_symbols.count(name) == 0)
		func_symbols.insert(std::pair<std::string, FuncInfo>(name, *func));
	else
		throw std::runtime_error("\nmulti definition of function at line: " + std::to_string(line));

	// change status to be in this func and block
	cur_func = name;
	pseudo_stack.push_back(&func_symbols[name].base_block);
}

void SymbolTable::addBlock(int line)
{
	BlockTable *block = new BlockTable;
	block->line = line;

	int depth = pseudo_stack.size();
	if (depth <= 1)
		throw std::runtime_error("\nBlock should only be in function.\n");
	BlockTable *top = pseudo_stack[depth - 1];
	top->children_blocks.insert(std::pair<int, BlockTable *>(line, block));
	pseudo_stack.push_back(block);
}

int SymbolTable::addStr(std::string &s)
{
	if (str_symbols.count(s) == 0)
	{
		str_symbols.insert(std::pair<std::string, int>(s, ++str_sym_counter));
		return str_sym_counter;
	}
	return str_symbols[s];
}

void printTable(std::ofstream &o, std::map<std::string, VarInfo> &sym_table)
{
	o << std::setw(14) << "name" << '|' << std::setw(7) << "class" << '|' << std::setw(12) << "type" << '|' << std::setw(10) << "val" << '|' << std::setw(5) << "def" << '|' << std::setw(11) << "offset" << std::endl;
	for (auto iter = sym_table.begin(); iter != sym_table.end(); iter++)
	{
		o << std::dec << std::setw(14) << iter->first << '|' << std::setw(7) << SYM_CLS(iter->second) << '|' << std::setw(12) << SYM_TYP(iter->second) << '|' << std::dec << std::setw(10) << iter->second.length << '|' << std::dec << std::setw(5) << iter->second.def_line << '|'
		  << std::setw(11) << iter->second.offset << '|' << std::endl;
	}
	o << std::endl;
}

void printBlock(std::ofstream &o, BlockTable *block)
{
	o << "Block " << block->line << " has " << block->children_blocks.size() << " child(ren):\n";
	for (auto iter = block->children_blocks.begin(); iter != block->children_blocks.end(); iter++)
		o << "\t" << iter->first;
	o << std::endl;

	o << "Block " << block->line << " locals:\n";
	printTable(o, block->local_symbols);
	for (auto iter = block->children_blocks.begin(); iter != block->children_blocks.end(); iter++)
		printBlock(o, iter->second);

	o << "Block " << block->line << " ends.\n";
}

void printFunc(std::ofstream &o, FuncInfo &func)
{
	o << func.name << ':' << std::endl;
	printBlock(o, &func.base_block);
	o << func.name << " ends.\n";
}

void SymbolTable::printSymbol(std::ofstream &o)
{
	for (auto iter = str_symbols.begin(); iter != str_symbols.end(); iter++)
	{
		o << "string_" << std::to_string(iter->second)
		  << " : .asciiz " << iter->first << '"' << std::endl;
	}
	o << std::right;
	o << "global:" << std::endl;
	printTable(o, global_symbols);

	for (auto iter = func_symbols.begin(); iter != func_symbols.end(); iter++)
	{
		if (iter->first != "main")
		{
			printFunc(o, iter->second);
		}
	}
	printFunc(o, func_symbols["main"]);
}
