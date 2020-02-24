#include "../parserDef.h"
#include "../parser.h"

#ifndef STACK_H
#define STACK_H

struct stack {

	struct stackNode *head;	
};

struct stackNode {
	union terminalOrNonTerminal symbol;
	enum typeOfSymbol flag;
	struct stackNode *next;
	struct treeNode *tree_node_ptr;
	enum nonTerminal tree_node_parent_enum;
};

void initialiseStack();
void push(struct stackNode *newNode);
struct stackNode *pop();
void printStack();
struct stackNode * addStackNode(struct rhsNode *rhs_node_ptr);
#endif
