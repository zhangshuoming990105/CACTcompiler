#include "IRListener.h"
#include "IR.h"

//产生临时变量
std::string IR::gen_temp(int lc, int cls, SymbolTable& st) {
    assert(cls == CLASS_INT || cls == CLASS_DOUBLE || cls == CLASS_FLOAT || cls == CLASS_BOOLEAN);
    auto name = OP::TEMP_VAR_HEAD + std::to_string(++temp_var_cnt);
    st.addSymbol(name, cls, TYPE_VAR, 0, lc);
	return name;
}

//产生临时数组
std::string IR::gen_temp_array(int lc, int cls, int size, SymbolTable& st) {
    assert(cls == CLASS_INT || cls == CLASS_DOUBLE || cls == CLASS_FLOAT || cls == CLASS_BOOLEAN);
    auto name = OP::TEMP_VAR_HEAD + std::to_string(++temp_var_cnt);
    st.addSymbol(name, cls, TYPE_ARRAY, size, lc);
	return name;
}

//产生标号
std::string IR::gen_label() {
    return OP::LABEL_HEAD + std::to_string(++label_cnt);
}

//添加中间代码四元式
void IR::addIMC(std::string rst, const std::string &op, std::string op1, std::string op2,int cls) {
    IRCode t;
    t.num1 = op1;
	//t.num1 = op1;
	t.num2 = op2;
	t.op = op;
	t.rst = rst;
	t.cls = cls;
    this->ir_codes.push_back(t);
}

//code归约
IRCode IR::popIMR() {
	auto back_ir = this->ir_codes[ir_codes.size()-1];
	this->ir_codes.pop_back();
	
	return back_ir;
}

//打印中间代码
void IR::printIMC(std::ofstream&o) {
	for(auto iter = ir_codes.begin(); iter!= ir_codes.end(); iter++) {
		o << std::right;
		std::string cls =(iter->cls==0)? "int":\
						(iter->cls==1)?	"double":\
						(iter->cls==2)?	"float":\
						(iter->cls==3)?	"bool":"";
			
		o << std::setw(10) << iter->op << "  " << std::setw(14) << iter->rst << "  " 
			<< std::setw(14) << iter->num1 << "  " << std::setw(14) << iter->num2 << "  "
			<< std::setw(7) << cls << std::endl;
	}

}

