#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "data_structures/stack.h"
#include "ast.h"
#include "semanticCheck.h"
#include "st.h"
#include "intCode.h"
#include "codeGen.h"

int init_everything__done = 0;

#define GRAMMAR_FILENAME "./docs/grammar/text/grammar.txt" 

extern grammar G;
extern struct firstAndFollow F;
// extern struct parseTree PT;
extern struct SymbolTable *global_symbol_table;
extern struct hashMap *terminalMap;
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
	1 --> Print out token list from lexical analysis\n\
	2 --> Parse the given file and construct the corresponding parse tree. Print out any errors on the console if necessary\n\
	3 --> Print AST\n\
	4 --> Print number of nodes and memory allocated for parse tree and AST\n\
	5 --> Print out symbol table\n\
	6 --> Print out activation record size for each module\n\
	7 --> Print information about the arrays used(static and dynamic)\n\
	8 --> Print out lexical, syntax, type mismatch or semantic errors if any, and the time taken for compilation\n\
	9 --> Implement code generation\n");
		shown = true;
	}
	printf(">>> ");
	fflush(stdout);
	scanf("%d", &choice);
	return choice;
}

void demonstrateLexicalAnalysis(int out) {
    FILE *fp = fopen(sourceFilePath, "r");
    if (fp == NULL) {
        die("Failed to open source file.");
    }

    struct symbol token;
	if(out == 1)
    	printf("             %10s %30s %30s\n", "LINE #", "LEXEME", "TOKEN");
    while (true) {
        if (getNextToken(fp, &token) == 0) {
			if(out == 1)
        		printf("Token value: %10d %30s %30s\n",
					token.line_no, terminalLiteralRepresentations[token.token], terminalStringRepresentations[token.token]);
			break;
		}
        if (token.token == -1) {
            continue;
        }
		if(out == 1){
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
	}

    fclose(fp);
}

void runSyntaxAnalyzer(int out) {
	
	initializeFirstAndFollow();
	computeFirstAndFollowSets();
	
	intializeParseTable();
	createParseTable();
    if(out == 1)
    	printf("\n\nDemonstrating SYNTAX ANALYSIS ...\n\n");
    extern int line_no;
    line_no = 1;
    defineBuffer();
	parseInputSourceCode(sourceFilePath);
	if(out == 1)
		printParseTree();
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

// void generateEverything(){
// 	demonstrateLexicalAnalysis(0);
// 	runSyntaxAnalyzer(0);
// 	createAST();

// }

void generateAndPrintAST(){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	printf("AST:\n");
	printAST();
}

void printPTandASTMem(){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	// to be implemented
	printf("AST Node Number:%d\n",return_AST_node_number());
	printf("AST Memory Allocated:%d Bytes\n",return_AST_node_size());
}

void generateAndPrintSymbolTable(){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	generateSymbolTables();
	printf("Symbol table:\n");
	printSymbolTable(global_symbol_table);
}

void printModuleActivationRecordsSize(){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	generateSymbolTables();
	// to be implemented
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
	    defineBuffer();
	    extern int line_no; 
	    line_no = 1;
		switch (choice) {

			case 0: // exit
				exit(0);

			case 1: // print out lexical analyser
				demonstrateLexicalAnalysis(1);
				break;

			case 2: // print errors in parsing and construct the parse tree
				runSyntaxAnalyzer(1);
				break;

			case 3: // print AST(as well as traverse order)
				generateAndPrintAST();
				break;

			case 4: // print memory allocated to parse tree and AST
				printPTandASTMem();
				break;

			case 5: // print symbol table 
				generateAndPrintSymbolTable();
				break;

			case 6: // print size of each module activation record
				printModuleActivationRecordsSize();
				break;

			case 7:// print static and dynamic array stuff
				break;
			case 8: //print all errors, and print time taken for compilation
				break;
			case 9: //code gen
				break;
			default:
				printf("\nWrong option! Enter option again[0,1,2,3,4,5,6,7]\n");
				// exit(0);
		}
	}

	return 0;
}