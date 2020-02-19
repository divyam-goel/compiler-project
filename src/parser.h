#include "lexer.h"
#include "utils.h"
#include "str_list.h"

#ifndef PARSER_H
#define PARSER_H

#define NUM_RULES 104
#define NUM_TERMINALS 56
#define NUM_NON_TERMINALS 56
#define MAX_LEN_NON_TERMINAL 30

enum nonTerminals {
	_OP1, _DECLARESTMT, _SUB_INPUT_PLIST,
	_EXTENDED_VAR, _IDLIST, _SUB_TERM,
	_LVALUEARRSTMT, _SUB_OUTPUT_PLIST,
	_OPTIONAL, _ASSIGNMENTSTMT, _WHICHSTMT,
	_DEFAULT, _PROGRAM, _STATEMENT, _NULLABLECASESTMT,
	_MODULEDECLARATIONS, _ITERATIVESTMT, _FACTOR,
	_SUB_IDLIST, _LVALUEIDSTMT, _CONDITIONALSTMT,
	_INPUT_PLIST, _ANYTERM, _BOOLCONSTT, _DYNAMIC_RANGE,
	_TERM, _SUB_U, _INDEX, _WHICHID, _CASESTMT,
	_OTHERMODULES, _MODULE, _RELATIONALOP, _LOGICALOP,
	_MODULEDEF, _MODULEREUSESTMT, _OUTPUT_PLIST,
	_VAR, _DRIVERMODULE, _NEW_EXPRESSION,
	_RET, _DATATYPE, _STATEMENTS, _ARITHMETICEXPR,
	_RANGE, _SUB_ARITHMETICEXPR, _MODULEDECLARATION,
	_TYPE, _SIMPLESTMT, _EXPRESSION, _N7, _N8,
	_VALUE, _OP2, _IOSTMT, _U
};

struct rule {
	char non_terminal[MAX_LEN_NON_TERMINAL + 1];
	struct str_list derivation;
	struct rule *next;
};

typedef struct rule grammar[NUM_RULES];

struct firstAndFollow {
	char first[NUM_NON_TERMINALS][NUM_TERMINALS];
	char follow[NUM_NON_TERMINALS][NUM_TERMINALS];
};

typedef int table[NUM_NON_TERMINALS][NUM_TERMINALS];

union terminalOrNonTerminal {
	struct symbol terminal;
	enum nonTerminals non_terminal;
};

struct node {
	union terminalOrNonTerminal symbol;
	struct node *child;
	struct node *next;
};

struct parseTree {
	struct node *head;
};

// ComputeFirstAndFollowSets (grammar G, FirstAndFollow  F)
// createParseTable(FirstAndFollow F, table T)
// parseInputSourceCode(char *testcaseFile, table T)
// printParseTree(parseTree PT, char *outfile)

#endif
