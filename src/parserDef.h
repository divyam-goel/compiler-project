#include "lexerDef.h"

#ifndef PARSERDEF_H
#define PARSERDEF_H

union terminalOrNonTerminal {
	enum nonTerminal non_terminal;
	enum terminal terminal;
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
	enum nonTerminal non_terminal;
	struct rhsNode *head;
};

typedef struct rule grammar[NUM_RULES];

struct firstAndFollow {
	int first[NUM_NON_TERMINALS][NUM_TERMINALS];
	char follow[NUM_NON_TERMINALS][NUM_TERMINALS];
};

typedef int table[NUM_NON_TERMINALS][NUM_TERMINALS];

struct rhsNode *createRhsNode(const char *val);

	union nodeValue {
	struct symbol terminal;
	enum nonTerminal non_terminal;
};

struct treeNode {
	int rule_number;  /* Use this to index into grammar[]. */
	union nodeValue symbol;
	enum typeOfSymbol flag;
	struct treeNode *child;
	struct treeNode *next;
};

struct parseTree {
	struct treeNode *head;
};

#endif
