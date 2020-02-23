#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "data_structures/stack.h"

#define GRAMMAR_FILENAME "./docs/grammar/text/grammar.txt" 

extern grammar G;
extern struct hashMap *terminalMap;
extern struct hashMap *terminalLiteralMap;
extern struct hashMap *nonTerminalMap;
extern char terminalStringRepresentations[NUM_TERMINALS][16];
extern char terminalLiteralRepresentations[NUM_TERMINALS][16];
char sourceFilePath[512];

void initializeEverything() {
    terminalMap = getTerminalMap();
    nonTerminalMap = getNonTerminalMap();
    terminalLiteralMap = getTerminalLiteralMap();
    defineBuffer();
    loadGrammar(GRAMMAR_FILENAME);
}

int promptUser() {
	static bool shown = false;

	int choice;
	if (shown == false) {
		printf("Menu:\n\
	0 --> Exit out of program\n\
	1 --> Remove comments, and print out cleaned code\n\
	2 --> Print out token list from the lexer output\n\
	3 --> Parse the given file and construct the corresponding parse tree. Print out any errors on the console if necessary\n\
	4 --> Print out time taken for lexical analysis and parsing\n");
		shown = true;
	}
	printf(">>> ");
	fflush(stdout);
	scanf("%d", &choice);
	return choice;
}

void demonstrateLexicalAnalysis() {
    FILE *fp = fopen(sourceFilePath, "r");
    if (fp == NULL) {
        die("Failed to open source file.");
    }

    struct symbol token;
    printf("             %15s %30s %10s\n", "TOKEN", "LEXEME", "LINE #");
    while (true) {
        if (getNextToken(fp, &token) == 0) {
			break;
		}
        if (token.token == -1) {
            continue;
        }
		switch (token.token) {
	        case IDENTIFIER:
	            printf("Token value: %15s %30s %10d\n", terminalStringRepresentations[token.token], token.lexeme.str, token.line_no);
				break;
	        case NUM:
	            printf("Token value: %15s %30d %10d\n", terminalStringRepresentations[token.token], token.lexeme.num, token.line_no);
				break;
	        case RNUM:
	            printf("Token value: %15s %30f %10d\n", terminalStringRepresentations[token.token], token.lexeme.rnum, token.line_no);
				break;
	        default:
	            printf("Token value: %15s %30s %10d\n",  terminalStringRepresentations[token.token],
					   terminalLiteralRepresentations[token.token],token.line_no);
    	}
	}

    fclose(fp);
}

int main(int argc, char const *argv[]) {
    initializeEverything();

	int choice = 0;

	if (argc < 2) {
		puts("Missing arguments.\nUsage: build/driver source_file");
		exit(-1);
	}

	strncpy(sourceFilePath, argv[1], 512);
	clock_t start_time, end_time;
	double CPU_time, CPU_time_seconds;

	while ((choice = promptUser())) {
		switch (choice) {

			case 0: // exit
				exit(0);

			case 1: // print the contents of removeComments
			    removeComments(sourceFilePath, NULL);
				fflush(stdout);
				break;

			case 2: // print out lexical analyser
				demonstrateLexicalAnalysis();
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
	}

	return 0;
}