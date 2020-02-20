// just assuming for now that F.follow is a character 2-D array- '0' means no, '1' means yes
// Assuming EPSILON is located at the end of follow[non_terminal]- assuming number of terminals= NUM_TERMINALS- 
// follow[non-term][NUM_TERMINALS]- EPSILON

// grammer is just a list of rules

void add_To_Follow(FirstandFollow F,int fromFirst, enum nonTerminals from,enum nonTerminals to){

	// if from a first to a follow
	if(fromFirst){
		// assuming all terminals are labelled from 0 to NUM_TERMINALS 
		for(int i=0;i<=NUM_TERMINALS;i++){
			if(F.first[from][i] == 1){
				F.follow[to][i] = '1';
			}
		}
	}
	// else- its from a follow-
	else{
		for(int i=0;i<=NUM_TERMINALS;i++){
			if(F.follow[from][i] == 1){
				F.follow[to][i] = '1';
			}
		}
	}

}
void computeFollow(FirstandFollow F,rules grammer[NUM_RULES]){

	// first non-terminal- starting sybmol- follow of this is $- END_SYMBOL
	F.follow[grammer[0].non_terminal][END_SYMBOL] = '1';
	// iterate thru each rule:
	for(int i =0;i<NUM_RULES;i++){

		enum nonTerminals non_terminal = grammer[i].non_terminal;
		struct rhsNode *head= grammer[i].head;
		// now, to iterate thru all the items on RHS- we want to calculate the follow of every non-terminal here
		while(head!=NULL){
			// if current symbol is terminal, ignore it
			if(head->typeOfSymbol == TERMINAL){
				head=head->next;
				continue;
			}
			// else, if head->symbol is a non-terminal, we now add elements to the follow of head->symbol
			struct rhsNode *M = head;
			while(1){

				// go to next symbol
				M = M->next;
				if(M == NULL){
					// we've reached the end of rule
					// add follow of <non_term> to follow(head->symbol) and then exit loop
					add_To_Follow(F,0,non_terminal,head->symbol);
					break;
				}
				else if(M->typeOfSymbol == TERMINAL){
					// add M->symbol to follow(head->symbol) and then exit
					F.follow[head->symbol][M->symbol] = '1';
					break;
				}
				else{
					// if another non-terminal encountered-
					// add first(M->symbol) to follow(head->symbol)
					add_To_Follow(F,1,M->symbol,head->symbol);
					// if first(M->symbol) doesn't contain EPSILON, then we break- check
					if(F.first[M->symbol][NUM_TERMINALS] !=1){
						break;
					}
					// otherwise, it contains epsilon, and it has to look for the next terminal- loop continues

 				}

			} //end of loop finding follow of a particular non_terminal

			// free space for M
			free(M);
			// advance head pointer to check for the next non-terminal
			head= head->next;

		} //end of the loop for finding all non- terminals in a rule

		// freeing up the space used for head
		free(head);
	} //end of loop going thru all the rules

	return;
}