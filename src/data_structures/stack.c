#include "stack.h"

/* Global Variable - START */

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

/* Global Variable - END */


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


struct stackNode * addStackNode(struct rhsNode *rhs_node_ptr) {
	struct stackNode *stack_node_ptr = (struct stackNode *) malloc(sizeof(struct stackNode));

	if(rhs_node_ptr->flag == TERMINAL)
		stack_node_ptr->symbol.terminal = rhs_node_ptr->symbol.terminal;
	else
		stack_node_ptr->symbol.non_terminal = rhs_node_ptr->symbol.non_terminal;
	
	stack_node_ptr->flag = rhs_node_ptr->flag;
	stack_node_ptr->next = NULL;
	stack_node_ptr->tree_node_ptr = NULL;

	return stack_node_ptr;
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
