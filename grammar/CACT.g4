grammar CACT;

start: compUnit EOF;

compUnit: (decl | funcDef)*;

decl: constDecl | varDecl;

constDecl: CONST bType constDef (',' constDef)* ';';

bType: INT | DOUBLE | FLOAT | BOOL;

constDef:
	Ident '[' IntConst ']' '=' '{' (constExp ( ',' constExp)*)? '}'	# constDefArray
	| Ident '=' constInitVal										# constDefVal;

constInitVal: constExp;

//constArrayVal: '{' ( constExp ( ',' constExp)*)? '}';

varDecl: bType varDef ( ',' varDef)* ';';

varDef:
	Ident																# defVal
	| Ident '[' IntConst ']'											# defArray
	| Ident '=' constInitVal											# defInitVal
	| Ident '[' IntConst ']' '=' '{' (constExp ( ',' constExp)*)? '}'	# defInitArray;

funcDef:
	ret = funcType name = Ident '(' (
		funcFParam ( ',' funcFParam)*
	)? ')' block;

funcType: VOID | INT | FLOAT | DOUBLE | BOOL;

funcFParam: bType Ident ('[' ']')?;

block: '{' ( blockItem)* '}';

blockItem: decl | stmt;

stmt:
	lVal '=' exp ';'						# assignStmt
	| exp ';'								# exprStmt
	| ';'									# blankStmt
	| block									# blockStmt
	| IF '(' ifCond ')' stmt elseStmt			# ifElseStmt
	| IF '(' ifCond ')' stmt					# ifStmt
	| WHILE '(' whileCond ')' stmt				# whileStmt
	| BREAK ';'								# breakStmt
	| CONTINUE ';'							# continueStmt
	| RETURN (exp)? ';'						# returnStmt;

elseStmt:ELSE stmt;

exp locals[
	std::string tmp,
	int cls
	]: addExp | BoolConst;

ifCond locals[
	std::string tmp,
	int cls,
	bool val
	]: cond;

whileCond locals[
	std::string tmp,
	int cls,
	bool val
	]: cond;

cond locals[
	std::string tmp,
	int cls,
	bool val
	]: lOrExp;

lVal locals[
	std::string tmp,
	int cls,
	bool elemwise
	]:
	Ident //变量
	| Ident '[' exp ']' ; //数组

primaryExp locals[
	std::string tmp,
	int cls,
	bool elemwise
	]: '(' exp ')' | number | BoolConst | lVal;

number locals[
	std::string tmp,
	int cls
	]: IntConst | DoubleConst | FloatConst;

unaryExp locals[
	std::string tmp,
	int cls,
	bool elsewise
	]:
	primaryExp						# primary
	| Ident '(' (funcRParams)? ')'	# funcall
	| op=(ADD | SUB | NOT) unaryExp				# unary;
//unaryOp: ADD | SUB | NOT;

funcRParams: exp (',' exp)*;

mulExp
	locals[
	std::string tmp,
	int cls,
	bool elemwise
	]: unaryExp | mulExp op=(MUL | DIV | MOD) unaryExp;

addExp
	locals[
	std::string tmp,
	int cls,
	bool elemwise
	]: mulExp | addExp op=(ADD | SUB) mulExp;

relExp
	locals[
	std::string tmp,
	int cls,
	bool val
	]:
	addExp
	| relExp op=(GT | LT | LE | GE) addExp
	| BoolConst;

eqExp 
locals[
	std::string tmp,
	int cls,
	bool val
	]: 
	relExp | eqExp op=(EQUAL | NOTEQUAL) relExp;

lAndExp locals[
	std::string tmp,
	int cls,
	bool val
	]: eqExp | lAndExp op=AND eqExp;

lOrExp locals[
	std::string tmp,
	int cls,
	bool val
	]: lAndExp | lOrExp op=OR lAndExp;

constExp locals[
	std::string tmp,
	int cls
	]: number | BoolConst;
//boolExp: BoolConst | lVal | Ident '(' (funcRParams)? ')';

//boolConst: TRUE | FALSE;

/*parser */

/*lexer */

INT: 'int';
BOOL: 'bool';
DOUBLE: 'double';
FLOAT: 'float';
WHILE: 'while';
BREAK: 'break';
CONTINUE: 'continue';
IF: 'if';
ELSE: 'else';
RETURN: 'return';
VOID: 'void';
CONST: 'const';
//bool const
BoolConst: TRUE | FALSE;
TRUE: 'true';
FALSE: 'false';

ADD: '+';
SUB: '-';
MUL: '*';
DIV: '/';
MOD: '%';	

NOT: '!';

NOTEQUAL: '!=';
EQUAL:'==';
GT:'>';
LT:'<';
GE:'>=';
LE:'<=';

AND: '&&';
OR: '||';

Ident: [a-zA-Z_]+ [a-zA-Z_0-9]*;

//integer
IntConst:
	DecimalConstant IntegerSuffix? //10
	| OctalConstant IntegerSuffix? //8
	| HexadecimalConstant IntegerSuffix?; //16
//	| BinaryConstant; //2

//fragment BinaryConstant: '0' [bB] [0-1]+;

fragment DecimalConstant: NonzeroDigit Digit*;

fragment OctalConstant: '0' OctalDigit*;

fragment HexadecimalConstant:
	HexadecimalPrefix HexadecimalDigit+;

fragment HexadecimalPrefix: '0' [xX];

fragment NonzeroDigit: [1-9];

fragment Digit: [0-9];

fragment OctalDigit: [0-7];

fragment HexadecimalDigit: [0-9a-fA-F];

fragment IntegerSuffix:
	UnsignedSuffix LongSuffix?
	| UnsignedSuffix LongLongSuffix
	| LongSuffix UnsignedSuffix?
	| LongLongSuffix UnsignedSuffix?;

fragment UnsignedSuffix: [uU];

fragment LongSuffix: [lL];

fragment LongLongSuffix: 'll' | 'LL';

//float & double const	
DoubleConst:
	FractionalConstant ExponentPart? //e.g:	2.38e-13
	| DigitSequence ExponentPart; //e.g: 	327e-10

FloatConst:
	FractionalConstant ExponentPart? FloatingSuffix
	| DigitSequence ExponentPart FloatingSuffix;

fragment FractionalConstant:
	DigitSequence? '.' DigitSequence
	| DigitSequence '.';

fragment ExponentPart:
	'e' Sign? DigitSequence
	| 'E' Sign? DigitSequence;

fragment Sign: '+' | '-';

DigitSequence: Digit+;

fragment FloatingSuffix: 'f' | 'F';

//SKIP
NewLine: ( '\r' '\n'? | '\n') -> skip;
WhiteSpace: [ \t]+ -> skip;
LineComment: '//' ~[\r\n]* -> skip;
BlockComment: '/*' .*? '*/' -> skip;