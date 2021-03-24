#include "IRListener.h"
#include "CACTLexer.h"
#include "symbolTable.h"
#include "debug.h"
#include <iostream>
#include "utils.h"
const std::string VOID = "";
const std::string TRUE = "true";
const std::string FALSE = "false";

void IRListener::enterStart(CACTParser::StartContext *ctx)
{
    //library function register into sym_table
    sym_table.addFunc("print_int", CLASS_VOID, 1, 0);
    in_func = true;
    ir.addIMC("print_int", OP::FUNC_BEGIN, "0", "0", CLASS_VOID);
    sym_table.addSymbol("_int_", CLASS_INT, TYPE_PARAM, 0, 0);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("print_int", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;

    sym_table.addFunc("print_bool", CLASS_VOID, 1, 0);
    in_func = true;
    ir.addIMC("print_bool", OP::FUNC_BEGIN, "0", "0", CLASS_VOID);
    sym_table.addSymbol("_bool_", CLASS_BOOLEAN, TYPE_PARAM, 0, 0);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("print_bool", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;

    sym_table.addFunc("print_float", CLASS_VOID, 1, 0);
    in_func = true;
    ir.addIMC("print_float", OP::FUNC_BEGIN, "0", "0", CLASS_VOID);
    sym_table.addSymbol("_float_", CLASS_FLOAT, TYPE_PARAM, 0, 0);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("print_float", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;

    sym_table.addFunc("print_double", CLASS_VOID, 1, 0);
    in_func = true;
    ir.addIMC("print_double", OP::FUNC_BEGIN, "0", "0", CLASS_VOID);
    sym_table.addSymbol("_double_", CLASS_DOUBLE, TYPE_PARAM, 0, 0);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("print_double", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;

    sym_table.addFunc("get_int", CLASS_INT, 0, 0);
    in_func = true;
    ir.addIMC("get_int", OP::FUNC_BEGIN, "0", "0", CLASS_INT);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("get_int", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;

    sym_table.addFunc("get_float", CLASS_FLOAT, 0, 0);
    in_func = true;
    ir.addIMC("get_float", OP::FUNC_BEGIN, "0", "0", CLASS_FLOAT);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("get_float", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;

    sym_table.addFunc("get_double", CLASS_DOUBLE, 0, 0);
    in_func = true;
    ir.addIMC("get_double", OP::FUNC_BEGIN, "0", "0", CLASS_DOUBLE);
    sym_table.pseudo_stack.pop_back();
    sym_table.cur_func = "$";
    in_func = false;
    ir.addIMC("get_double", OP::FUNC_END, "0", "0", CLASS_VOID);
    sym_table.stack_frame = 0;
}
void IRListener::exitStart(CACTParser::StartContext *ctx)
{
    if (sym_table.lookup_func("main") == nullptr)
        throw std::runtime_error("\nexpect a main function in CACT file\n");
}

void IRListener::enterFuncDef(CACTParser::FuncDefContext *ctx)
{
#ifdef DEBUG
    printf("entering func: %s \n", ctx->name->getText().c_str());
    printf("    return type: %s \n", ctx->ret->getText().c_str());
    printf("    param_nums: %d \n", ctx->funcFParam().size());
#endif
    // get func name
    sym_table.stack_frame = 0;
    std::string func_name = ctx->name->getText();
    // check return type
    std::string rt_type = ctx->ret->getText();
    int rt_type_int;
    if (rt_type == "int")
    {
        rt_type_int = CLASS_INT;
    }
    else if (rt_type == "float")
    {
        rt_type_int = CLASS_FLOAT;
    }
    else if (rt_type == "double")
    {
        rt_type_int = CLASS_DOUBLE;
    }
    else if (rt_type == "bool")
    {
        rt_type_int = CLASS_BOOLEAN;
    }
    else if (rt_type == "void")
    {
        rt_type_int = CLASS_VOID;
    }
    else
    {
        throw std::runtime_error("\nunsupport function-return type at line: " + std::to_string(ctx->getStart()->getLine()));
    }

    // get param num
    int param_nums = ctx->funcFParam().size();
    if (func_name == "main")
    {
        if (param_nums != 0)
        {
            throw std::runtime_error("\nCACT main function has no args\n");
        }
        if (rt_type_int != CLASS_INT)
        {
            throw std::runtime_error("\nCACT main function must be int return type\n");
        }
    }
    // -----sym table and ir------
    // add func to sym table
    int depth = sym_table.pseudo_stack.size();
    sym_table.addFunc(func_name, rt_type_int, param_nums, ctx->getStart()->getLine());

    in_func = true;
    ir.addIMC(func_name, OP::FUNC_BEGIN, VOID, VOID, rt_type_int);

    // add param to sym table
    int t_cnt = 0; // param cnt
    for (auto &param : ctx->funcFParam())
    {
        std::string param_type = param->bType()->getText();
        std::string param_name = param->Ident()->getText();
        int param_cls;
        if (param_type == "int")
        {
            param_cls = CLASS_INT;
        }
        else if (param_type == "double")
        {
            param_cls = CLASS_DOUBLE;
        }
        else if (param_type == "float")
        {
            param_cls = CLASS_FLOAT;
        }
        else if (param_type == "bool")
        {
            param_cls = CLASS_BOOLEAN;
        }
        else
        {
            throw std::runtime_error("\ninvalid type at line " + std::to_string(ctx->getStart()->getLine()));
        }
        //定义形参时不考虑是否与全局名字重名
        sym_table.addSymbol(param_name, param_cls, TYPE_PARAM, t_cnt, param->getStart()->getLine());
        auto info = sym_table.lookup(param_name);
        auto str = sym_table.parse_local_to_offset(info);
        ir.addIMC("t2", OP::MOV, "a" + std::to_string(t_cnt), VOID, param_cls);
        ir.addIMC("t2", OP::STORE, str, VOID, param_cls);
        //read param register, store to memory
        t_cnt++;
    }
}
void IRListener::exitFuncDef(CACTParser::FuncDefContext *ctx)
{
#ifdef DEBUG
    printf("leaving func %s \n", this->curr_func.c_str());
    printf("func %s frame size is %d\n", this->curr_func.c_str(), sym_table.stack_frame);
#endif
    std::string func_name = ctx->name->getText();
    ir.addIMC(func_name, OP::FUNC_END, "0", "0", CLASS_VOID);
    auto funcInfo = sym_table.lookup_func(func_name);
    funcInfo->stack_size = sym_table.stack_frame;
    sym_table.cur_func = "$";
    sym_table.pseudo_stack.pop_back(); // This is for base block
    in_func = false;
    sym_table.stack_frame = 0;
}

void IRListener::enterFuncFParam(CACTParser::FuncFParamContext *ctx) {}
void IRListener::exitFuncFParam(CACTParser::FuncFParamContext *ctx) {}

void IRListener::enterBlock(CACTParser::BlockContext *ctx)
{
    sym_table.addBlock(ctx->getStart()->getLine());
}

void IRListener::exitBlock(CACTParser::BlockContext *ctx)
{
    sym_table.pseudo_stack.pop_back();
}

void IRListener::enterConstDecl(CACTParser::ConstDeclContext *ctx)
{
    std::string class_str = ctx->bType()->getText();
    int var_nums = ctx->constDef().size();
    int var_type = TYPE_CONST;
    u_stack.push(class_str); //push type info into stack for child to use
}
void IRListener::exitConstDecl(CACTParser::ConstDeclContext *ctx)
{
    u_stack.pop(); //pop the type info
}

void IRListener::enterConstDefVal(CACTParser::ConstDefValContext *ctx)
{
    std::string var_name = ctx->Ident()->getText();
    int var_cls;
    int var_type = TYPE_CONST;
    std::string class_str = u_stack.top();
    std::string literal_val = ctx->constInitVal()->getText();
    int vi;
    bool vb;
    float vf;
    double vd;

    if (class_str == "bool")
    {
        vb = (literal_val == "true") ? 1 : 0;
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::BoolConst)
            throw std::runtime_error("\ninvalid bool value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));

        var_cls = CLASS_BOOLEAN;
        sym_table.addSymbol(var_name, var_cls, var_type, vb, ctx->getStart()->getLine());
    }
    else if (class_str == "int")
    {
        vi = std::stoi(literal_val, 0, getIntStringBase(literal_val));
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::IntConst)
            throw std::runtime_error("\ninvalid int value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
        var_cls = CLASS_INT;
        sym_table.addSymbol(var_name, var_cls, var_type, vi, ctx->getStart()->getLine());
    }
    else if (class_str == "double")
    {
        vd = std::stod(literal_val);
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::DoubleConst)
            throw std::runtime_error("\ninvalid double value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
        var_cls = CLASS_DOUBLE;
        sym_table.addSymbol(var_name, var_cls, var_type, vd, ctx->getStart()->getLine());
    }
    else if (class_str == "float")
    {
        vf = std::stof(literal_val.substr(0, literal_val.length() - 1));
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::FloatConst)
            throw std::runtime_error("\ninvalid float value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
        var_cls = CLASS_FLOAT;
        sym_table.addSymbol(var_name, var_cls, var_type, vf, ctx->getStart()->getLine());
    }

    VarInfo *var = sym_table.lookup(var_name);
    auto str = sym_table.parse_local_to_offset(var);
    int global = var->global;
    if (!global)
    {
        if (var_cls == CLASS_INT)
        {
            ir.addIMC("t2", OP::LI, std::to_string(vi), VOID, CLASS_INT);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_INT);
        }

        else if (var_cls == CLASS_FLOAT)
        {
            ir.addIMC("t2", OP::LI, std::to_string(vf), VOID, CLASS_FLOAT);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_FLOAT);
        }

        else if (var_cls == CLASS_BOOLEAN)
        {
            ir.addIMC("t2", OP::LI, std::to_string(vb), VOID, CLASS_BOOLEAN);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_BOOLEAN);
        }

        else if (var_cls == CLASS_DOUBLE)
        {
            ir.addIMC("t2", OP::LI, std::to_string(vd), VOID, CLASS_DOUBLE);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_DOUBLE);
        }
    }
    else //global
    {
        if (var_cls == CLASS_INT)
        {
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(vi), "const", CLASS_INT);
        }
        else if (var_cls == CLASS_BOOLEAN)
        {
            int val = (literal_val == "true") ? 1 : 0;
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(vb), "const", CLASS_BOOLEAN);
        }
        else if (var_cls == CLASS_DOUBLE)
        {
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(vd), "const", CLASS_DOUBLE);
        }
        else if (var_cls == CLASS_FLOAT)
        {
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(vf), "const", CLASS_FLOAT);
        }
    }
}
void IRListener::exitConstDefVal(CACTParser::ConstDefValContext *ctx) {}

void IRListener::enterConstDefArray(CACTParser::ConstDefArrayContext *ctx)
{
    int array_len = std::stoi(ctx->IntConst()->getText());
    if (array_len <= 0)
        throw std::runtime_error("\nunexpect array len <=0 at line: " + std::to_string(ctx->getStart()->getLine()));
    std::string var_name = ctx->Ident()->getText();
    std::string class_str = u_stack.top();
    int var_cls;
    int var_type = TYPE_CONST_ARRAY;
    if (class_str == "int")
        var_cls = CLASS_INT;
    else if (class_str == "double")
        var_cls = CLASS_DOUBLE;
    else if (class_str == "float")
        var_cls = CLASS_FLOAT;
    else if (class_str == "bool")
        var_cls = CLASS_BOOLEAN;
    else
        throw std::runtime_error("\nunsupport variable type at line: " + std::to_string(ctx->getStart()->getLine()));
    sym_table.addSymbol(var_name, var_cls, var_type, array_len, ctx->getStart()->getLine());
    VarInfo *searched_var = sym_table.lookup(var_name);
    auto str = sym_table.parse_local_to_offset(searched_var);
    int global = searched_var->global;
    if (!global) //local constant
    {
    }
    else //global
    {
        ir.addIMC(var_name, OP::GLOBAL_ASSIGN_ARR, std::to_string(array_len), "const", var_cls);
        int k = 0;
        for (auto &elem : ctx->constExp())
        {
            std::string literal_val = elem->getText();
            if (var_cls == CLASS_INT)
            {
                int val = std::stoi(literal_val, 0, getIntStringBase(literal_val));
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(val), std::to_string(k), var_cls);
            }
            else if (var_cls == CLASS_BOOLEAN)
            {
                int val = (literal_val == "true") ? 1 : 0;
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(val), std::to_string(k), var_cls);
            }
            else if (var_cls == CLASS_DOUBLE)
            {
                double vd = std::stod(literal_val);
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(vd), std::to_string(k), var_cls);
            }
            else //FLOAT
            {
                float vf = std::stof(literal_val);
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(vf), std::to_string(k), var_cls);
            }

            k++;
        }
        if (k > array_len)
            throw std::runtime_error("\ninit array out of bound\n");
        while (k < array_len)
        {
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, "0", std::to_string(k), var_cls);
            k++;
        }
    }
}
void IRListener::exitConstDefArray(CACTParser::ConstDefArrayContext *ctx) {}

void IRListener::enterVarDecl(CACTParser::VarDeclContext *ctx)
{
    std::string class_str = ctx->bType()->getText();
    int var_nums = ctx->varDef().size();
    u_stack.push(class_str); //push type info into stack for child to use
}
void IRListener::exitVarDecl(CACTParser::VarDeclContext *ctx)
{
    u_stack.pop();
}

void IRListener::enterDefVal(CACTParser::DefValContext *ctx)
{
    std::string class_str = u_stack.top();
    int var_cls;
    var_cls = (class_str == "int") ? CLASS_INT : (class_str == "float") ? CLASS_FLOAT : (class_str == "double") ? CLASS_DOUBLE : (class_str == "bool") ? CLASS_BOOLEAN : CLASS_VOID;
    std::string var_name = ctx->Ident()->getText();
    int var_type = TYPE_VAR;
    int var_val = UNINITIALIZED;
    sym_table.addSymbol(var_name, var_cls, var_type, var_val, ctx->getStart()->getLine());
}
void IRListener::exitDefVal(CACTParser::DefValContext *ctx) {}

void IRListener::enterDefArray(CACTParser::DefArrayContext *ctx)
{
    std::string class_str = u_stack.top();
    int var_cls;
    var_cls = (class_str == "int") ? CLASS_INT : (class_str == "float") ? CLASS_FLOAT : (class_str == "double") ? CLASS_DOUBLE : (class_str == "bool") ? CLASS_BOOLEAN : CLASS_VOID;
    std::string var_name = ctx->Ident()->getText();
    int var_type = TYPE_ARRAY;
    int length = stoi(ctx->IntConst()->getText());
    sym_table.addSymbol(var_name, var_cls, var_type, length, ctx->getStart()->getLine());
    auto array_info = sym_table.lookup(var_name);
    int global = array_info->global;
    if (global)
    {
        ir.addIMC(var_name, OP::GLOBAL_ASSIGN_ARR, std::to_string(length), "uninited", var_cls);
    }
}
void IRListener::exitDefArray(CACTParser::DefArrayContext *ctx) {}

void IRListener::enterDefInitVal(CACTParser::DefInitValContext *ctx)
{
    std::string class_str = u_stack.top();
    int var_cls;
    var_cls = (class_str == "int") ? CLASS_INT : (class_str == "float") ? CLASS_FLOAT : (class_str == "double") ? CLASS_DOUBLE : (class_str == "bool") ? CLASS_BOOLEAN : CLASS_VOID;

    std::string var_name = ctx->Ident()->getText();
    std::string var_initval;
    var_initval = ctx->constInitVal()->getText();
    int var_type = TYPE_VAR;
    if (class_str == "bool")
    {
        var_cls = CLASS_BOOLEAN;
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::BoolConst)
            throw std::runtime_error("\ninvalid bool value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    else if (class_str == "int")
    {
        var_cls = CLASS_INT;
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::IntConst)
            throw std::runtime_error("\ninvalid int value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    else if (class_str == "double")
    {
        var_cls = CLASS_DOUBLE;
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::DoubleConst)
            throw std::runtime_error("\ninvalid double value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    else if (class_str == "float")
    {
        var_cls = CLASS_FLOAT;
        if (ctx->constInitVal()->getStart()->getType() != CACTLexer::FloatConst)
            throw std::runtime_error("\ninvalid float value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    sym_table.addSymbol(var_name, var_cls, var_type, 0, ctx->getStart()->getLine());
    VarInfo *var = sym_table.lookup(var_name);
    auto global = var->global;
    auto str = sym_table.parse_local_to_offset(var);
    if (!global)
    {
        if (var_cls == CLASS_INT)
        {
            int vi = std::stoi(var_initval, 0, getIntStringBase(var_initval));
            //ir.addIMC(str, OP::ASSIGN, std::to_string(vi), VOID);
            ir.addIMC("t2", OP::LI, std::to_string(vi), VOID, CLASS_INT);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_INT);
        }
        else if (var_cls == CLASS_FLOAT)
        {
            float vf = std::stof(var_initval.substr(0, var_initval.length() - 1));
            ir.addIMC("t2", OP::LI, std::to_string(vf), VOID, CLASS_FLOAT);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_FLOAT);
        }

        else if (var_cls == CLASS_BOOLEAN)
        {
            int vb = (var_initval == "true") ? 1 : 0;
            ir.addIMC("t2", OP::LI, std::to_string(vb), VOID, CLASS_BOOLEAN);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_BOOLEAN);
        }

        else if (var_cls == CLASS_DOUBLE)
        {
            double vd = std::stod(var_initval);
            ir.addIMC("t2", OP::LI, std::to_string(vd), VOID, CLASS_DOUBLE);
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_DOUBLE);
        }
    }
    else //global
    {
        if (var_cls == CLASS_INT)
        {
            int val = std::stoi(var_initval, 0, getIntStringBase(var_initval));
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(val), "var", CLASS_INT);
        }
        else if (var_cls == CLASS_BOOLEAN)
        {
            int val = (var_initval == "true") ? 1 : 0;
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(val), "var", CLASS_BOOLEAN);
        }
        else if (var_cls == CLASS_DOUBLE)
        {
            double vd = std::stod(var_initval);
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(vd), "var", CLASS_DOUBLE);
        }
        else if (var_cls == CLASS_FLOAT)
        {
            float vf = std::stof(var_initval);
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN, std::to_string(vf), "var", CLASS_FLOAT);
        }
    }
}
void IRListener::exitDefInitVal(CACTParser::DefInitValContext *ctx) {}

void IRListener::enterDefInitArray(CACTParser::DefInitArrayContext *ctx)
{
    std::string class_str = u_stack.top();
    int var_cls;
    var_cls = (class_str == "int") ? CLASS_INT : (class_str == "float") ? CLASS_FLOAT : (class_str == "double") ? CLASS_DOUBLE : (class_str == "bool") ? CLASS_BOOLEAN : CLASS_VOID;
    std::string var_name = ctx->Ident()->getText();
    int var_type = TYPE_ARRAY;
    int length = stoi(ctx->IntConst()->getText());
    sym_table.addSymbol(var_name, var_cls, var_type, length, ctx->getStart()->getLine());
    VarInfo *searched_var = sym_table.lookup(var_name);
    auto global = searched_var->global;
    int k = 0;
    if (!global) //not global, data in stack
    {
        auto array_str = sym_table.parse_local_to_offset(searched_var);
        if (var_cls == CLASS_INT)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::IntConst)
                    throw std::runtime_error("\ninvalid int value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                std::string name = elem->getText();
                int value = std::stoi(name, 0, getIntStringBase(name));
                name = std::to_string(value);
                ir.addIMC("t2", OP::LI, name, VOID, var_cls);
                ir.addIMC("t3", OP::LI, std::to_string(k), VOID, CLASS_INT);
                ir.addIMC("t2", OP::SAVE_ARR, array_str, "t3", var_cls);
                k++;
            }
        }
        else if (var_cls == CLASS_DOUBLE)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::DoubleConst)
                    throw std::runtime_error("\ninvalid double value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                std::string name = elem->getText();
                double value = std::stod(name);
                name = std::to_string(value);
                ir.addIMC("t2", OP::LI, name, VOID, var_cls);
                ir.addIMC("t3", OP::LI, std::to_string(k), VOID, CLASS_INT);
                ir.addIMC("t2", OP::SAVE_ARR, array_str, "t3", var_cls);
                k++;
            }
        }
        else if (var_cls == CLASS_FLOAT)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::FloatConst)
                    throw std::runtime_error("\ninvalid float value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                std::string name = elem->getText();
                float value = std::stof(name.substr(0, name.length() - 1));
                name = std::to_string(value);
                ir.addIMC("t2", OP::LI, name, VOID, var_cls);
                ir.addIMC("t3", OP::LI, std::to_string(k), VOID, CLASS_INT);
                ir.addIMC("t2", OP::SAVE_ARR, array_str, "t3", var_cls);
                k++;
            }
        }
        else if (var_cls == CLASS_BOOLEAN)
        {
            bool *addr = (bool *)searched_var->addr; //find base here
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::BoolConst)
                    throw std::runtime_error("\ninvalid bool value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));

                std::string name = elem->getText();
                std::string value = (name == "true") ? "1" : "0";
                ir.addIMC("t2", OP::LI, value, VOID, var_cls);
                ir.addIMC("t3", OP::LI, std::to_string(k), VOID, CLASS_INT);
                ir.addIMC("t2", OP::SAVE_ARR, array_str, "t3", var_cls);
                k++;
            }
        }
        else
            throw std::runtime_error("\nunsupport variable type xxx");
        if (k > length)
            throw std::runtime_error("\ninit array out of bound\n");
        while (k < length)
        {
            ir.addIMC("t2", OP::LI, "0", VOID, var_cls);
            ir.addIMC("t3", OP::LI, std::to_string(k), VOID, CLASS_INT);
            ir.addIMC("t2", OP::SAVE_ARR, array_str, "t3", var_cls);
            k++;
        }
    }
    else //global
    {    //数组都align3
        ir.addIMC(var_name, OP::GLOBAL_ASSIGN_ARR, std::to_string(length), "var", var_cls);
        //该IR翻译成直到具体赋值前的所有asm
        if (var_cls == CLASS_INT)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::IntConst)
                    throw std::runtime_error("\ninvalid int value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                auto name = elem->getText();
                int vi = std::stoi(name, 0, getIntStringBase(name));
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(vi), std::to_string(k), var_cls);
                k++;
            }
        }
        else if (var_cls == CLASS_FLOAT)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::FloatConst)
                    throw std::runtime_error("\ninvalid float value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                std::string name = elem->getText();
                name = name.substr(0, name.length() - 1);
                float vf = std::stof(name);
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(vf), std::to_string(k), var_cls);
                k++;
            }
        }
        else if (var_cls == CLASS_DOUBLE)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::DoubleConst)
                    throw std::runtime_error("\ninvalid double value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                std::string name = elem->getText();
                double vd = std::stod(name);
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, std::to_string(vd), std::to_string(k), var_cls);
                k++;
            }
        }
        else if (var_cls == CLASS_BOOLEAN)
        {
            for (auto &elem : ctx->constExp())
            {
                if (elem->getStart()->getType() != CACTLexer::BoolConst)
                    throw std::runtime_error("\ninvalid bool value " + var_name + " at line: " + std::to_string(ctx->getStart()->getLine()));
                std::string name = elem->getText();
                std::string val = (name == "true") ? "1" : "0";
                ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, val, std::to_string(k), var_cls);
                k++;
            }
        }
        else
            throw std::runtime_error("\nunsupport variable type xxx");
        if (k > length)
            throw std::runtime_error("\ninit array out of bound\n");
        while (k < length)
        {
            ir.addIMC(var_name, OP::GLOBAL_ASSIGN_IDX, "0", std::to_string(k), var_cls);
            k++;
        }
    }
}
void IRListener::exitDefInitArray(CACTParser::DefInitArrayContext *ctx) {}

void IRListener::enterAssignStmt(CACTParser::AssignStmtContext *ctx) {}
void IRListener::exitAssignStmt(CACTParser::AssignStmtContext *ctx)
{
    //  lVal '=' exp ';'
    bool elemwise = false;
    bool isArray = false;
    std::string index;
    std::string LVAL = ctx->lVal()->tmp;
    if (ctx->lVal()->exp()) //LHS is:  array[index]
    {
        index = ctx->lVal()->exp()->tmp;
        auto *index_var = sym_table.lookup(index);
        if (index_var->cls != CLASS_INT)
            throw std::runtime_error("\ninvalid index at line: " + std::to_string(ctx->getStart()->getLine()));
        isArray = true;
    }
    //lhs_var
    auto *lhs_var = sym_table.lookup(LVAL); //LHS
    if (lhs_var == nullptr)                 //检查左符号是否被定义
    {
        throw std::runtime_error("\nundefined var at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    auto rval = ctx->exp()->tmp;
    //rhs_var
    auto *rval_var = sym_table.lookup(rval); //RHS
    if (lhs_var->type == TYPE_ARRAY &&
        (rval_var->type == TYPE_ARRAY || rval_var->type == TYPE_CONST_ARRAY)) //判断是否为数组element wise
    {
        elemwise = true;
    }
    int var_cls = lhs_var->cls;
    int var_type = lhs_var->type;
    //do type check here
    if (var_type == TYPE_CONST || var_type == TYPE_CONST_ARRAY)
    {
        throw std::runtime_error("\ncannot assign const at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    else
    {
        if (ctx->exp()->cls == CLASS_VOID)
            throw std::runtime_error("\nError: void func return type at line: " + std::to_string(ctx->getStart()->getLine()));
        if (rval_var->cls != var_cls)
        {
            throw std::runtime_error("\nCACT cannot cast type at line: " + std::to_string(ctx->getStart()->getLine()));
        }
        //finish type check
        if (!isArray && !elemwise) //normal case
        {
            ir.addIMC("t2", OP::LOAD, sym_table.parse_local_to_offset(rval_var), VOID, var_cls);
            ir.addIMC("t2", OP::STORE, sym_table.parse_local_to_offset(lhs_var), VOID, var_cls);
        }
        else if (elemwise) //A = exp(exp is array element-wise arithmetic result)
        {
            /* do all array check here */
            if (lhs_var->type != TYPE_ARRAY)
                throw std::runtime_error("\ninvalid array element-wise assignment at line: " + std::to_string(ctx->getStart()->getLine()));
            if (lhs_var->length != rval_var->length)
                throw std::runtime_error("\nunmatch size for array element-wise assignment at line: " + std::to_string(ctx->getStart()->getLine()));

            auto rhs_str = sym_table.parse_local_to_offset(rval_var);
            auto lhs_str = sym_table.parse_local_to_offset(lhs_var);
            for (int i = 0; i < lhs_var->length; i++)
            {
                ir.addIMC("t3", OP::LI, std::to_string(i), VOID, CLASS_INT);
                ir.addIMC("t2", OP::READ_ARR, rhs_str, "t3", var_cls);
                ir.addIMC("t2", OP::SAVE_ARR, lhs_str, "t3", var_cls);
            }
        }
        else //array[index] = exp
        {
            auto array_name = ctx->lVal()->Ident()->getText();
            auto array_info = sym_table.lookup(array_name);
            auto array_str = sym_table.parse_local_to_offset(array_info);
            auto index_var = sym_table.lookup(index);
            auto index_str = sym_table.parse_local_to_offset(index_var);
            ir.addIMC("t2", OP::LOAD, sym_table.parse_local_to_offset(rval_var), VOID, CLASS_INT);
            ir.addIMC("t4", OP::LOAD, index_str, VOID, CLASS_INT);
            ir.addIMC("t2", OP::SAVE_ARR, array_str, "t4", var_cls);
        }
    }
}

void IRListener::enterExp(CACTParser::ExpContext *ctx) {}
void IRListener::exitExp(CACTParser::ExpContext *ctx)
{
    if (ctx->addExp())
    {
        ctx->cls = ctx->addExp()->cls;
        ctx->tmp = ctx->addExp()->tmp;
    }
}

void IRListener::enterAddExp(CACTParser::AddExpContext *ctx) {}

void IRListener::exitAddExp(CACTParser::AddExpContext *ctx)
{
    int cls = ctx->mulExp()->cls;
    if (!ctx->addExp()) // case: addexp -> mulexp
    {
        ctx->tmp = ctx->mulExp()->tmp;
        ctx->cls = ctx->mulExp()->cls;
        ctx->elemwise = ctx->mulExp()->elemwise; //not used after all
    }
    else //case: addexp -> addexp (+/-) mulexp
    {
        if (ctx->addExp()->cls != cls) //类型检查
            throw std::runtime_error("\nCACT cannot cast type at line: " + std::to_string(ctx->getStart()->getLine()));
        auto op1 = ctx->addExp()->tmp;
        auto op2 = ctx->mulExp()->tmp;
        auto *ARR1 = sym_table.lookup(op1);
        auto *ARR2 = sym_table.lookup(op2);
        if ((ARR1->type == TYPE_ARRAY || ARR1->type == TYPE_CONST_ARRAY) &&
            (ARR2->type == TYPE_ARRAY || ARR2->type == TYPE_CONST_ARRAY)) //Array element wise op
        {                                                                 //  E.g:    A + B   , both A, B are array of the same size
            if (ARR1->length != ARR2->length)                             //check size
            {
                throw std::runtime_error("\ndifferent size error at line: " + std::to_string(ctx->getStart()->getLine()));
            }
            if (ARR1->cls != ARR2->cls) //check class
            {
                throw std::runtime_error("\ndifferent class error at line: " + std::to_string(ctx->getStart()->getLine()));
            }

            auto res = ir.gen_temp_array(ctx->getStart()->getLine(), ARR1->cls, ARR1->length, sym_table);
            auto op1_info = sym_table.lookup(op1);
            auto op2_info = sym_table.lookup(op2);
            auto res_info = sym_table.lookup(res);
            auto op1_str = sym_table.parse_local_to_offset(op1_info);
            auto op2_str = sym_table.parse_local_to_offset(op2_info);
            auto res_str = sym_table.parse_local_to_offset(res_info);
            //读取基址
            if (ctx->op->getType() == CACTLexer::ADD) //+
            {

                for (int i = 0; i < ARR1->length; i++)
                {
                    ir.addIMC("t4", OP::LI, std::to_string(i), VOID, CLASS_INT);
                    ir.addIMC("t1", OP::READ_ARR, op1_str, "t4", cls);
                    ir.addIMC("t2", OP::READ_ARR, op2_str, "t4", cls);
                    ir.addIMC("t3", OP::ADD, "t1", "t2", cls); //+
                    ir.addIMC("t3", OP::SAVE_ARR, res_str, "t4", cls);
                }
            }
            else //-
            {
                for (int i = 0; i < ARR1->length; i++)
                {
                    ir.addIMC("t4", OP::LI, std::to_string(i), VOID, CLASS_INT);
                    ir.addIMC("t1", OP::READ_ARR, op1_str, "t4", cls);
                    ir.addIMC("t2", OP::READ_ARR, op2_str, "t4", cls);
                    ir.addIMC("t3", OP::SUB, "t1", "t2", cls); //-
                    ir.addIMC("t3", OP::SAVE_ARR, res_str, "t4", cls);
                }
            }
            ctx->elemwise = true;
            ctx->tmp = res;
        }
        else //normal Scalar op
        {
            std::string op1 = ctx->addExp()->tmp;
            std::string op2 = ctx->mulExp()->tmp;
            std::string res;
            auto *var1 = sym_table.lookup(op1);
            auto *var2 = sym_table.lookup(op2);
            //both var aren't array type
            if (var1->type == TYPE_ARRAY || var1->type == TYPE_CONST_ARRAY ||
                var2->type == TYPE_ARRAY || var2->type == TYPE_CONST_ARRAY)
            { //E.g:  A + A[1]
                throw std::runtime_error("\nInvalid op with array and scalar at line: " + std::to_string(ctx->getStart()->getLine()));
            }

            if (cls == CLASS_INT)
            {
                res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_INT, sym_table);
            }
            else if (cls == CLASS_DOUBLE)
            {
                res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_DOUBLE, sym_table);
            }
            else if (cls == CLASS_FLOAT)
            {
                res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_FLOAT, sym_table);
            }
            else if (cls == CLASS_BOOLEAN)
            {
                //bool type doesn't support +/- op
                throw std::runtime_error("\nCACT doesn't support bool op at line: " + std::to_string(ctx->getStart()->getLine()));
            }

            auto res_info = sym_table.lookup(res);
            auto res_str = sym_table.parse_local_to_offset(res_info);

            //+
            if (ctx->op->getType() == CACTLexer::ADD)
            {
                //ir.addIMC(res_str, OP::ADD, sym_table.parse_local_to_offset(var1), sym_table.parse_local_to_offset(var2),cls);
                ir.addIMC("t0", OP::LOAD, sym_table.parse_local_to_offset(var1), VOID, cls);
                ir.addIMC("t1", OP::LOAD, sym_table.parse_local_to_offset(var2), VOID, cls);
                ir.addIMC("t2", OP::ADD, "t0", "t1", cls);
                ir.addIMC("t2", OP::STORE, res_str, VOID, cls);
            }
            //-
            else if (ctx->op->getType() == CACTLexer::SUB)
            {
                ir.addIMC("t0", OP::LOAD, sym_table.parse_local_to_offset(var1), VOID, cls);
                ir.addIMC("t1", OP::LOAD, sym_table.parse_local_to_offset(var2), VOID, cls);
                ir.addIMC("t2", OP::SUB, "t0", "t1", cls);
                ir.addIMC("t2", OP::STORE, res_str, VOID, cls);
            }
            ctx->tmp = res;
            ctx->cls = cls;
        }
    }
}

void IRListener::enterMulExp(CACTParser::MulExpContext *ctx) {}
void IRListener::exitMulExp(CACTParser::MulExpContext *ctx)
{
    int cls = ctx->unaryExp()->cls;
    if (!ctx->mulExp()) // case: mulexp ->unaryexp
    {
        ctx->cls = ctx->unaryExp()->cls;
        if (ctx->cls == CLASS_VOID) //only void funcall return void here
        {
            ctx->tmp = "NULL"; //just give a name here
        }
        else
            ctx->tmp = ctx->unaryExp()->tmp;
    }
    else //case: mulexp -> mulexp (* /) unaryexp
    {
        if (ctx->mulExp()->cls != cls) //类型检查
            throw std::runtime_error("\nCACT cannot cast type at line: " + std::to_string(ctx->getStart()->getLine()));
        auto op1 = ctx->mulExp()->tmp;
        auto op2 = ctx->unaryExp()->tmp;
        auto *ARR1 = sym_table.lookup(op1);
        auto *ARR2 = sym_table.lookup(op2);

        if ((ARR1->type == TYPE_ARRAY || ARR1->type == TYPE_CONST_ARRAY) &&
            (ARR2->type == TYPE_ARRAY || ARR2->type == TYPE_CONST_ARRAY))
        { //Array element wise op

            if (ARR1->length != ARR2->length) //check size
            {
                throw std::runtime_error("\ndifferent size error at line: " + std::to_string(ctx->getStart()->getLine()));
            }
            if (ARR1->cls != ARR2->cls) //check class
            {
                throw std::runtime_error("\ndifferent class error at line: " + std::to_string(ctx->getStart()->getLine()));
            }

            auto res = ir.gen_temp_array(ctx->getStart()->getLine(), ARR1->cls, ARR1->length, sym_table);
            auto op1_info = sym_table.lookup(op1);
            auto op2_info = sym_table.lookup(op2);
            auto res_info = sym_table.lookup(res);
            auto op1_str = sym_table.parse_local_to_offset(op1_info);
            auto op2_str = sym_table.parse_local_to_offset(op2_info);
            auto res_str = sym_table.parse_local_to_offset(res_info);
            if (ctx->op->getType() == CACTLexer::MUL) // *
            {
                for (int i = 0; i < ARR1->length; i++)
                {
                    ir.addIMC("t4", OP::LI, std::to_string(i), VOID, CLASS_INT);
                    ir.addIMC("t1", OP::READ_ARR, op1_str, "t4", cls);
                    ir.addIMC("t2", OP::READ_ARR, op2_str, "t4", cls);
                    ir.addIMC("t3", OP::MUL, "t1", "t2", cls); //*
                    ir.addIMC("t3", OP::SAVE_ARR, res_str, "t4", cls);
                }
            }
            else if (ctx->op->getType() == CACTLexer::DIV) // /
            {
                for (int i = 0; i < ARR1->length; i++)
                {
                    ir.addIMC("t4", OP::LI, std::to_string(i), VOID, CLASS_INT);
                    ir.addIMC("t1", OP::READ_ARR, op1_str, "t4", cls);
                    ir.addIMC("t2", OP::READ_ARR, op2_str, "t4", cls);
                    ir.addIMC("t3", OP::DIV, "t1", "t2", cls); //*
                    ir.addIMC("t3", OP::SAVE_ARR, res_str, "t4", cls);
                }
            }
            else // %
            {
                for (int i = 0; i < ARR1->length; i++)
                {
                    ir.addIMC("t4", OP::LI, std::to_string(i), VOID, CLASS_INT);
                    ir.addIMC("t1", OP::READ_ARR, op1_str, "t4", cls);
                    ir.addIMC("t2", OP::READ_ARR, op2_str, "t4", cls);
                    ir.addIMC("t3", OP::MOD, "t1", "t2", cls); //%
                    ir.addIMC("t3", OP::SAVE_ARR, res_str, "t4", cls);
                }
            }
            ctx->elemwise = true;
            ctx->tmp = res;
        }
        else //normal Scalar op
        {
            //ctx->addExp()->getText()
            std::string op1 = ctx->mulExp()->tmp;
            std::string op2 = ctx->unaryExp()->tmp;
            std::string res;

            auto *var1 = sym_table.lookup(op1);
            auto *var2 = sym_table.lookup(op2);
            //both var aren't array type
            if (var1->type == TYPE_ARRAY || var1->type == TYPE_CONST_ARRAY ||
                var2->type == TYPE_ARRAY || var2->type == TYPE_CONST_ARRAY)
            { //E.g:  A * A[1]
                throw std::runtime_error("\nInvalid op with array and scalar at line: " + std::to_string(ctx->getStart()->getLine()));
            }
            if (cls == CLASS_INT)
            {
                res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_INT, sym_table);
            }
            else if (cls == CLASS_DOUBLE)
            {
                res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_DOUBLE, sym_table);
            }
            else if (cls == CLASS_FLOAT)
            {
                res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_FLOAT, sym_table);
            }
            else if (cls == CLASS_BOOLEAN)
            {
                //bool type doesn't support * / % op
                throw std::runtime_error("\nCACT doesn't support bool op at line: " + std::to_string(ctx->getStart()->getLine()));
            }
            auto res_info = sym_table.lookup(res);
            auto res_str = sym_table.parse_local_to_offset(res_info);
            // *
            if (ctx->op->getType() == CACTLexer::MUL)
            {
                //ir.addIMC(res_str, OP::MUL, sym_table.parse_local_to_offset(var1), sym_table.parse_local_to_offset(var2),cls);
                ir.addIMC("t0", OP::LOAD, sym_table.parse_local_to_offset(var1), VOID, cls);
                ir.addIMC("t1", OP::LOAD, sym_table.parse_local_to_offset(var2), VOID, cls);
                ir.addIMC("t2", OP::MUL, "t0", "t1", cls);
                ir.addIMC("t2", OP::STORE, res_str, VOID, cls);
            }
            // /
            else if (ctx->op->getType() == CACTLexer::DIV)
            {
                //ir.addIMC(res_str, OP::DIV, sym_table.parse_local_to_offset(var1), sym_table.parse_local_to_offset(var2),cls);
                ir.addIMC("t0", OP::LOAD, sym_table.parse_local_to_offset(var1), VOID, cls);
                ir.addIMC("t1", OP::LOAD, sym_table.parse_local_to_offset(var2), VOID, cls);
                ir.addIMC("t2", OP::DIV, "t0", "t1", cls);
                ir.addIMC("t2", OP::STORE, res_str, VOID, cls);
            }
            // %
            else if (ctx->op->getType() == CACTLexer::MOD)
            {
                //ir.addIMC(res_str, OP::MOD, sym_table.parse_local_to_offset(var1), sym_table.parse_local_to_offset(var2),cls);
                ir.addIMC("t0", OP::LOAD, sym_table.parse_local_to_offset(var1), VOID, cls);
                ir.addIMC("t1", OP::LOAD, sym_table.parse_local_to_offset(var2), VOID, cls);
                ir.addIMC("t2", OP::MOD, "t0", "t1", cls);
                ir.addIMC("t2", OP::STORE, res_str, VOID, cls);
            }
            ctx->tmp = res;
            ctx->cls = cls;
        }
    }
}

void IRListener::enterPrimary(CACTParser::PrimaryContext *ctx) {}
void IRListener::exitPrimary(CACTParser::PrimaryContext *ctx)
{
    if (ctx->primaryExp()->exp() != nullptr) //primaryexp -> ( exp )
    {
        ctx->tmp = ctx->primaryExp()->exp()->tmp;
        ctx->cls = ctx->primaryExp()->exp()->cls;
    }
    else
    {
        if (ctx->primaryExp()->lVal() != nullptr)
        {
            ctx->tmp = ctx->primaryExp()->lVal()->tmp;
            ctx->cls = ctx->primaryExp()->lVal()->cls;
        }
        else if (ctx->getText() == "true" || ctx->getText() == "false")
        { //bool
            ctx->cls = CLASS_BOOLEAN;
            std::string temp;
            temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_BOOLEAN, sym_table);
            auto temp_info = sym_table.lookup(temp);
            int value = (ctx->getText() == "true") ? 1 : 0;
            ir.addIMC("t2", OP::LI, std::to_string(value), VOID, CLASS_BOOLEAN);
            ir.addIMC("t2", OP::STORE, sym_table.parse_local_to_offset(temp_info), VOID, CLASS_BOOLEAN);
            //ir.addIMC(sym_table.parse_local_to_offset(temp_info), OP::ASSIGN, std::to_string(value), VOID,CLASS_BOOLEAN);
            ctx->tmp = temp;
        }
        else //number
        {
            int num_cls = ctx->primaryExp()->getStart()->getType();
            std::string temp;
            std::string number = ctx->getText();
            if (num_cls == CACTLexer::IntConst)
            {
                ctx->cls = CLASS_INT;
                temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_INT, sym_table);
                VarInfo *var = sym_table.lookup(temp);
                int value = std::stoi(number);
                number = std::to_string(value);
                ir.addIMC("t2", OP::LI, number, VOID, CLASS_INT);
                ir.addIMC("t2", OP::STORE, sym_table.parse_local_to_offset(var), VOID, CLASS_INT);
                //ir.addIMC(sym_table.parse_local_to_offset(var), OP::ASSIGN, number, VOID,CLASS_INT);
                ctx->tmp = temp;
            }
            else if (num_cls == CACTLexer::FloatConst)
            {
                ctx->cls = CLASS_FLOAT;
                temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_FLOAT, sym_table);
                VarInfo *var = sym_table.lookup(temp);
                float value = std::stof(number.substr(0, number.size() - 1));
                number = std::to_string(value);
                ir.addIMC("t2", OP::LI, number, VOID, CLASS_FLOAT);
                ir.addIMC("t2", OP::STORE, sym_table.parse_local_to_offset(var), VOID, CLASS_FLOAT);
                //ir.addIMC(sym_table.parse_local_to_offset(var), OP::ASSIGN, number, VOID,CLASS_FLOAT);
                //need to add it to rodata
                ctx->tmp = temp;
            }
            else if (num_cls == CACTLexer::DoubleConst)
            {
                ctx->cls = CLASS_DOUBLE;
                temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_DOUBLE, sym_table);
                VarInfo *var = sym_table.lookup(temp);
                double value = std::stod(number);
                number = std::to_string(value);
                ir.addIMC("t2", OP::LI, number, VOID, CLASS_DOUBLE);
                ir.addIMC("t2", OP::STORE, sym_table.parse_local_to_offset(var), VOID, CLASS_DOUBLE);
                ctx->tmp = temp;
            }
        }
    }
}

void IRListener::enterFuncall(CACTParser::FuncallContext *ctx)
{
    auto func_name = ctx->Ident()->getText();
    int param_num = 0;
    if (ctx->funcRParams())
        param_num = ctx->funcRParams()->exp().size();
    auto func_info = sym_table.lookup_func(func_name);
    if (!func_info)
    {
        throw std::runtime_error("\nundefine function at line: " + std::to_string(ctx->getStart()->getLine()));
    }
    if (param_num != func_info->num_params)
    { // 参数数量不一致
        throw std::runtime_error("\nparam num mismatch at line: " + std::to_string(ctx->getStart()->getLine()));
    }
}
void IRListener::exitFuncall(CACTParser::FuncallContext *ctx)
{
    auto func_name = ctx->Ident()->getText();
    auto func_info = sym_table.lookup_func(func_name);
    int cnt = 0;
    if (ctx->funcRParams()) //有参数
    {
        sym_table.pseudo_stack.push_back(&func_info->base_block);
        for (auto &param : ctx->funcRParams()->exp())
        {
            sym_table.pseudo_stack.pop_back();
            auto temp_info = sym_table.lookup(param->tmp);
            auto cls = temp_info->cls;
            auto str = sym_table.parse_local_to_offset(temp_info);
            std::string head = "t";
            std::string head1 = "a";
            ir.addIMC(head + std::to_string(cnt), OP::LOAD, str, VOID, cls);
            ir.addIMC(head1 + std::to_string(cnt), OP::MOV, head + std::to_string(cnt), VOID, cls);
            sym_table.pseudo_stack.push_back(&func_info->base_block);
            //逐个检查实参形参是否类型对应
            auto *para_info = sym_table.lookup_param(*func_info, cnt); //形参
            if (para_info->cls != param->cls)                          //形参类型与实参类型
                throw std::runtime_error("unmatch param type at line: " + std::to_string(ctx->getStart()->getLine()));
            cnt++;
        }
        sym_table.pseudo_stack.pop_back();
    }
    ir.addIMC(func_name, OP::CALL, VOID, VOID, CLASS_VOID);
    if (func_info->cls == CLASS_VOID)
    {
        ctx->cls = CLASS_VOID;
    }
    else
    {
        auto ret_var = ir.gen_temp(ctx->getStart()->getLine(), func_info->cls, sym_table);
        auto ret_info = sym_table.lookup(ret_var);
        auto str = sym_table.parse_local_to_offset(ret_info);
        ir.addIMC("t2", OP::MOV, "a0", VOID, ret_info->cls);
        ir.addIMC("t2", OP::STORE, str, VOID, ret_info->cls);
        ctx->cls = func_info->cls;
        ctx->tmp = ret_var;
    }
}

void IRListener::enterUnary(CACTParser::UnaryContext *ctx) {}
void IRListener::exitUnary(CACTParser::UnaryContext *ctx)
{
    std::string temp;
    if (ctx->op->getType() == CACTLexer::NOT)
    {
        if (ctx->unaryExp()->cls != CLASS_BOOLEAN)
            throw std::runtime_error("\ninvalid '!' op at line: " + std::to_string(ctx->getStart()->getLine()));
        ctx->cls = CLASS_BOOLEAN;
        std::string childtemp = ctx->unaryExp()->tmp;
        temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_BOOLEAN, sym_table);
        auto temp_info = sym_table.lookup(temp);
        auto str = sym_table.parse_local_to_offset(temp_info);
        auto child_temp_info = sym_table.lookup(childtemp);
        auto child_str = sym_table.parse_local_to_offset(child_temp_info);
        /*  SHOULD be done in lower level
        VarInfo *childvar= sym_table.lookup(childtemp);
        VarInfo *var = sym_table.lookup(temp);
        bool origin= *(bool *)childvar->addr;
        *(bool *)var->addr = !origin;*/
        ir.addIMC(str, OP::NOT, child_str, VOID, CLASS_BOOLEAN);
        ctx->tmp = temp;
    }
    else if (ctx->op->getType() == CACTLexer::ADD)
    {
        ctx->cls = ctx->unaryExp()->cls;
        if (ctx->cls == CLASS_BOOLEAN)
            throw std::runtime_error("\ninvalid '+' op at line: " + std::to_string(ctx->getStart()->getLine()));
        ctx->tmp = ctx->unaryExp()->tmp;
    }
    else if (ctx->op->getType() == CACTLexer::SUB)
    {
        ctx->cls = ctx->unaryExp()->cls;
        if (ctx->cls == CLASS_INT)
        {
            temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_INT, sym_table);
            auto info = sym_table.lookup(temp);
            auto str = sym_table.parse_local_to_offset(info);
            auto child_info = sym_table.lookup(ctx->unaryExp()->tmp);
            auto child_str = sym_table.parse_local_to_offset(child_info);
            ir.addIMC("t0", OP::LOAD, child_str, VOID, CLASS_INT);
            ir.addIMC("t2", OP::NEG, "t0", VOID, CLASS_INT); 
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_INT);
            ctx->tmp = temp;
        }
        else if (ctx->cls == CLASS_DOUBLE)
        {
            temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_DOUBLE, sym_table);
            auto info = sym_table.lookup(temp);
            auto str = sym_table.parse_local_to_offset(info);
            auto child_info = sym_table.lookup(ctx->unaryExp()->tmp);
            auto child_str = sym_table.parse_local_to_offset(child_info);
            ir.addIMC("t0", OP::LI, "0", VOID, CLASS_DOUBLE);
            ir.addIMC("t1", OP::LOAD, child_str, VOID, CLASS_DOUBLE);
            ir.addIMC("t2", OP::SUB, "t0", "t1", CLASS_DOUBLE); 
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_DOUBLE);
            ctx->tmp = temp;
        }
        else if (ctx->cls == CLASS_FLOAT)
        {
            temp = ir.gen_temp(ctx->getStart()->getLine(), CLASS_FLOAT, sym_table);
            auto info = sym_table.lookup(temp);
            auto str = sym_table.parse_local_to_offset(info);
            auto child_info = sym_table.lookup(ctx->unaryExp()->tmp);
            auto child_str = sym_table.parse_local_to_offset(child_info);
            ir.addIMC("t0", OP::LI, "0", VOID, CLASS_FLOAT);
            ir.addIMC("t1", OP::LOAD, child_str, VOID, CLASS_FLOAT);
            ir.addIMC("t2", OP::SUB, "t0", "t1", CLASS_FLOAT); 
            ir.addIMC("t2", OP::STORE, str, VOID, CLASS_FLOAT);
            ctx->tmp = temp;
        }
        else
        {
            throw std::runtime_error("\ninvalid '-' op at line: " + std::to_string(ctx->getStart()->getLine()));
        }
    }
}

void IRListener::enterLVal(CACTParser::LValContext *ctx) {}
void IRListener::exitLVal(CACTParser::LValContext *ctx)
{
    if (ctx->exp() == nullptr) //Ident
    {
        std::string var_name = ctx->Ident()->getText();
        VarInfo *var = sym_table.lookup(var_name);
        if (!var)
            throw std::runtime_error("\nUndefined variable: " + var_name + "\n");
        ctx->cls = var->cls;
        ctx->tmp = ctx->Ident()->getText();
    }
    else //Ident[exp]
    {
        VarInfo *array = sym_table.lookup(ctx->Ident()->getText());
        ctx->cls = array->cls;

        if (!(array->type == TYPE_ARRAY || array->type == TYPE_CONST_ARRAY))
            throw std::runtime_error("\nid not array at line: " + std::to_string(ctx->getStart()->getLine()));
        auto idx = ctx->exp()->tmp;
        auto temp = sym_table.lookup(idx);
        auto temp_str = sym_table.parse_local_to_offset(temp);
        if (temp->cls != CLASS_INT)
            throw std::runtime_error("\ninvalid index class at line: " + std::to_string(ctx->getStart()->getLine()));
        std::string arrayelem = ir.gen_temp(ctx->getStart()->getLine(), ctx->cls, sym_table);
        //ir.addIMC(arrayelem, OP::READ_ARR, ctx->Ident()->getText(), idx, array->cls);
        //arrayelem=A[b[1]];
        auto array_name = ctx->Ident()->getText();
        auto array_info = sym_table.lookup(array_name);
        auto array_str = sym_table.parse_local_to_offset(array_info);
        auto arrayelem_info = sym_table.lookup(arrayelem);
        auto arrayelem_str = sym_table.parse_local_to_offset(arrayelem_info);
        ir.addIMC("t1", OP::LOAD, temp_str, VOID, CLASS_INT /*array->cls*/);
        ir.addIMC("t2", OP::READ_ARR, array_str, "t1", array->cls);
        ir.addIMC("t2", OP::STORE, arrayelem_str, VOID, array->cls);
        ctx->tmp = arrayelem;
    }
}

void IRListener::enterReturnStmt(CACTParser::ReturnStmtContext *ctx) {}
void IRListener::exitReturnStmt(CACTParser::ReturnStmtContext *ctx)
{
    if (ctx->exp() != nullptr)
    {
        int cls = ctx->exp()->cls;
        std::string temp = ctx->exp()->tmp;
        VarInfo *true_return_value = sym_table.lookup(temp);
        auto str = sym_table.parse_local_to_offset(true_return_value);

        int depth = sym_table.pseudo_stack.size();
        std::string curr_func = sym_table.cur_func;

        FuncInfo *func = sym_table.lookup_func(curr_func);
        int func_cls = func->cls;
        if (cls - func_cls != 0) //fixed
        {
            throw std::runtime_error("\nwrong return type at line: " + std::to_string(ctx->getStart()->getLine()));
        }
        ir.addIMC("t2", OP::LOAD, str, VOID, cls);
        ir.addIMC("a0", OP::MOV, "t2", VOID, cls);
        ir.addIMC(VOID, OP::RET, VOID, VOID, CLASS_VOID);
    }
    else //return void
    {
        int depth = sym_table.pseudo_stack.size();
        std::string curr_func = sym_table.cur_func;

        FuncInfo *func = sym_table.lookup_func(curr_func);
        if (func->cls == CLASS_VOID)
            ir.addIMC(VOID, OP::RET, VOID, VOID, CLASS_VOID);
        else
        {
            throw std::runtime_error("\nexpect return void at line: " + std::to_string(ctx->getStart()->getLine()));
        }
    }
}

void IRListener::enterRelExp(CACTParser::RelExpContext *ctx) {}
void IRListener::exitRelExp(CACTParser::RelExpContext *ctx)
{
    if (ctx->BoolConst()) //relExp->boolconst
    {
        ctx->val = ctx->BoolConst()->getText() == "true" ? true : false;
        ctx->cls = CLASS_BOOLEAN;
    }
    else if (ctx->relExp()) //relExp->relExp op addExp
    {
        //relOp seems only working on int double or float, not boolean
        //so ignore relExp()->type = bool case
        std::string name1 = ctx->relExp()->tmp;
        std::string name2 = ctx->addExp()->tmp;

        VarInfo *var1 = sym_table.lookup(name1);
        VarInfo *var2 = sym_table.lookup(name2);
        auto str1 = sym_table.parse_local_to_offset(var1);
        auto str2 = sym_table.parse_local_to_offset(var2);

        std::string res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_BOOLEAN, sym_table);
        auto res_info = sym_table.lookup(res);
        auto str_res = sym_table.parse_local_to_offset(res_info);
        //class check
        if (var1->cls != var2->cls)
        {
            throw std::runtime_error("\nerror relExp class at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //type check
        if (!(var1->type == TYPE_VAR || var1->type == TYPE_CONST || var1->type == TYPE_PARAM) ||
            !(var2->type == TYPE_VAR || var2->type == TYPE_CONST || var2->type == TYPE_PARAM))
        {
            throw std::runtime_error("\nerror relExp type at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //codeGen
        if (ctx->op->getType() == CACTLexer::GT) //>
        {
            ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
            ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
            ir.addIMC("t2", OP::GT, "t0", "t1", var1->cls);
            ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        }
        else if (ctx->op->getType() == CACTLexer::GE) //>=
        {
            ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
            ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
            ir.addIMC("t2", OP::GE, "t0", "t1", var1->cls);
            ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        }
        else if (ctx->op->getType() == CACTLexer::LE) //<=
        {
            ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
            ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
            ir.addIMC("t2", OP::LE, "t0", "t1", var1->cls);
            ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        }
        else if (ctx->op->getType() == CACTLexer::LT) //<
        {
            ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
            ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
            ir.addIMC("t2", OP::LT, "t0", "t1", var1->cls);
            ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        }
        ctx->cls = CLASS_BOOLEAN;
        ctx->tmp = res;
    }
    else //relExp->addExp
    {
        ctx->cls = ctx->addExp()->cls;

        ctx->tmp = ctx->addExp()->tmp;
    }
}

void IRListener::enterEqExp(CACTParser::EqExpContext *ctx) {}
void IRListener::exitEqExp(CACTParser::EqExpContext *ctx)
{
    if (ctx->eqExp()) //eqExp op=(EQUAL | NOTEQUAL) relExp
    {
        std::string name1 = ctx->eqExp()->tmp;
        std::string name2 = ctx->relExp()->tmp;

        VarInfo *var1 = sym_table.lookup(name1);
        VarInfo *var2 = sym_table.lookup(name2);
        auto str1 = sym_table.parse_local_to_offset(var1);
        auto str2 = sym_table.parse_local_to_offset(var2);
        std::string res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_BOOLEAN, sym_table);
        auto res_info = sym_table.lookup(res);
        auto str_res = sym_table.parse_local_to_offset(res_info);
        //class check
        if (var1->cls != var2->cls)
        {
            throw std::runtime_error("\nerror eqExp class at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //type check
        if (!(var1->type == TYPE_VAR || var1->type == TYPE_CONST || var1->type == TYPE_PARAM) ||
            !(var2->type == TYPE_VAR || var2->type == TYPE_CONST || var2->type == TYPE_PARAM))
        {
            throw std::runtime_error("\nerror eqExp type at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        if (ctx->op->getType() == CACTLexer::EQUAL) //==
        {
            //ir.addIMC(res, OP::EQU, str1, str2,var1->cls);
            ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
            ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
            ir.addIMC("t2", OP::EQU, "t0", "t1", var1->cls);
            ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        }
        else if (ctx->op->getType() == CACTLexer::NOTEQUAL) //!=
        {
            //ir.addIMC(res, OP::NEQ, str1, str2,var1->cls);
            ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
            ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
            ir.addIMC("t2", OP::NEQ, "t0", "t1", var1->cls);
            ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        }
        ctx->cls = CLASS_BOOLEAN;
        ctx->tmp = res;
    }
    else //relExp
    {
        ctx->tmp = ctx->relExp()->tmp;
        ctx->cls = ctx->relExp()->cls;
    }
}

void IRListener::enterLAndExp(CACTParser::LAndExpContext *ctx) {}
void IRListener::exitLAndExp(CACTParser::LAndExpContext *ctx)
{
    if (ctx->lAndExp()) //lAndExp -> lAndExp eqExp
    {
        std::string name1 = ctx->lAndExp()->tmp;
        std::string name2 = ctx->eqExp()->tmp;

        VarInfo *var1 = sym_table.lookup(name1);
        VarInfo *var2 = sym_table.lookup(name2);
        auto str1 = sym_table.parse_local_to_offset(var1);
        auto str2 = sym_table.parse_local_to_offset(var2);
        std::string res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_BOOLEAN, sym_table);
        auto res_info = sym_table.lookup(res);
        auto str_res = sym_table.parse_local_to_offset(res_info);
        //class check
        if (var1->cls != var2->cls)
        {
            throw std::runtime_error("\nerror AndExp class at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //type check
        if (!(var1->type == TYPE_VAR || var1->type == TYPE_CONST) || !(var2->type == TYPE_VAR || var2->type == TYPE_CONST))
        {
            throw std::runtime_error("\nerror AndExp type at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //ir.addIMC(res, OP::AND, str1, str2,CLASS_BOOLEAN);
        ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
        ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
        ir.addIMC("t2", OP::AND, "t0", "t1", CLASS_BOOLEAN);
        ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        ctx->cls = CLASS_BOOLEAN;
        ctx->tmp = res;
    }
    else //lAndExp -> eqExp
    {
        ctx->tmp = ctx->eqExp()->tmp;
        ctx->cls = ctx->eqExp()->cls;
    }
}

void IRListener::enterLOrExp(CACTParser::LOrExpContext *ctx) {}
void IRListener::exitLOrExp(CACTParser::LOrExpContext *ctx)
{
    if (ctx->lOrExp()) //lOrExp -> lOrExp lAndExp
    {
        std::string name1 = ctx->lOrExp()->tmp;
        std::string name2 = ctx->lAndExp()->tmp;

        VarInfo *var1 = sym_table.lookup(name1);
        VarInfo *var2 = sym_table.lookup(name2);
        auto str1 = sym_table.parse_local_to_offset(var1);
        auto str2 = sym_table.parse_local_to_offset(var2);
        std::string res = ir.gen_temp(ctx->getStart()->getLine(), CLASS_BOOLEAN, sym_table);
        auto res_info = sym_table.lookup(res);
        auto str_res = sym_table.parse_local_to_offset(res_info);
        //class check
        if (var1->cls != var2->cls)
        {
            throw std::runtime_error("\nerror OrExp class at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //type check
        if (!(var1->type == TYPE_VAR || var1->type == TYPE_CONST) || !(var2->type == TYPE_VAR || var2->type == TYPE_CONST))
        {
            throw std::runtime_error("\nerror OrExp type at line:" + std::to_string(ctx->getStart()->getLine()));
        }
        //ir.addIMC(res, OP::OR, str1, str2,CLASS_BOOLEAN);
        ir.addIMC("t0", OP::LOAD, str1, VOID, var1->cls);
        ir.addIMC("t1", OP::LOAD, str2, VOID, var2->cls);
        ir.addIMC("t2", OP::OR, "t0", "t1", CLASS_BOOLEAN);
        ir.addIMC("t2", OP::STORE, str_res, VOID, CLASS_BOOLEAN);
        ctx->cls = CLASS_BOOLEAN;
        ctx->tmp = res;
    }
    else //lOrExp -> lAndExp
    {
        ctx->tmp = ctx->lAndExp()->tmp;
        ctx->cls = ctx->lAndExp()->cls;
    }
}

void IRListener::enterCond(CACTParser::CondContext *ctx) {}
void IRListener::exitCond(CACTParser::CondContext *ctx)
{
    ctx->tmp = ctx->lOrExp()->tmp;
    ctx->cls = ctx->lOrExp()->cls;
    if (ctx->cls != CLASS_BOOLEAN)
    {
        throw std::runtime_error("\nerror Cond type at line:" + std::to_string(ctx->getStart()->getLine()));
    }
}

void IRListener::enterIfCond(CACTParser::IfCondContext *ctx) {}
void IRListener::exitIfCond(CACTParser::IfCondContext *ctx)
{
    ctx->tmp = ctx->cond()->tmp;
    ctx->cls = ctx->cond()->cls;

    auto labels = if_labels.get(ctx->parent);
    auto info = sym_table.lookup(ctx->tmp);
    auto str = sym_table.parse_local_to_offset(info);
    ir.addIMC("t2", OP::LOAD, str, VOID, CLASS_BOOLEAN);
    ir.addIMC(labels[0], OP::BEQZ, "t2", VOID, CLASS_VOID); //if(tmp==false) goto label
    // true case, fall
}

void IRListener::enterWhileCond(CACTParser::WhileCondContext *ctx) {}
void IRListener::exitWhileCond(CACTParser::WhileCondContext *ctx)
{
    ctx->tmp = ctx->cond()->tmp;
    ctx->cls = ctx->cond()->cls;
    auto info = sym_table.lookup(ctx->tmp);
    auto str = sym_table.parse_local_to_offset(info);

    auto labels = while_labels.get(ctx->parent);
    ir.addIMC("t2", OP::LOAD, str, VOID, CLASS_BOOLEAN);
    ir.addIMC(labels[1], OP::BEQZ, "t2", VOID, CLASS_VOID); //if(tmp==false) goto label
    // true case, fall
}

void IRListener::enterIfStmt(CACTParser::IfStmtContext *ctx)
{
    auto label1 = ir.gen_label();
    std::vector<std::string> t_labels = {label1};
    if_labels.put(ctx, t_labels);
}
void IRListener::exitIfStmt(CACTParser::IfStmtContext *ctx)
{
    // S.code = cond.code||label(cond.true)||stmt.code
    auto labels = if_labels.get(ctx);
    ir.addIMC(labels[0], OP::LABEL, VOID, VOID, CLASS_VOID);
}

void IRListener::enterIfElseStmt(CACTParser::IfElseStmtContext *ctx)
{
    auto label1 = ir.gen_label();
    auto label2 = ir.gen_label();
    std::vector<std::string> t_labels = {label1, label2};
    if_labels.put(ctx, t_labels);
}
void IRListener::exitIfElseStmt(CACTParser::IfElseStmtContext *ctx)
{
    auto labels = if_labels.get(ctx);
    ir.addIMC(labels[1], OP::LABEL, VOID, VOID, CLASS_VOID); //mark label position
}

void IRListener::enterElseStmt(CACTParser::ElseStmtContext *ctx)
{
    auto labels = if_labels.get(ctx->parent);
    ir.addIMC(labels[1], OP::GOTO, VOID, VOID, CLASS_VOID);
    ir.addIMC(labels[0], OP::LABEL, VOID, VOID, CLASS_VOID);
}
void IRListener::exitElseStmt(CACTParser::ElseStmtContext *ctx) {}

void IRListener::enterWhileStmt(CACTParser::WhileStmtContext *ctx)
{
    auto label_begin = ir.gen_label();
    ir.addIMC(label_begin, OP::LABEL, VOID, VOID, CLASS_VOID);
    auto label_end = ir.gen_label();
    std::vector<std::string> t_labels = {label_begin, label_end};
    while_labels.put(ctx, t_labels);
    label_stack.push_back(label_begin);
    label_stack.push_back(label_end);
};

void IRListener::exitWhileStmt(CACTParser::WhileStmtContext *ctx)
{
    auto labels = while_labels.get(ctx);
    ir.addIMC(labels[0], OP::GOTO, VOID, VOID, CLASS_VOID);
    ir.addIMC(labels[1], OP::LABEL, VOID, VOID, CLASS_VOID); //mark label position
    label_stack.pop_back();
    label_stack.pop_back();
}

void IRListener::enterBreakStmt(CACTParser::BreakStmtContext *ctx) {}
void IRListener::exitBreakStmt(CACTParser::BreakStmtContext *ctx)
{
    auto idx = label_stack.size();
    auto label = label_stack[idx - 1];                  //没有使用之前的方法，是因为breakstmt的parent不一定是大循环
    ir.addIMC(label, OP::GOTO, VOID, VOID, CLASS_VOID); //goto while end
}

void IRListener::enterContinueStmt(CACTParser::ContinueStmtContext *ctx) {}
void IRListener::exitContinueStmt(CACTParser::ContinueStmtContext *ctx)
{
    auto idx = label_stack.size();
    auto label = label_stack[idx - 2];
    ir.addIMC(label, OP::GOTO, VOID, VOID, CLASS_VOID); //goto while start
}

/*********************************************
 * *******************************************
 * *******************************************
 * *******************************************
 * *******************************************
 * *******************************************
 * *******************************************
 * *******************************************
 * ******************************************/

//CODE GEN HERE

void IRListener::RISCV_Gen(std::string out_file)
{
    //file info
    std::string name;
    name = ".file   \"" + out_file + ".cact\"";
    riscv_codes.push_back(name);
    riscv_codes.push_back(".option nopic");
    riscv_codes.push_back(".attribute arch, \"rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0\"");
    //RV64GC
    riscv_codes.push_back(".attribute unaligned_access, 0");
    riscv_codes.push_back(".attribute stack_align, 16");

    //text section
    riscv_codes.push_back("");
    riscv_codes.push_back(".text");
    for (auto i = ir.ir_codes.begin(); i != ir.ir_codes.end(); i++)
    {
        auto ins = *i;
        if (ins.op == OP::FUNC_BEGIN)
        {
            RISCV_FuncBegin(ins);
        }
        else if (ins.op == OP::LABEL)
        {
            RISCV_Label(ins);
        }
        else if (ins.op == OP::GOTO)
        {
            RISCV_Goto(ins);
        }

        else if (ins.op == OP::FUNC_END)
        {
            RISCV_FuncEnd(ins);
        }
        else if (ins.op == OP::CALL)
        {
            RISCV_Call(ins);
        }
        else if (ins.op == OP::RET)
        {
            RISCV_Return(ins);
        }
        else if (ins.op == OP::MOV)
        {
            RISCV_Move(ins);
        }
        else if (ins.op == OP::STORE)
        {
            RISCV_Store(ins);
        }
        else if (ins.op == OP::LOAD)
        {
            RISCV_Load(ins);
        }
        else if (ins.op == OP::LI)
        {
            RISCV_Li(ins);
        }
        else if (ins.op == OP::BEQZ)
        {
            RISCV_Beqz(ins);
        }
        else if (ins.op == OP::SAVE_ARR)
        {
            RISCV_SAVEARR(ins);
        }
        else if (ins.op == OP::READ_ARR)
        {
            RISCV_READARR(ins);
        }

        else if (ins.op == OP::GLOBAL_ASSIGN)
        {
            RISCV_GLOBAL(ins);
        }
        else if (ins.op == OP::GLOBAL_ASSIGN_ARR)
        {
            RISCV_GLOBAL_ARR(ins);
        }
        else if (ins.op == OP::GLOBAL_ASSIGN_IDX)
        {
            RISCV_GLOBAL_ARR_IDX(ins);
        }
        else
        {
            RISCV_Arithmetic(ins);
        }
    }
    // 输出到文件，清空再输入
    std::ofstream out("./out/" + out_file + ".S", std::ios::trunc);
    for (auto &ins : riscv_codes)
    {
        if (ins.find(':') == -1)
        {
            out << '\t';
        }
        out << ins << std::endl;
    }
    out.close();
}

void IRListener::RISCV_FuncBegin(IRCode &c)
{
    auto func_name = c.rst;
    if (func_name == "print_int" || func_name == "print_bool" ||
        func_name == "print_double" || func_name == "print_float" ||
        func_name == "get_int" ||
        func_name == "get_float" || func_name == "get_double")
    {
        ;
    }
    else
    {
        auto func_info = sym_table.lookup_func(func_name);
        riscv_codes.push_back(".text");
        riscv_codes.push_back(".align   1");
        riscv_codes.push_back(".globl   " + func_name);
        riscv_codes.push_back(".type    " + func_name + ", @function");
        riscv_codes.push_back(func_name + ":");
        //calculate stack size
        auto info = sym_table.lookup_func(c.rst);
        int frame_size = info->stack_size; //read from func info
        int div = frame_size / 16;
        int mod = frame_size % 16;
        int size = (mod == 0) ? (div + 1) * 16 : (div + 2) * 16; //ra sp 8+8
        riscv_codes.push_back("#frame size:" + std::to_string(size));
        riscv_codes.push_back("addi     sp,sp,-" + std::to_string(size));
        riscv_codes.push_back("sd   ra," + std::to_string(size - 8) + "(sp)");
        riscv_codes.push_back("sd   s0," + std::to_string(size - 16) + "(sp)");
        riscv_codes.push_back("addi     s0,sp," + std::to_string(size));
        //passing argument
        //暂不考虑参数寄存器不够用的情况
        //passing a0-a7, fa0-fa7 to offset(s0)
    }
}

void IRListener::RISCV_Return(IRCode &c)
{
    std::string j = "j    ";
    std::string target = "end_handle" + std::to_string(end_func_label); //@ need specify name
    riscv_codes.push_back(j + target);
}

void IRListener::RISCV_FuncEnd(IRCode &c)
{
    auto func_name = c.rst;
    if (func_name == "print_int" || func_name == "print_bool" ||
        func_name == "print_double" || func_name == "print_float" ||
        func_name == "get_int" ||
        func_name == "get_float" || func_name == "get_double")
    {
        ;
    }
    else
    {
        auto info = sym_table.lookup_func(c.rst);
        int frame_size = info->stack_size; //read from func info
        int div = frame_size / 16;
        int mod = frame_size % 16;
        int size = (mod == 0) ? (div + 1) * 16 : (div + 2) * 16; //ra sp 8+8
        riscv_codes.push_back("#frame size:" + std::to_string(size));
        //all func end and return with this pattern
        riscv_codes.push_back("end_handle" + std::to_string(end_func_label) + ":");
        end_func_label++;
        riscv_codes.push_back("ld   ra," + std::to_string(size - 8) + "(sp)");
        riscv_codes.push_back("ld   s0," + std::to_string(size - 16) + "(sp)");
        riscv_codes.push_back("addi     sp,sp," + std::to_string(size));
        riscv_codes.push_back("jr ra");
        riscv_codes.push_back(".size    " + func_name + ",  .-" + func_name);
    }
}

void IRListener::RISCV_Call(IRCode &c)
{
    auto call_func_name = c.rst;
    riscv_codes.push_back("call     " + call_func_name);
    riscv_codes.push_back("nop");
}

void IRListener::RISCV_Label(IRCode &c)
{
    auto lab = c.rst.erase(0, 7);
    riscv_codes.push_back("label" + lab + ":");
}

void IRListener::RISCV_Move(IRCode &c)
{
    if (c.cls == CLASS_INT || c.cls == CLASS_BOOLEAN)
    {
        riscv_codes.push_back("mv   " + c.rst + "," + c.num1);
    }
    else if (c.cls == CLASS_FLOAT)
    {
        riscv_codes.push_back("fmv.s   f" + c.rst + ",f" + c.num1);
    }
    else if (c.cls == CLASS_DOUBLE)
    {
        //riscv_codes.push_back("mv   f" + c.rst + ",f" + c.num1);
        riscv_codes.push_back("fmv.d   f" + c.rst + ",f" + c.num1);
    }
}

void IRListener::RISCV_Goto(IRCode &c)
{
    auto lab = c.rst.erase(0, 7);
    riscv_codes.push_back("j    label" + lab);
}

void IRListener::RISCV_Arithmetic(IRCode &c)
{
    if (c.cls == CLASS_INT)
    {
        if (c.op == OP::ADD)
        {
            riscv_codes.push_back("addw      " + c.rst + ", " + c.num1 + ", " + c.num2);
        }
        else if (c.op == OP::SUB)
        {
            riscv_codes.push_back("subw      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::MUL)
        {
            riscv_codes.push_back("mul      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::DIV)
        {
            riscv_codes.push_back("div      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::MOD)
        {
            riscv_codes.push_back("rem      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::NEG)
        {
            riscv_codes.push_back("neg      " + c.rst + "," + c.num1);
        }
        else if (c.op == OP::LT)
        {
            riscv_codes.push_back("slt      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::GE) //LT取反
        {
            riscv_codes.push_back("slt      " + c.rst + "," + c.num1 + "," + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::EQU)
        {
            riscv_codes.push_back("subw      " + c.rst + "," + c.num1 + "," + c.num2);
            riscv_codes.push_back("sltu      " + c.rst + ",zero," + c.rst);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::NEQ)
        {
            riscv_codes.push_back("subw      " + c.rst + "," + c.num1 + "," + c.num2);
            riscv_codes.push_back("sltu      " + c.rst + ",zero," + c.rst);
        }
        else if (c.op == OP::LE) //lt  交换num1 num2, 再取反
        {
            riscv_codes.push_back("slt      " + c.rst + "," + c.num2 + "," + c.num1);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::GT) //lt  交换num1 num2
        {
            riscv_codes.push_back("slt      " + c.rst + "," + c.num2 + "," + c.num1);
        }
    }
    else if (c.cls == CLASS_BOOLEAN)
    {
        if (c.op == OP::AND)
        {
            riscv_codes.push_back("and      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::OR)
        {
            riscv_codes.push_back("or      " + c.rst + "," + c.num1 + "," + c.num2);
        }
        else if (c.op == OP::EQU)
        {
            riscv_codes.push_back("subw      " + c.rst + "," + c.num1 + "," + c.num2);
            riscv_codes.push_back("sltu      " + c.rst + ",zero," + c.rst);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::NEQ)
        {
            riscv_codes.push_back("subw      " + c.rst + "," + c.num1 + "," + c.num2);
            riscv_codes.push_back("sltu      " + c.rst + ",zero," + c.rst);
        }
    }
    else if (c.cls == CLASS_DOUBLE)
    {
        if (c.op == OP::ADD)
        {
            riscv_codes.push_back("fadd.d      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        else if (c.op == OP::SUB)
        {
            riscv_codes.push_back("fsub.d      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        else if (c.op == OP::MUL)
        {
            riscv_codes.push_back("fmul.d      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        else if (c.op == OP::DIV)
        {
            riscv_codes.push_back("fdiv.d      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        //float % not valid
        else if (c.op == OP::GE)
        {
            riscv_codes.push_back("flt.d      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::GT)
        {
            riscv_codes.push_back("fle.d      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::LE)
        {
            riscv_codes.push_back("fle.d      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
        }
        else if (c.op == OP::LT)
        {
            riscv_codes.push_back("flt.d      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
        }
        else if (c.op == OP::EQU)
        {
            riscv_codes.push_back("feq.d      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
        }
        else if (c.op == OP::NEQ)
        {
            riscv_codes.push_back("feq.d      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
    }
    else if (c.cls == CLASS_FLOAT)
    {
        if (c.op == OP::ADD)
        {
            riscv_codes.push_back("fadd.s      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        else if (c.op == OP::SUB)
        {
            riscv_codes.push_back("fsub.s      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        else if (c.op == OP::MUL)
        {
            riscv_codes.push_back("fmul.s      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        else if (c.op == OP::DIV)
        {
            riscv_codes.push_back("fdiv.s      f" + c.rst + ", f" + c.num1 + ", f" + c.num2);
        }
        //float % not valid
        else if (c.op == OP::GE)
        {
            riscv_codes.push_back("flt.s      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::GT)
        {
            riscv_codes.push_back("fle.s      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
        else if (c.op == OP::LE)
        {
            riscv_codes.push_back("fle.s      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
        }
        else if (c.op == OP::LT)
        {
            riscv_codes.push_back("flt.s      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
        }
        else if (c.op == OP::EQU)
        {
            riscv_codes.push_back("feq.s      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
        }
        else if (c.op == OP::NEQ)
        {
            riscv_codes.push_back("feq.s      " + c.rst + ",f" + c.num1 + ",f" + c.num2);
            riscv_codes.push_back("not      " + c.rst + "," + c.rst);
            riscv_codes.push_back("andi      " + c.rst + "," + c.rst + ",1");
        }
    }
    else
    {
        ;
    }
}

void IRListener::RISCV_Load(IRCode &c)
{
    auto name = c.num1;
    auto postfix = name.substr(name.size() - 2, 2);
    if (postfix == ".g") //global
    {
        name.pop_back();
        name.pop_back();
        if (c.cls == CLASS_INT || c.cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("lui   " + c.rst + ",%hi(" + name + ")");
            riscv_codes.push_back("lw   " + c.rst + ",%lo(" + name + ")(" + c.rst + ")");
        }
        else if (c.cls == CLASS_FLOAT)
        {
            riscv_codes.push_back("lui   " + c.rst + ",%hi(" + name + ")");
            riscv_codes.push_back("flw   f" + c.rst + ",%lo(" + name + ")(" + c.rst + ")");
        }
        else
        {
            riscv_codes.push_back("lui   " + c.rst + ",%hi(" + name + ")");
            riscv_codes.push_back("fld   f" + c.rst + ",%lo(" + name + ")(" + c.rst + ")");
        }
    }
    else
    {
        if (c.cls == CLASS_INT || c.cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("lw   " + c.rst + ", " + c.num1);
        }
        else if (c.cls == CLASS_FLOAT)
        { //flw   fs1     offset(s0)
            riscv_codes.push_back("flw   f" + c.rst + ", " + c.num1);
        }
        else if (c.cls == CLASS_DOUBLE)
        {
            riscv_codes.push_back("fld   f" + c.rst + ", " + c.num1);
        }
    }
}

void IRListener::RISCV_Store(IRCode &c)
{
    auto name = c.num1;
    auto postfix = name.substr(name.size() - 2, 2);
    if (postfix == ".g") //global
    {
        name.pop_back();
        name.pop_back();
        if (c.cls == CLASS_INT || c.cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("lui   " + c.rst + ",%hi(" + name + ")");
            riscv_codes.push_back("sw   " + c.rst + ",%lo(" + name + ")(" + c.rst + ")");
        }
        else if (c.cls == CLASS_FLOAT)
        {
            riscv_codes.push_back("lui   " + c.rst + ",%hi(" + name + ")");
            riscv_codes.push_back("fsw   f" + c.rst + ",%lo(" + name + ")(" + c.rst + ")");
        }
        else
        {
            riscv_codes.push_back("lui   " + c.rst + ",%hi(" + name + ")");
            riscv_codes.push_back("fsd   f" + c.rst + ",%lo(" + name + ")(" + c.rst + ")");
        }
    }
    if (c.cls == CLASS_INT || c.cls == CLASS_BOOLEAN)
    {
        riscv_codes.push_back("sw   " + c.rst + ", " + c.num1);
    }
    else if (c.cls == CLASS_FLOAT)
    {
        riscv_codes.push_back("fsw   f" + c.rst + ", " + c.num1);
    }
    else if (c.cls == CLASS_DOUBLE)
    {
        riscv_codes.push_back("fsd   f" + c.rst + ", " + c.num1);
    }
}

void IRListener::RISCV_READARR(IRCode &c)
{
    int cls = c.cls;
    auto name = c.num1;
    auto postfix = name.substr(name.size() - 2, 2);
    if (postfix == ".g") //global
    {
        name.pop_back();
        name.pop_back();
        if (cls == CLASS_INT || cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("lui  s2,%hi(" + name + ")");
            riscv_codes.push_back("addi s2,s2,%lo(" + name + ")"); //base
            riscv_codes.push_back("add s3,s2,t5");                 //s3 marked position  0(s3)
            riscv_codes.push_back("lw   " + c.rst + ",0(s3)");
        }
        else if (cls == CLASS_FLOAT)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("lui  s2,%hi(" + name + ")");
            riscv_codes.push_back("addi s2,s2,%lo(" + name + ")"); //base
            riscv_codes.push_back("add s3,s2,t5");                 //s3 marked position  0(s3)
            riscv_codes.push_back("flw   f" + c.rst + ",0(s3)");
        }
        else
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,3");
            riscv_codes.push_back("lui  s2,%hi(" + name + ")");
            riscv_codes.push_back("addi s2,s2,%lo(" + name + ")"); //base
            riscv_codes.push_back("add s3,s2,t5");                 //s3 marked position  0(s3)
            riscv_codes.push_back("fld   f" + c.rst + ",0(s3)");
        }
    }
    else
    {
        if (cls == CLASS_INT || cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("add  s2,s0,t5");
            std::string origin = "lw     " + c.rst + "," + c.num1;
            origin = origin.substr(0, origin.find_first_of("("));
            origin.append("(s2)");
            riscv_codes.push_back(origin);
        }
        else if (cls == CLASS_FLOAT)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("add  s2,s0,t5");
            std::string origin = "flw     f" + c.rst + "," + c.num1;
            origin = origin.substr(0, origin.find_first_of("("));
            origin.append("(s2)");
            riscv_codes.push_back(origin);
        }
        else if (cls == CLASS_DOUBLE)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,3");
            riscv_codes.push_back("add  s2,s0,t5");
            std::string origin = "fld     f" + c.rst + "," + c.num1;
            origin = origin.substr(0, origin.find_first_of("("));
            origin.append("(s2)");
            riscv_codes.push_back(origin);
        }
    }
}
void IRListener::RISCV_SAVEARR(IRCode &c)
{
    int cls = c.cls;
    auto name = c.num1;
    auto postfix = name.substr(name.size() - 2, 2);
    if (postfix == ".g") //global
    {
        name.pop_back();
        name.pop_back();
        if (cls == CLASS_INT || cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("lui  s2,%hi(" + name + ")");
            riscv_codes.push_back("addi s2,s2,%lo(" + name + ")"); //base
            riscv_codes.push_back("add s3,s2,t5");                 //s3 marked position  0(s3)
            riscv_codes.push_back("sw   " + c.rst + ",0(s3)");
        }
        else if (cls == CLASS_FLOAT)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("lui  s2,%hi(" + name + ")");
            riscv_codes.push_back("addi s2,s2,%lo(" + name + ")"); //base
            riscv_codes.push_back("add s3,s2,t5");                 //s3 marked position  0(s3)
            riscv_codes.push_back("fsw   f" + c.rst + ",0(s3)");
        }
        else
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,3");
            riscv_codes.push_back("lui  s2,%hi(" + name + ")");
            riscv_codes.push_back("addi s2,s2,%lo(" + name + ")"); //base
            riscv_codes.push_back("add s3,s2,t5");                 //s3 marked position  0(s3)
            riscv_codes.push_back("fsd   f" + c.rst + ",0(s3)");
        }
    }

    else
    {
        if (cls == CLASS_INT || cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("add  s2,s0,t5"); 
            std::string origin = "sw     " + c.rst + "," + c.num1;
            origin = origin.substr(0, origin.find_first_of("("));
            origin.append("(s2)");
            riscv_codes.push_back(origin);
        }
        else if (cls == CLASS_FLOAT)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,2");
            riscv_codes.push_back("add  s2,s0,t5");
            std::string origin = "fsw     f" + c.rst + "," + c.num1;
            origin = origin.substr(0, origin.find_first_of("("));
            origin.append("(s2)");
            riscv_codes.push_back(origin);
        }
        else if (cls == CLASS_DOUBLE)
        {
            riscv_codes.push_back("mv   t5," + c.num2);
            riscv_codes.push_back("slliw    t5,t5,3");
            riscv_codes.push_back("add  s2,s0,t5");
            std::string origin = "fsd     f" + c.rst + "," + c.num1;
            origin = origin.substr(0, origin.find_first_of("("));
            origin.append("(s2)");
            riscv_codes.push_back(origin);
        }
    }
}

void IRListener::RISCV_Li(IRCode &c)
{
    if (c.cls == CLASS_INT || c.cls == CLASS_BOOLEAN)
    {
        riscv_codes.push_back("li   " + c.rst + ", " + c.num1);
    }
    else if (c.cls == CLASS_FLOAT)
    {
        riscv_codes.push_back("lui  a5,%hi(.LC" + std::to_string(float_tmp_cnt) + ")");
        riscv_codes.push_back("flw  f" + c.rst + ",%lo(.LC" + std::to_string(float_tmp_cnt) + ")(a5)");
        float fval = std::stof(c.num1);
        int decimal_fval = *(int *)&fval;

        riscv_codes.push_back(".section .rodata");
        riscv_codes.push_back(".align  2");
        riscv_codes.push_back(".LC" + std::to_string(float_tmp_cnt) + ":");
        riscv_codes.push_back(".word    " + std::to_string(decimal_fval));
        riscv_codes.push_back(".text");
        float_tmp_cnt++;
    }
    else //double
    {
        riscv_codes.push_back("lui  a5,%hi(.LC" + std::to_string(float_tmp_cnt) + ")");
        riscv_codes.push_back("fld  f" + c.rst + ",%lo(.LC" + std::to_string(float_tmp_cnt) + ")(a5)");
        double dval = std::stod(c.num1);
        long decimal_dval = *(long *)&dval;
        int hi = decimal_dval >> 32;
        int lo = decimal_dval & 0x00000000ffffffff;

        riscv_codes.push_back(".section .rodata");
        riscv_codes.push_back(".align  3");
        riscv_codes.push_back(".LC" + std::to_string(float_tmp_cnt) + ":");
        riscv_codes.push_back(".word    " + std::to_string(lo));
        riscv_codes.push_back(".word    " + std::to_string(hi));
        riscv_codes.push_back(".text");
        float_tmp_cnt++;
    }
}

void IRListener::RISCV_Beqz(IRCode &c)
{
    auto lab = c.rst.erase(0, 7);
    riscv_codes.push_back("beqz   " + c.num1 + ",label" + lab);
}

void IRListener::RISCV_GLOBAL(IRCode &c)
{
    auto name = c.rst;
    auto cls = c.cls;
    std::string ro = (c.num2 == "const") ? ".section     .srodata,\"a\"" : (c.num2 == "uninited") ? ".bss" : /*(cls == CLASS_DOUBLE || cls == CLASS_FLOAT) ?*/ ".section     .sdata,\"aw\"" /*: ".data"*/;
    std::string align = (c.cls == CLASS_DOUBLE) ? ".align   3" : ".align  2";
    int size = (cls == CLASS_DOUBLE) ? 8 : 4;

    riscv_codes.push_back(".globl   " + name);
    riscv_codes.push_back(ro);
    riscv_codes.push_back(align);
    riscv_codes.push_back(".type    " + name + ", @object");
    riscv_codes.push_back(".size    " + name + ", " + std::to_string(size));
    riscv_codes.push_back(name + ":");
    if (c.num2 == "uninited")
    {
        riscv_codes.push_back(".zero    " + std::to_string(size));
    }
    else
    {
        if (cls == CLASS_INT || cls == CLASS_BOOLEAN)
        {
            riscv_codes.push_back(".word    " + c.num1);
        }
        else if (cls == CLASS_FLOAT)
        {
            //如何float转10进制的IEEE 754表示
            auto floatvalue = std::stof(c.num1);
            int decimal_fval = *(int *)&floatvalue;
            riscv_codes.push_back(".word    " + std::to_string(decimal_fval));
        }
        else //double
        {
            auto dvalue = std::stod(c.num1);
            long decimal_dval = *(long *)&dvalue;
            int hi = (decimal_dval >> 32);
            int lo = decimal_dval & (long)0x00000000ffffffff;
            riscv_codes.push_back(".word    " + std::to_string(lo));
            riscv_codes.push_back(".word    " + std::to_string(hi));
        }
    }
}

void IRListener::RISCV_GLOBAL_ARR(IRCode &c)
{
    auto name = c.rst;
    auto cls = c.cls;
    auto leng_str = c.num1;
    int length = std::stoi(leng_str);
    std::string ro = (c.num2 == "const") ? ".section     .srodata,\"a\"" : (c.num2 == "uninited") ? ".bss" : (cls == CLASS_DOUBLE || cls == CLASS_FLOAT) ? ".section     .sdata,\"aw\"" : ".data";
    std::string align = ".align   3";
    int size = (cls == CLASS_DOUBLE) ? 8 * length : 4 * length;

    riscv_codes.push_back(".globl   " + name);
    riscv_codes.push_back(ro);
    riscv_codes.push_back(align);
    riscv_codes.push_back(".type    " + name + ", @object");
    riscv_codes.push_back(".size    " + name + ", " + std::to_string(size));
    riscv_codes.push_back(name + ":");
    if (c.num2 == "uninited")
    {
        riscv_codes.push_back(".zero    " + std::to_string(size));
    }
}

void IRListener::RISCV_GLOBAL_ARR_IDX(IRCode &c)
{
    auto cls = c.cls;
    if (cls == CLASS_INT || cls == CLASS_BOOLEAN)
    {
        riscv_codes.push_back(".word    " + c.num1);
    }
    else if (cls == CLASS_FLOAT)
    {
        //如何float转10进制的IEEE 754表示
        auto floatvalue = std::stof(c.num1);
        int decimal_fval = *(int *)&floatvalue;
        riscv_codes.push_back(".word    " + std::to_string(decimal_fval));
    }
    else //double
    {
        auto dvalue = std::stod(c.num1);
        long decimal_dval = *(long *)&dvalue;
        int hi = (decimal_dval >> 32);
        int lo = decimal_dval & (long)0x00000000ffffffff;
        riscv_codes.push_back(".word    " + std::to_string(lo));
        riscv_codes.push_back(".word    " + std::to_string(hi));
    }
}