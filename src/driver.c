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

int promptUser() {
	int choice;
	printf("Menu:\n\
	0 --> Exit out of program\n\
	1 --> Remove comments, and print out cleaned code\n\
	2 --> Print out token list from the lexer output\n\
	3 --> Parse the given file and construct the corresponding parse tree. Print out any errors on the console if necessary\n\
	4 --> Print out time taken for lexical analysis and parsing\n\
>>> ");
	fflush(stdout);
	scanf("%d", &choice);
	return choice;
}

int main(int argc, char const *argv[]) {
    initializeEverything();

	int choice = 0;

	if (argc < 2) {
		puts("Missing arguments.\nUsage: build/driver source_file");
		exit(-1);
	}

    char testcaseFile[512];
	strncpy(testcaseFile, argv[1], 512);
	clock_t start_time, end_time;
	double CPU_time, CPU_time_seconds;

	do {
		switch (promptUser()) {

			case 0: // exit
				exit(0);

			case 1: // print the contents of removeComments
			    removeComments(testcaseFile, NULL);
				break;

			case 2: // print out lexical analyser
				break;

			case 3: // print errors in parsing and construct the parse tree 
				break;

			case 4: // lexer and parsing and then print out time taken 
				start_time = clock();
				// code for parsing and lexeical analysis
				end_time = clock();
				CPU_time = (double) (end_time - start_time);
				CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;

				printf("Time taken: %lf seconds.\n", CPU_time_seconds);
				break;

			default:
				exit(0);
		}
	} while (choice !=0 );

	return 0;
}