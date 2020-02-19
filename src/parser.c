#include "parser.h"

void computeParseTable(struct firstAndFollow F, table T) {
	for (int i = 0; i < NUM_NON_TERMINALS; i++) {
		for (int j = 0; j < NUM_TERMINALS; j++) {
			if (F.first[i][j] != -1)
				T[i][j] = F.first[i][j];
		}
		if (F.first[i][NUM_TERMINALS] != -1) {
			for (int j = 0; j < NUM_TERMINALS; j++) {
				if (F.follow[i][j] == '1') {
					T[i][j] = NUM_TERMINALS;
				}
			}
		}
	}
}