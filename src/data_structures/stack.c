#include "stack.h"

// code for stack used in parsing 
// now, stack node has the same structure as rhsNode(flag for nonterm or term, the non terminal or terminal itself,and 
//pointer to itself  so we use it for nodes for a stack

// global variable 
struct stack *stack;

char map_non_terminal[NUM_NON_TERMINALS][32] = {
	"<program>", "<moduleDeclarations>", "<moduleDeclaration>", "<otherModules>",
	"<driverModule>", "<module>", "<ret>", "<input_plist>",
	"<sub_input_plist>", "<output_plist>", "<sub_output_plist>", "<dataType>",
	"<dynamic_range>", "<type>", "<moduleDef>", "<statements>",
	"<statement>", "<ioStmt>", "<boolConstt>", "<extended_var>",
	"<var>", "<whichId>", "<simpleStmt>", "<assignmentStmt>",
	"<whichStmt>", "<lvalueIDStmt>", "<lvalueARRStmt>", "<index>",
	"<moduleReuseStmt>", "<optional>", "<idList>", "<sub_idList>",
	"<new_expression>", "<u>", "<sub_u>", "<expression>",
	"<N7>", "<AnyTerm>", "<N8>", "<arithmeticExpr>",
	"<sub_arithmeticExpr>", "<term>", "<sub_term>", "<factor>",
	"<op1>", "<op2>", "<logicalOp>", "<relationalOp>",
	"<declareStmt>", "<conditionalStmt>", "<caseStmt>", "<nullableCaseStmt>",
	"<value>", "<default>", "<iterativeStmt>", "<range>"};


char map_terminal[NUM_TERMINALS][16] = {
		"ID", "NUM", "RNUM", "PLUS", "MINUS",
		"MUL", "DIV", "LT", "LE", "GE",
		"GT", "EQ", "NE", "DEF", "DRIVERDEF",
		"ENDDEF", "DRIVERENDDEF", "SQBO", "SQBC", "BO",
		"BC", "RANGEOP", "COLON", "ASSIGNOP", "SEMICOL",
		"COMMA", "INTEGER", "REAL", "BOOLEAN", "OF",
		"ARRAY", "START", "END", "DECLARE", "MODULE",
		"DRIVER", "PROGRAM", "GET_VALUE", "PRINT", "USE",
		"WITH", "PARAMETERS", "TRUE", "FALSE", "TAKES",
		"INPUT", "RETURNS", "AND", "OR", "FOR",
		"IN", "SWITCH", "CASE", "BREAK", "DEFAULT",
		"WHILE", "EPSILON"
	};

void initialiseStack() {

	stack = (struct stack *) malloc(sizeof(struct stack));
	stack -> head = NULL;
}

void push(struct stackNode *newNode) {
	
	if(stack->head == NULL){
		stack->head = newNode;
		return;
	}
	newNode->next = stack->head;
	stack->head = newNode;	
}

struct stackNode *pop() {

	if(stack->head == NULL){
		return NULL;
	}
	struct stackNode *temp = stack ->head;
	stack->head = (stack->head)->next;
	temp->next = NULL;
	return temp;
} 

void pushRuleIntoStack(struct rule *production_rule) {

	struct rhsNode *curr = production_rule->head;
	// X -> PQR - we pop non terminal X and push in all elements of RHS of rule- in reverse order- RQP
	// X has already been popped- if we call this function
	struct rhsNode *last = NULL;
	struct rhsNode *temp = NULL;

	// first, temporarily reversing the order of temp order
	while(curr!=NULL){
		temp = curr->next;
		curr->next = last;
		last = curr;
		curr = temp;
	}
	// now, theyre in reverse order- now to insert elements as well as restore the original order
	while(last!=NULL){

		struct stackNode *newNode = (struct stackNode *)malloc(sizeof(struct stackNode));
		// insert data
		if(last->flag == TERMINAL){
			newNode->symbol.terminal = last->symbol.terminal;
		}
		else {
			newNode->symbol.non_terminal = last->symbol.non_terminal;
		}
		newNode->flag = last->flag;
		newNode->next = NULL;
		// insert new node into stack
		push(newNode);
		// now, to restore the previous connections
		temp = last->next;
		last->next = curr;
		curr = last;
		last = temp;
	}

	// thats it- rule's nonterminals and terminals have been added to the stack- reversed
	return;
	
}

void printStack() {
	struct stackNode *ptr = stack->head;
	while(ptr != NULL) {
		if (ptr->flag == NON_TERMINAL)
			printf("%s\n", map_non_terminal[ptr->symbol.non_terminal]);
		else
			printf("%s\n", map_terminal[ptr->symbol.terminal]);
		ptr = ptr->next;
	}
	return;
}