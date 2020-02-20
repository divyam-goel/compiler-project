#include "lexerDef.h"

#ifndef PARSERDEF_H
#define PARSERDEF_H

// #define NUM_RULES 104
#define NUM_RULES 8
// #define NUM_TERMINALS 56
#define NUM_TERMINALS 7
// #define NUM_NON_TERMINALS 56
#define NUM_NON_TERMINALS 5

// enum nonTerminals {
// 	_OP1, _DECLARESTMT, _SUB_INPUT_PLIST,
// 	_EXTENDED_VAR, _IDLIST, _SUB_TERM,
// 	_LVALUEARRSTMT, _SUB_OUTPUT_PLIST,
// 	_OPTIONAL, _ASSIGNMENTSTMT, _WHICHSTMT,
// 	_DEFAULT, _PROGRAM, _STATEMENT, _NULLABLECASESTMT,
// 	_MODULEDECLARATIONS, _ITERATIVESTMT, _FACTOR,
// 	_SUB_IDLIST, _LVALUEIDSTMT, _CONDITIONALSTMT,
// 	_INPUT_PLIST, _ANYTERM, _BOOLCONSTT, _DYNAMIC_RANGE,
// 	_TERM, _SUB_U, _INDEX, _WHICHID, _CASESTMT,
// 	_OTHERMODULES, _MODULE, _RELATIONALOP, _LOGICALOP,
// 	_MODULEDEF, _MODULEREUSESTMT, _OUTPUT_PLIST,
// 	_VAR, _DRIVERMODULE, _NEW_EXPRESSION,
// 	_RET, _DATATYPE, _STATEMENTS, _ARITHMETICEXPR,
// 	_RANGE, _SUB_ARITHMETICEXPR, _MODULEDECLARATION,
// 	_TYPE, _SIMPLESTMT, _EXPRESSION, _N7, _N8,
// 	_VALUE, _OP2, _IOSTMT, _U
// };

enum nonTerminals {
	E, E_, T, T_, F_
};

enum terminals {
	ID_, PLUS_, MUL_, BO_, BC_, DOLLAR_, EPS_
};

union terminalOrNonTerminal {
	enum nonTerminals non_terminal;
	enum terminals terminal;
};

enum typeOfSymbol {
	TERMINAL,
	NON_TERMINAL
};

struct rhsNode {
	union terminalOrNonTerminal symbol;
	enum typeOfSymbol flag;
	struct rhsNode *next;
};

struct rule {
	enum nonTerminals non_terminal;
	struct rhsNode *head;
};

typedef struct rule grammar[NUM_RULES];

struct firstAndFollow {
	int first[NUM_NON_TERMINALS][NUM_TERMINALS + 1];
	char follow[NUM_NON_TERMINALS][NUM_TERMINALS + 1];
};

typedef int table[NUM_NON_TERMINALS][NUM_TERMINALS];

union nodeValue {
	struct symbol terminal;
	enum nonTerminals non_terminal;
};

struct node {
	union nodeValue symbol;
	enum typeOfSymbol flag;
	struct node *child;
	struct node *next;
};

struct parseTree {
	struct node *head;
};

#endif