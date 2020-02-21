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
	int first[NUM_NON_TERMINALS][NUM_TERMINALS + 1];
	char follow[NUM_NON_TERMINALS][NUM_TERMINALS + 1];
};

typedef int table[NUM_NON_TERMINALS][NUM_TERMINALS];

struct rhsNode *createRhsNode(const char *val);

union nodeValue {
	struct symbol terminal;
	enum nonTerminal non_terminal;
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