#include "parser.h"

grammar G;
struct firstAndFollow F;
// table T;

void add_To_Follow(int fromFirst, enum nonTerminals from, enum nonTerminals to){
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

	F.follow[G[0].non_terminal][DOLLAR_] = '1';

	for(int i = 0; i < NUM_RULES; i++) {

		enum nonTerminals non_terminal = G[i].non_terminal;
		struct rhsNode *head = G[i].head;

		while(head != NULL){
			
			if(head->flag == TERMINAL){
				head = head->next;
				continue;
			}

			struct rhsNode *M = head;
			while(1){

				M = M->next;
				if(M == NULL){
					add_To_Follow(0, non_terminal, head->symbol.non_terminal);
					break;
				}

				else if(M->flag == TERMINAL){
					F.follow[head->symbol.non_terminal][M->symbol.terminal] = '1';
					break;
				}

				else{
					add_To_Follow(1, M->symbol.non_terminal, head->symbol.non_terminal);

					if(F.first[M->symbol.non_terminal][NUM_TERMINALS] != 1){
						break;
					}
 				}

			}

			// free(M);
			head = head->next;

		}

		// free(head);
	}

	return;
}


void computeFirstAndFollow() {
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
		printf("Rules used count: %d\n", rules_used_count);
		
		for (int i = 0; i < NUM_RULES; i++) {
			
			non_terminal = G[i].non_terminal;
			
			if (dirty_bit_non_terminals[non_terminal] == 0 || dirty_bit_rules[i] == 0)
				continue;
			
			ptr_node = G[i].head;
			
			while (ptr_node != NULL) {
				if (ptr_node->flag == TERMINAL) {
					printf("Rule %d: case 1\n", i);
					F.first[non_terminal][ptr_node->symbol.terminal] = i;
					dirty_bit_rules[i] = 0;
					rules_used_count += 1;
					dirty_bit_non_terminals[G[i].non_terminal] -= 1;
					break;
				}
				
				else if (dirty_bit_non_terminals[ptr_node->symbol.non_terminal] != 0) {
					printf("Rule %d: case 2\n", i);
					break;
				}
				
				else {
					printf("Rule %d: case 3\n", i);
					for (int j = 0; j < NUM_TERMINALS; j++) {
						if (F.first[ptr_node->symbol.non_terminal][j] != -1 && j != EPS_) {
							F.first[non_terminal][j] = F.first[ptr_node->symbol.non_terminal][j];
						}
					}

					if (F.first[ptr_node->symbol.non_terminal][EPS_] == -1) {
						printf("Rule %d: case 3-1\n", i);
						dirty_bit_rules[i] = 0;
						rules_used_count += 1;
						dirty_bit_non_terminals[G[i].non_terminal] -= 1;
						break;
					}

					if (ptr_node->next == NULL) {
						printf("Rule %d: case 3-2\n", i);
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


// void computeParseTable(struct firstAndFollow F, table T) {
// 	for (int i = 0; i < NUM_NON_TERMINALS; i++) {
// 		for (int j = 0; j < NUM_TERMINALS; j++) {
// 			if (F.first[i][j] != -1)
// 				T[i][j] = F.first[i][j];
// 		}
// 		if (F.first[i][EPSELON] != -1) {
// 			for (int j = 0; j < NUM_TERMINALS; j++) {
// 				if (F.follow[i][j] == '1') {
// 					T[i][j] = NUM_TERMINALS;
// 				}
// 			}
// 		}
// 	}
// }
