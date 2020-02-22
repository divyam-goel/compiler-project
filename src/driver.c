#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "data_structures/stack.h"

#define GRAMMAR_FILENAME "./docs/grammar/text/grammar.txt" 

extern grammar G;
extern struct hashMap *terminalMap;
extern struct hashMap *nonTerminalMap;

void initializeEverything() {
    terminalMap = getTerminalMap();
    nonTerminalMap = getNonTerminalMap();
    defineBuffer();
    loadGrammar(GRAMMAR_FILENAME);
}

int main(int argc, char const *argv[]) {
    initializeEverything();

	int choice = 0;

	if (argc < 2) {
		puts("missing arguments.");
		exit(-1);
	}

	do{
		printf("Menu:\n0 --> Exit out of program\n1 --> Remove comments, and print out cleaned code\n");
		printf("2 --> Print out token list from the lexer output\n");
		printf("3 --> Parse the given file and construct the corresponding parse tree. Print out any errors on the console if necessary\n");
		printf("4 --> Print out time taken for lexical analysis and parsing\n");
		printf("Input your choice here:\n");
		fflush(stdout);
		scanf("%d", &choice);
		switch(choice){
			case 0:{
				// exit
				exit(0);
				break;
			}
			case 1:{
				// print the contents of removeComments
    			char *cleanFile = "./no_comments.erplag";
			    char testcaseFile[512];
				strncpy(testcaseFile, argv[1], 512);
			    removeComments(testcaseFile, cleanFile);
				printf("Check %s for the clean file.\n", cleanFile);
				break;
			}
			case 2:{
				// print out lexical analyser
				break;
			}
			case 3:{
				// print errors in parsing and construct the parse tree 
				break;
			}
			case 4:{
				// lexer and parsing and then print out time taken 
				clock_t start_time,end_time;
				double CPU_time,CPU_time_seconds;
				start_time = clock();
				// code for parsing and lexeical analysis
				end_time = clock();
				CPU_time = (double) (end_time - start_time);
				CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;

				printf("It took %lf seconds.\n", CPU_time_seconds);
				break;
			}
			default:{
				printf("Wrong input,try again:\n\n");
			}
		}
		// char while_cond = 'n'; 
		printf("Continue?(Enter 0 to stop,anything else to continue):\n");
		scanf("%d", &choice);
		system("clear");



	} while(choice !=0 );
	return 0;
}