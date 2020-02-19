#include "lexer.h"
#include "utils.h"

#ifndef PARSER_H
#define PARSER_H

#define NUM_RULES 104
#define NUM_TERMINALS 56
#define NUM_NON_TERMINALS 56
#define MAX_LEN_NON_TERMINAL 30

enum nonTerminals {
	OP1, DECLARESTMT, SUB_INPUT_PLIST,
	EXTENDED_VAR, IDLIST, SUB_TERM,
	LVALUEARRSTMT, SUB_OUTPUT_PLIST,
	OPTIONAL, ASSIGNMENTSTMT, WHICHSTMT,
	DEFAULT, PROGRAM, STATEMENT, NULLABLECASESTMT,
	MODULEDECLARATIONS, ITERATIVESTMT, FACTOR,
	SUB_IDLIST, LVALUEIDSTMT, CONDITIONALSTMT,
	INPUT_PLIST, ANYTERM, BOOLCONSTT, DYNAMIC_RANGE,
	TERM, SUB_U, INDEX, WHICHID, CASESTMT,
	OTHERMODULES, MODULE, RELATIONALOP, LOGICALOP,
	MODULEDEF, MODULEREUSESTMT, OUTPUT_PLIST,
	VAR, DRIVERMODULE, NEW_EXPRESSION,
	RET, DATATYPE, STATEMENTS, ARITHMETICEXPR,
	RANGE, SUB_ARITHMETICEXPR, MODULEDECLARATION,
	TYPE, SIMPLESTMT, EXPRESSION, N7, N8,
	VALUE, OP2, IOSTMT, U
};

struct rule {
	char non_terminal[MAX_LEN_NON_TERMINAL + 1];
	str_list derivation;
	struct rule *next;
};

typedef rule[NUM_RULES] grammar;

struct firstAndFollow {
	char first[NUM_NON_TERMINALS][NUM_TERMINALS];
	char follow[NUM_NON_TERMINALS][NUM_TERMINALS];
};

typedef int[NUM_NON_TERMINALS][NUM_TERMINALS] table;

union terminalOrNonTerminal {
	struct symbol terminal;
	enum nonTerminal non_terminal;
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
