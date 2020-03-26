#include "lexerDef.h"

/* To avoid circular dependency issues we moved four structures
 * over to astDef.h, namely, typeOfSymbol nodeValue, treeNode
 * and parseTree. */
#include "astDef.h"

#ifndef PARSERDEF_H
#define PARSERDEF_H

union terminalOrNonTerminal {
	enum nonTerminal non_terminal;
	enum terminal terminal;
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

#endif
