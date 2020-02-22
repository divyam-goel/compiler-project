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
};

void initialiseStack();
void push(struct stackNode *newNode);
struct stackNode *pop();
void pushRuleIntoStack(struct rule *production_rule);
void printStack();

#endif
