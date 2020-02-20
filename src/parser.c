#include "parser.h"

grammar G;
table parseTable;
struct parseTree PT;
struct hashMap *nonTerminalMap;
struct hashMap *terminalMap;
struct firstAndFollow F;

char nonTerminalStringRepresentations[NUM_NON_TERMINALS][32] = {
		"<program>", "<lvalueIDStmt>", "<N8>", "<op1>", "<idList>",
		"<conditionalStmt>", "<iterativeStmt>", "<type>", "<logicalOp>", "<term>",
		"<default>", "<module>", "<var>", "<sub_u>", "<whichId>",
		"<whichStmt>", "<moduleDeclaration>", "<new_expression>", "<input_plist>", "<sub_input_plist>",
		"<expression>", "<assignmentStmt>", "<nullableCaseStmt>", "<dataType>", "<u>",
		"<statement>", "<optional>", "<arithmeticExpr>", "<moduleDeclarations>", "<sub_term>",
		"<op2>", "<declareStmt>", "<range>", "<AnyTerm>", "<caseStmt>",
		"<dynamic_range>", "<ret>", "<relationalOp>", "<moduleReuseStmt>", "<lvalueARRStmt>",
		"<sub_arithmeticExpr>", "<output_plist>", "<sub_output_plist>", "<moduleDef>", "<otherModules>",
		"<sub_idList>", "<N7>", "<simpleStmt>", "<statements>", "<extended_var>",
		"<driverModule>", "<value>", "<ioStmt>", "<index>", "<factor>",
		"<boolConstt>"
	};

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

void add_To_Follow(int fromFirst, enum nonTerminal from, enum nonTerminal to){
	if (fromFirst) {
		for (int i = 0; i < NUM_TERMINALS; i++) {
			if (F.first[from][i] != -1){
				F.follow[to][i] = '1';
			}
		}
	}

	else {
		for (int i = 0; i < NUM_TERMINALS; i++) {
			if (F.follow[from][i] == '1'){
				F.follow[to][i] = '1';
			}
		}
	}

}


void computeFollow(){
	F.follow[G[0].non_terminal][DOLLAR] = '1';

	struct rhsNode *ptr;
	struct rhsNode *iter;
	
	for(int i = 0; i < 2; i++) {
		for(int i = 0; i < NUM_RULES; i++) 
			enum nonTerminal non_terminal = G[i].non_terminal;
			ptr = G[i].head;

			while(ptr != NULL){				
				if(ptr->flag == TERMINAL) {
					ptr = ptr->next;
					continue;
				}
				
				iter = ptr;

				while (true) {
					iter = iter->next;
					
					if(iter == NULL) {
						for (int i = 0; i < NUM_TERMINALS; i++) {
							if (F.first[non_terminal][i] != -1) {
								if(i == EPS_)
									continue;
								F.follow[ptr->symbol.non_terminal][i] = '1';
							}
						}
						break;
					}

					else if(iter->flag == TERMINAL) {
						F.follow[ptr->symbol.non_terminal][iter->symbol.terminal] = '1';
						break;
					}

					else {
						for (int i = 0; i < NUM_TERMINALS; i++) {
							if (F.follow[iter->symbol.non_terminal][i] == '1') {
								if(i == EPS_)
									continue;
								F.follow[ptr->symbol.non_terminal][i] = '1';
							}
						}

						if(F.first[iter->symbol.non_terminal][EPS_] == -1){
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
		
		for (int i = 0; i < NUM_RULES; i++) {
			
			non_terminal = G[i].non_terminal;
			
			if (dirty_bit_non_terminals[non_terminal] == 0 || dirty_bit_rules[i] == 0)
				continue;
			
			ptr_node = G[i].head;
			
			while (ptr_node != NULL) {
				if (ptr_node->flag == TERMINAL) {
					// printf("Rule %d: case 1\n", i);
					F.first[non_terminal][ptr_node->symbol.terminal] = i;
					dirty_bit_rules[i] = 0;
					rules_used_count += 1;
					dirty_bit_non_terminals[G[i].non_terminal] -= 1;
					break;
				}
				
				else if (dirty_bit_non_terminals[ptr_node->symbol.non_terminal] != 0) {
					// printf("Rule %d: case 2\n", i);
					break;
				}
				
				else {
					// printf("Rule %d: case 3\n", i);
					for (int j = 0; j < NUM_TERMINALS; j++) {
						if (F.first[ptr_node->symbol.non_terminal][j] != -1 && j != EPSILON) {
							// F.first[non_terminal][j] = F.first[ptr_node->symbol.non_terminal][j];
							F.first[non_terminal][j] = i;
						}
					}

					if (F.first[ptr_node->symbol.non_terminal][EPSILON] == -1) {
						// printf("Rule %d: case 3-1\n", i);
						dirty_bit_rules[i] = 0;
						rules_used_count += 1;
						dirty_bit_non_terminals[G[i].non_terminal] -= 1;
						break;
					}

					if (ptr_node->next == NULL) {
						// printf("Rule %d: case 3-2\n", i);
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

	computeFollow();
}


void createParseTable() {
	for (int i = 0; i < NUM_NON_TERMINALS; i++) {
		for (int j = 0; j < NUM_TERMINALS; j++) {
			if (F.first[i][j] != -1 && j != EPS_) {
					parseTable[i][j] = F.first[i][j];
			}
		}
		
		if (F.first[i][EPS_] != -1) {
			for (int j = 0; j < NUM_TERMINALS; j++) {
				if (F.follow[i][j] == '1') {
					parseTable[i][j] = F.first[i][EPS_];
				}
			}
		}
	}
}


// parseInputSourceCode(char *testcaseFile, table T) {
// 	//
// }


// char *writeNode(struct node *ptr) {

// }


void recursivePrint(struct node *ptr, FILE *fp) {
	if (ptr == NULL)
		return;
	recursivePrint(ptr->child, fp);
	char *str = "%s\t%d\t%s\t%d";
	// fwrite(fp, )
}


void printParseTree(parseTree PT, char *outfile) {
    FILE * fp = fopen(outfile, "w");

    if(fp == NULL) {
        printf("Error in opening the file!\n");
        return;
    }

	struct node *ptr = PT.head;

	recursivePrint(ptr, fp);
}
