// code for stack used in parsing 
// now, stack node has the same structure as rhsNode(flag for nonterm or term, the non terminal or terminal itself,and 
//pointer to itself  so we use it for nodes for a stack

struct parseStack{

	struct stackNode *head;	
};

struct stackNode {
	union terminalOrNonTerminal symbol;
	enum typeOfSymbol flag;
	struct stackNode *next;
};

// global variable 
struct parseStack *stack;

void initialiseStack(){

	stack = (struct parseStack *)malloc(sizeof(struct parseStack));
	stack -> head = NULL;
}
void push(struct stackNode *newNode){
	
	if(stack->head == NULL){
		stack->head = newNode;
		return;
	}
	newNode->next = stack->head;
	stack->head = newNode;	
}

struct stackNode *pop(){

	if(stack->head == NULL){
		return NULL;
	}
	struct stackNode *temp = stack ->head;
	stack->head = (stack->head)->next;
	temp->next = NULL;
	return temp;
} 

void pushRuleIntoStack(struct rule *production_rule){

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
		else{
			newNode->symbol.non_terminal = last->symbol.non_terminal;
		}
		newNode->flag = last->flag;
		newNode->next = NULL;
		// insert new node into stack
		push(newNode)
		// now, to restore the previous connections
		temp = last->next;
		last->next = curr;
		curr = last;
		last = temp;
	}

	// thats it- rule's nonterminals and terminals have been added to the stack- reversed
	return;
	
}

