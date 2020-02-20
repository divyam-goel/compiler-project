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
