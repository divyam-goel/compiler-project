#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "data_structures/stack.h"

#define GRAMMAR_FILENAME "./docs/grammar/text/grammar.txt" 

extern grammar G;
extern struct firstAndFollow F;
extern struct hashMap *terminalMap;
extern struct hashMap *terminalLiteralMap;
extern struct hashMap *nonTerminalMap;
extern char terminalStringRepresentations[NUM_TERMINALS][16];
extern char nonTerminalStringRepresentations[NUM_NON_TERMINALS][32];
extern char terminalLiteralRepresentations[NUM_TERMINALS][16];
char sourceFilePath[512];
char outputFilePath[512];

void initializeEverything() {
    terminalMap = getTerminalMap();
    nonTerminalMap = getNonTerminalMap();
    terminalLiteralMap = getTerminalLiteralMap();
    defineBuffer();
    loadGrammar(GRAMMAR_FILENAME);
}

void printProgressInfo() {
	printf("Welcome to Group #17's ERPLAG compiler. Here is our progress:\n");
	int num_msg = 3;
	char *messages[] = {
		"Lexical analysis module implemented and works on all test cases.",
		"Syntax analysis module implemented and works on all test cases.",
		"FIRST and FOLLOW set generation automated.",
	};
	for (int i = 0; i < num_msg; ++i) {
		printf("\t %d. %s\n", i, messages[i]);
	}
	printf("\n");
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
	4 --> Print out time taken for lexical analysis and parsing\n\
	5 --> Print the grammar\n\
	6 --> Print the first sets\n\
	7 --> Print the follow sets\n");
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
    printf("             %10s %30s %30s\n", "LINE #", "LEXEME", "TOKEN");
    while (true) {
        if (getNextToken(fp, &token) == 0) {
        	printf("Token value: %10d %30s %30s\n",
					token.line_no, terminalLiteralRepresentations[token.token], terminalStringRepresentations[token.token]);
			break;
		}
        if (token.token == -1) {
            continue;
        }
		switch (token.token) {
	        case IDENTIFIER:
	            printf("Token value: %10d %30s %30s\n",
	            	token.line_no, token.lexeme.str, terminalStringRepresentations[token.token]);
				break;
	        case NUM:
	            printf("Token value: %10d %30d %30s\n",
	            	token.line_no, token.lexeme.num, terminalStringRepresentations[token.token]);
				break;
	        case RNUM:
	            printf("Token value: %10d %30f %30s\n",
	            	token.line_no, token.lexeme.rnum, terminalStringRepresentations[token.token]);
				break;
	        default:
	            printf("Token value: %10d %30s %30s\n",
					token.line_no, terminalLiteralRepresentations[token.token], terminalStringRepresentations[token.token]);
    	}
	}

    fclose(fp);
}

void runLexicalAndSyntaxAnalyzers() {
    printf("Demonstrating LEXICAL ANALYSIS ...\n\n");
	demonstrateLexicalAnalysis();
	
	initializeFirstAndFollow();
	computeFirstAndFollowSets();
	
	intializeParseTable();
	createParseTable();
    
    printf("\n\nDemonstrating SYNTACTIC ANALYSIS ...\n\n");
	parseInputSourceCode(sourceFilePath);
	printParseTree(outputFilePath);
}

void printFirstSetHumanFriendly() {
    initializeFirstAndFollow();
    computeFirstAndFollowSets();

    for (int i = 0; i < NUM_NON_TERMINALS; ++i) {
        printf("%s: { ", nonTerminalStringRepresentations[i]);
        for (int j = 0; j < NUM_TERMINALS; ++j) {
            if (F.first[i][j] != -1) {
                printf("%s ", terminalStringRepresentations[j]);
            }
        }
        printf("}\n");
    }

    return;
}

void printFollowSetHumanFriendly() {
    initializeFirstAndFollow();
    computeFirstAndFollowSets();

    for (int i = 0; i < NUM_NON_TERMINALS; ++i) {
        printf("%s: { ", nonTerminalStringRepresentations[i]);
        for (int j = 0; j < NUM_TERMINALS; ++j) {
            if (F.follow[i][j] != '0') {
                printf("%s ", terminalStringRepresentations[j]);
            }
        }
        printf("}\n");
        fflush(stdout);
    }

    return;
}

int main(int argc, char const *argv[]) {

	if (argc < 3) {
		puts("Missing arguments.\nUsage: build/driver source_file output_file");
		exit(-1);
	}

    initializeEverything();
	printProgressInfo();

	int choice = 0;

	strncpy(sourceFilePath, argv[1], 512);
	strncpy(outputFilePath, argv[2], 512);

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
				runLexicalAndSyntaxAnalyzers();
				break;

			case 4: // lexer and parsing and then print out time taken 
				start_time = clock();

				runLexicalAndSyntaxAnalyzers();

				end_time = clock();
				CPU_time = (double) (end_time - start_time);
				CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;

				printf("Time taken: %lf seconds.\n", CPU_time_seconds);
				break;

			case 5:
				printGrammar();
				break;

			case 6:
				printFirstSetHumanFriendly();
				break;

			case 7:
				printFollowSetHumanFriendly();
				break;

			default:
				exit(0);
		}
	}

	return 0;
}