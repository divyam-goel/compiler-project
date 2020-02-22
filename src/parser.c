#include "parser.h"
#include "data_structures/stack.h"

grammar G;
table parseTable;
struct parseTree PT;
struct hashMap *nonTerminalMap;
struct hashMap *terminalMap;
struct firstAndFollow F;
extern struct stack *stack;

char nonTerminalStringRepresentations[NUM_NON_TERMINALS][32] = {
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


char terminalStringRepresentations[NUM_TERMINALS][16] = {
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
	
char terminalLiteralRepresentations[NUM_TERMINALS][16] = {
		"ID", "NUM", "RNUM", "+", "-",
		"*", "/", "<", "<=", ">",
		">=", "=", "!=", "<<", "<<<",
		">>", ">>>", "[", "]", "(",
		")", "..", ":", ":=", ";",
		",", "INTEGER", "REAL", "BOOLEAN", "of",
		"array", "start", "end", "declare", "module",
		"driver", "program", "get_value", "print", "use",
		"with", "parameters", "true", "false", "takes",
		"input", "returns", "AND", "OR", "for",
		"in", "switch", "case", "break", "default",
		"while", "EPSILON"
	};


struct hashMap *getTerminalMap() {
	struct hashMap *map = initialiseHashMap();
	for (int i = 0; i < NUM_TERMINALS; ++i) {
		hashMapPut(terminalStringRepresentations[i], i, map);
	}

	return map;
}

struct hashMap *getNonTerminalMap() {
	struct hashMap *map = initialiseHashMap();	
	for (int i = 0; i < NUM_NON_TERMINALS; ++i) {
		hashMapPut(nonTerminalStringRepresentations[i], i, map);
	}

	return map;
}

struct rhsNode *createRhsNode(const char *val) {
	struct rhsNode *node = (struct rhsNode *) malloc(sizeof(struct rhsNode)); // TODO: Free this memory
	if (val[0] == '<') {
		node->flag = NON_TERMINAL;
		node->symbol.non_terminal = hashMapGet(val, nonTerminalMap);
		if (node->symbol.non_terminal == -1) {
			die(val);
		}
	} else {
		node->flag = TERMINAL;
		node->symbol.terminal = hashMapGet(val, terminalMap);
		if (node->symbol.terminal == -1) {
			die(val);
		}
	}
	return node;
}

/* Extract the non-termial LHS and the rules for that LHS from a given line in the grammar.
*  @returns          the ruleNumber to continue populating from (input ruleNumber - return
*                    value = number of rules added). */
int extractRules(char *line, int ruleNumber) {
	if (!( line[0] == '<')) {
		// Probably just a blank line.
		return ruleNumber;
	}

    // idx is the index of the character we are reading in the line
	// we do this parsing in a single pass so idx is monotonically
	// increasing for each line (and set to 0 between lines).
	int idx = 0;
	char lhsTermBuf[64];  // Exculsively for storing the LHS of the rule (rule->non_terminal).
	char termBuf[64];  // We reuse this to obtain every symbol/term in the RHS of the rule.
	struct rule *currRule = &G[ruleNumber];
	struct rhsNode **term = &currRule->head;

	// First extract the non-terminal from this line.
	while (line[idx] != ' ') {
		lhsTermBuf[idx] = line[idx];
		idx += 1;
	}
	lhsTermBuf[idx] = '\0';
	currRule->non_terminal = hashMapGet(lhsTermBuf, nonTerminalMap);

	// Skip over the whitespace in the middle.
	while (isspace(line[idx]) || line[idx] == '-' || line[idx] == '>') {
		idx += 1;
	}

	// Now read each rule and load the terms one by one.
	// Change the rule and rule number accordingly.
	int i;
	while (true) {
		switch (line[idx]) {
			case '|':  // Next rule for this LHS/non-terminal
				ruleNumber += 1;
				currRule = &G[ruleNumber];
				currRule->non_terminal = hashMapGet(lhsTermBuf, nonTerminalMap);
				term = &currRule->head;
				idx += 1;
				break;
			case '\n':  // End all rules for this LHS/non-terminal
				ruleNumber += 1;
				return ruleNumber;
			case ' ':  // Next term/symbol - or just before the pipe symbol
				idx += 1;
				break;
			default:  // Extract the term/symbol and add it as a rhsNode to the rule.
				i = 0;
				while (!isspace(line[idx])) {
					termBuf[i] = line[idx];
					idx += 1;
					i += 1;
				}
				termBuf[i] = '\0'; // Because of this we don't need to clear the buffer.
				*term = createRhsNode(termBuf);
				term = &(*term)->next;
		}
	}

	return ruleNumber;  // Control will not actually ever reach this point; case '\n' is the base case.
}

void loadGrammar(const char *filename) {
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		die("Could not open the grammar file.");
	}

	terminalMap = getTerminalMap();
	nonTerminalMap = getNonTerminalMap();

	int i = 0;
	ssize_t rd;
	long unsigned int line_buf_size = 512;
	char *line_buf = (char *) malloc(line_buf_size);
	while (true) {
		rd = getline(&line_buf, &line_buf_size, fp);
		if (rd == -1) {
			break;
		}
		i = extractRules(line_buf, i);  // i represents rule number.
	}

	free(line_buf);
	free(terminalMap);
	free(nonTerminalMap);
	fclose(fp);
	return;
}

void printGrammar() {
	struct rule *currRule;
	struct rhsNode *currTerm;
	for (int i = 0; i < NUM_RULES; ++i) {
		currRule = &G[i];
		currTerm = currRule->head;
		printf("%s ::=", nonTerminalStringRepresentations[(int)currRule->non_terminal]);
		while (currTerm != NULL) {
			if (currTerm->flag == TERMINAL) {
				printf(" %s", terminalStringRepresentations[(int)currTerm->symbol.terminal]);
			} else {
				printf(" %s", nonTerminalStringRepresentations[(int)currTerm->symbol.non_terminal]);
			}
			currTerm = currTerm->next;
		}
		printf("\n");
	}
}


void computeFollow(){
	F.follow[G[0].non_terminal][DOLLAR] = '1';

	struct rhsNode *ptr;
	struct rhsNode *iter;
	
	for(int loop = 0; loop < 2; loop++) {
		for(int i = 0; i < NUM_RULES; i++)  {
			enum nonTerminal non_terminal = G[i].non_terminal;
			ptr = G[i].head;

			while(ptr != NULL) {				
				if(ptr->flag == TERMINAL) {
					ptr = ptr->next;
					continue;
				}
				
				iter = ptr;
				// printf("Calculating follow of non_terminal %d\n", ptr->symbol.non_terminal);

				while (true) {
					iter = iter->next;
					
					if(iter == NULL) {
						// printf("%d Case 1\n", non_terminal);
						for (int i = 0; i < NUM_TERMINALS; i++) {
							if (F.follow[non_terminal][i] == '1') {
								if(i == EPSILON)
									continue;
								F.follow[ptr->symbol.non_terminal][i] = '1';
							}
						}
						break;
					}

					else if(iter->flag == TERMINAL) {
						// printf("%d Case 2\n", non_terminal);
						F.follow[ptr->symbol.non_terminal][iter->symbol.terminal] = '1';
						break;
					}

					else {
						// printf("%d Case 3\n", non_terminal);
						for (int i = 0; i < NUM_TERMINALS; i++) {
							if (F.first[iter->symbol.non_terminal][i] != -1) {
								if(i == EPSILON)
									continue;
								// printf("%d\n", i);
								F.follow[ptr->symbol.non_terminal][i] = '1';
							}
						}

						if(F.first[iter->symbol.non_terminal][EPSILON] == -1){
							break;
						}
	 				}
				}
				
				ptr = ptr->next;
			}
		}
	} 

	return;
}


void computeFirstAndFollowSets() {
	printf("Computing first set ...\n");
	
	int dirty_bit_non_terminals[NUM_NON_TERMINALS];
	for (int i = 0; i < NUM_NON_TERMINALS; i++) {
		dirty_bit_non_terminals[i] = 0;
	}

	int dirty_bit_rules[NUM_RULES];
	for (int i = 0; i < NUM_RULES; i++) {
		dirty_bit_rules[i] = 1;
		dirty_bit_non_terminals[G[i].non_terminal] += 1;
	}

	int rules_used_count = 0;
	struct rhsNode *ptr_node;

	enum nonTerminal non_terminal;

	while (rules_used_count != NUM_RULES) {
		// printf("Rules used count: %d\n", rules_used_count);
		// fflush(stdout);
		
		for (int i = 0; i < NUM_RULES; i++) {
			
			non_terminal = G[i].non_terminal;
			
			if (dirty_bit_non_terminals[non_terminal] == 0 || dirty_bit_rules[i] == 0)
				continue;
			
			ptr_node = G[i].head;
			
			while (ptr_node != NULL) {
				if (ptr_node->flag == TERMINAL) {
					// printf("Rule %d: case 1\n", i);
					// fflush(stdout);
					F.first[non_terminal][ptr_node->symbol.terminal] = i;
					dirty_bit_rules[i] = 0;
					rules_used_count += 1;
					dirty_bit_non_terminals[G[i].non_terminal] -= 1;
					break;
				}
				
				else if (dirty_bit_non_terminals[ptr_node->symbol.non_terminal] != 0) {
					// printf("Rule %d: case 2\n", i);
					// fflush(stdout);
					break;
				}
				
				else {
					// printf("Rule %d: case 3\n", i);
					// fflush(stdout);
					for (int j = 0; j < NUM_TERMINALS; j++) {
						if (F.first[ptr_node->symbol.non_terminal][j] != -1 && j != EPSILON) {
							// F.first[non_terminal][j] = F.first[ptr_node->symbol.non_terminal][j];
							F.first[non_terminal][j] = i;
						}
					}

					if (F.first[ptr_node->symbol.non_terminal][EPSILON] == -1) {
						// printf("Rule %d: case 3-1\n", i);
						// fflush(stdout);
						dirty_bit_rules[i] = 0;
						rules_used_count += 1;
						dirty_bit_non_terminals[G[i].non_terminal] -= 1;
						break;
					}

					if (ptr_node->next == NULL) {
						// printf("Rule %d: case 3-2\n", i);
						// fflush(stdout);
						F.first[non_terminal][EPSILON] = i;
						dirty_bit_rules[i] = 0;
						rules_used_count += 1;
						dirty_bit_non_terminals[G[i].non_terminal] -= 1;
					}
					
					ptr_node = ptr_node->next;
				}
			}
		}
	}

	printf("Computing follow set ...\n");

	computeFollow();
}


void createParseTable() {
	for (int i = 0; i < NUM_NON_TERMINALS; i++) {
		for (int j = 0; j < NUM_TERMINALS; j++) {
			if (F.first[i][j] != -1 && j != EPSILON) {
					parseTable[i][j] = F.first[i][j];
			}
		}
		
		if (F.first[i][EPSILON] != -1) {
			for (int j = 0; j < NUM_TERMINALS; j++) {
				if (F.follow[i][j] == '1') {
					parseTable[i][j] = F.first[i][EPSILON];
				}
			}
		}
	}
}


struct parseTree * initialiseParseTree() {
	struct parseTree * parse_tree = (struct parseTree *) malloc(sizeof(struct parseTree));
	return parse_tree;
}

struct treeNode * addTreeNode(union nodeValue symbol, enum typeOfSymbol flag) {
	struct treeNode * node_ptr = (struct treeNode *) malloc(sizeof(struct treeNode));
	node_ptr->symbol = symbol;
	node_ptr->flag = flag;
	node_ptr->child = NULL;
	node_ptr->next = NULL;
	return node_ptr;
}


void parseInputSourceCode(char *testcaseFile) {
	FILE *fp = fopen(testcaseFile, "r");

	if (fp == NULL) {
		printf("Error in opening the file!!\n");
		exit(0);
	}

	printf("Initialising stack ...\n");
    initialiseStack();

    printf("Adding <program> to stack\n");
    // Push <program> to stack
    struct stackNode *stack_node = (struct stackNode *) malloc(sizeof(struct stackNode));
    stack_node->symbol.non_terminal = _PROGRAM;
    stack_node->flag = NON_TERMINAL;
    stack_node->next = NULL;
    push(stack_node);

    // printf("Check 1\n");



    // union nodeValue node_value;

    // struct parseTree * parse_tree = initialiseParseTree();
    // struct treeNode * tree_node = NULL;

    // tree_node = addTreeNode(node_value, NON_TERMINAL);
    // node_value.non_terminal = _PROGRAM

    // struct rhsNode *rhs_node = NULL;

    struct symbol symbol;
    stack_node = NULL;
    enum nonTerminal stack_top_non_terminal;
    enum terminal symbol_terminal;
    int rule_no;

    // printf("Check 2\n");
	symbol = getNextToken(fp);
	symbol_terminal = symbol.token;

	// printf("check 3\n");
    while(true) {

    	if(stack->head == NULL){
    		break;
    	}

    	printf("\n");
    	printf("Printing stack ...\n");
    	printStack();
    	printf("\n");

    	if (stack->head->flag == NON_TERMINAL) {
    		stack_top_non_terminal = stack->head->symbol.non_terminal;
    		// printf("%d %d\n", stack_top_non_terminal, symbol_terminal);
   //  		printf("Check 4\n");
			// fflush(stdout);
    		// printf("%d\n", parseTable[stack_top_non_terminal][symbol_terminal]);

    		
    		// find next applicable rule
    		if (parseTable[stack_top_non_terminal][symbol_terminal] != -1) {
    			rule_no = parseTable[stack_top_non_terminal][symbol_terminal];
    			
    			// printf("check in loop\n");
    			stack_node = pop();
    			if (G[rule_no].head->flag == TERMINAL && G[rule_no].head->symbol.terminal == EPSILON) {
    				continue;
    			}
    			else
    				pushRuleIntoStack(&G[rule_no]);
    				// addRuletoTree(&G[rule_no]);

    			// add rule to parse tree

    		}

    		// if no rule, then ERROR state
    		else {
	    		printf("ERROR!!\n");
	    		break;

    		}
    	}

    	else if (stack->head->symbol.terminal == symbol_terminal) {
    		// pop the terminal
    		stack_node = pop();
			
			// get the next token
			symbol = getNextToken(fp);
			symbol_terminal = symbol.token;
			printf("Token: %s\n", terminalStringRepresentations[symbol.token]);
			printf("Value:%s\n", symbol.lexeme.str);
    		
    		// add stack node to parse tree
    	}

    	else {
    		// ERROR state
    		printf("ERROR!!\n");
    		break;
    	}
    	
    }

}


/*
lexeme	lineno	tokenName	valueIfNumber	parentNodeSymbol	isLeafNode(yes/no)	NodeSymbol

The lexeme of the current node is printed when it is the leaf
node else a dummy string of characters "‐‐‐‐" is printed. The line number is
one of the information collected by the lexical analyzer during single pass of the source code.
The token name corresponding to the current node is printed third.
If the lexeme is an integer or real number, then its value computed by the 
lexical analyzer should be printed at the fourth place. Print the grammar 
symbol (non-terminal symbol) of the parent node of the currently visited 
node appropriately at fifth place (for the root node print ROOT for parent symbol).
The sixth column is for printing yes or no appropriately. Print the non-terminal
symbol of the node being currently visited at the 7th place,
if the node is not the leaf node [Print the actual non-terminal symbol and not the
enumerated values for the non-terminal]. Ensure appropriate justification so
that the columns appear neat and straight.
*/

/*
	Maps needed:
	map_non_terminal : enum to string (ex: 0 -> "PROGRAM")
	map_terminal_enum_str : enum to string (ex: 4 -> "PLUS")
	map_terminal_enum_lex : enum to string (ex: 4 -> "+")
*/

// char *writeNode(struct node *ptr, struct node *p_ptr, FILE *fp) {
// 	/*
// 		ptr - pointer to current node
// 		p_ptr - pointer to parent node
// 	*/

// 	char is_leaf_node[4];
// 	if (p_ptr != NULL)
// 		char parent_node_symbol[] = map_non_terminal[p_ptr->symbol.non_terminal];
// 	else
// 		char parent_node_symbol[] = "NULL";

// 	if (ptr->flag == TERMINAL) {
// 		int line_no = ptr->symbol.line_no;
// 		char token_name = map_terminal_enum_str[ptr->symbol.token]; // DEFINE MAP TOKEN
// 		strcpy(is_leaf_node, "YES");

// 		if (ptr->symbol.token == NUM) {
// 			int value_if_number = ptr->symbol.lexeme.num;
// 			fprintf(fp, "---- \t %d \t %s \t %d \t %s \t %s \n",
// 				line_no, token_name, value_if_number,
// 				parent_node_symbol, is_leaf_node);
// 		}

// 		else if (ptr->symbol.tkoen == RNUM) {
// 			float value_if_number = ptr->symbol.lexeme.rnum;
// 			fprintf(fp, "---- \t %d \t %s \t %f \t %s \t %s \n",
// 				line_no, token_name, value_if_number,
// 				parent_node_symbol, is_leaf_node);
// 		}

// 		else {
// 			char lexeme[MAX_LEXEME_LEN];
			
// 			if (ptr->symbol.token == IDENTIFIER) {
// 				strcpy(lexeme, ptr->symbol.lexeme.str);
// 			}

// 			else {
// 				strcpy(lexeme, map_terminal_enum_lex[ptr->symbol.token]);
// 			}

// 			fprintf(fp, "%s \t %d \t %s \t %f \t %s \t %s \n",
// 				lexeme, line_no, token_name, value_if_number,
// 				parent_node_symbol, is_leaf_node);
// 		}
// 	}

// 	else {
// 		strcpy(is_leaf_node, "NO");
// 		char node_symbol[] = map_non_terminal[ptr->symbol.non_terminal];
// 		fprintf(fp, " \t \t \t \t %s \t %s \t %s"
// 			parent_node, is_leaf_node, node_symbol);
// 	}
// }


// struct node *recursiveInOrderPrint(struct node *ptr, struct node *p_ptr, FILE *fp) {
// 	if (ptr == NULL)
// 		return NULL;
	
// 	struct node *last_child = recursivePrint(ptr->child, ptr, fp);
	
// 	writeNode(ptr, p_ptr, fp);
	
// 	if (last_child != NULL) {
// 		recursivePrint(last_child, ptr, fp);
// 	}
	
// 	if (ptr->next != NULL and ptr->next->next == NULL) {
// 		return ptr->next;
// 	}
	
// 	return recursivePrint(ptr->next, ptr, fp);
// }


// void printParseTree(parseTree PT, char *outfile) {
//     FILE * fp = fopen(outfile, "w");

//     if(fp == NULL) {
//         printf("Error in opening the file!\n");
//         return;
//     }

// 	struct node *ptr = PT.head;

// 	recursiveInOrderPrint(ptr, NULL, fp);
// }
