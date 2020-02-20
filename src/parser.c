#include "parser.h"

grammar G;
struct firstAndFollow F;
table parseTable;
struct parseTree PT;

void computeFollow(){
	F.follow[G[0].non_terminal][DOLLAR_] = '1';

	struct rhsNode *ptr;
	struct rhsNode *iter;
	
	for(int i = 0; i < 2; i++) {
		for(int i = 0; i < NUM_RULES; i++) {

			enum nonTerminals non_terminal = G[i].non_terminal;
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

	enum nonTerminals non_terminal;

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
						if (F.first[ptr_node->symbol.non_terminal][j] != -1 && j != EPS_) {
							// F.first[non_terminal][j] = F.first[ptr_node->symbol.non_terminal][j];
							F.first[non_terminal][j] = i;
						}
					}

					if (F.first[ptr_node->symbol.non_terminal][EPS_] == -1) {
						// printf("Rule %d: case 3-1\n", i);
						dirty_bit_rules[i] = 0;
						rules_used_count += 1;
						dirty_bit_non_terminals[G[i].non_terminal] -= 1;
						break;
					}

					if (ptr_node->next == NULL) {
						// printf("Rule %d: case 3-2\n", i);
						F.first[non_terminal][EPS_] = i;
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
