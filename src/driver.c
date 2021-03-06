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
extern struct ProgramNode AST;
extern ICInstr *start_global_ic_instr;
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
	char *messages ="Level 4: AST/ Symbol table/type Checking/ Semantic rules/handled static and dynamic arrays in type checking/ code generation";
	printf("\t%s\n",messages);
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
	int fail = 1;
	fail = scanf("%d", &choice);
	if(fail == 0){ //chat was input - need to clear the buffer
		int t;
		while ((t = getchar()) != '\n' );
		return 10;
	}
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
	parseInputSourceCode(sourceFilePath,out);
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
	printf("Post Order Traversal:\n");
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	printf("AST:\n");
	printAST();
}

void printPTandASTMem(){
	float percent;
	set_AST_node_num();
	set_PT_node_num();
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	// to be implemented
	printf("Number of nodes in Parse Tree: %d\n", return_PT_node_number());
	printf("Memory allocated for Parse Tree: %ld Bytes\n", return_PT_node_size());
	printf("Number of nodes in AST: %d\n", return_AST_node_number());
	printf("Memory allocated for AST: %ld Bytes\n", return_AST_node_size());
	percent = (float)(return_PT_node_size() - return_AST_node_size());
	percent /= (float)return_PT_node_size();
	percent *= 100;
	printf("Percentage compression: %f%%\n", percent);
}

void PrintSymbolTableOrArrays(int print_arr){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	generateSymbolTables();
	if(print_arr == 0)
		printf("\nSymbol table:\n");
	else 
		printf("\nPrinting all arrays used in program:\n");
  	printSymbolTablesForDriver(print_arr);
}

void printModuleActivationRecordsSize(){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	generateSymbolTables();
	char *key;
	int num_keys = global_symbol_table->keys->filled;
	struct SymbolTableNode *data;

	if (num_keys == 0){
		printf("{}\n");
		return;
	}
	for (int i = 0; i < num_keys - 1; ++i){
		key = strl_get(global_symbol_table->keys, i);
		data = symbolTableGet(global_symbol_table, key);
		printf("Size of %s module: %d bytes\n", data->key, data->value.module.activation_record_size);
	}
	key = strl_get(global_symbol_table->keys, num_keys - 1);
	data = symbolTableGet(global_symbol_table, key);
	printf("Size of %s module: %d bytes\n", data->key, data->value.module.activation_record_size);
}

void printAllErrorsAndTime(){

	clock_t start_time, end_time, compiler_start_time, compiler_end_time;
	double CPU_time, CPU_time_seconds;
	compiler_start_time = clock();

	start_time = compiler_start_time;
	demonstrateLexicalAnalysis(0);
	end_time = clock();
	CPU_time = (double)(end_time - start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Lexical Analysis: Time taken: %lf seconds\n", CPU_time_seconds);

	start_time = clock();
	runSyntaxAnalyzer(0);
	end_time = clock();
	CPU_time = (double)(end_time - start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Syntax Analysis: Time taken: %lf seconds\n", CPU_time_seconds);

	start_time = clock();
	createAST();
	end_time = clock();
	CPU_time = (double)(end_time - start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Creation of AST: Time taken: %lf seconds\n", CPU_time_seconds);

	start_time = clock();
	generateSymbolTables();
	end_time = clock();
	CPU_time = (double)(end_time - start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Symbol Table Creation: Time taken: %lf seconds\n", CPU_time_seconds);

	start_time = clock();
	semanticChecker(&AST);
	end_time = clock();
	CPU_time = (double)(end_time - start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Semantic Analysis: Time taken: %lf seconds\n", CPU_time_seconds);

	start_time = clock();
	generateIntermediateCode(&AST);
	generateASMCode(start_global_ic_instr, outputFilePath);
	end_time = clock();
	CPU_time = (double)(end_time - start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Code Generation: Time taken: %lf seconds\n", CPU_time_seconds);

	printf("\nIn total:\n");
	compiler_end_time = clock();
	CPU_time = (double)(compiler_end_time - compiler_start_time);
	CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;
	printf("Compiler: Total clocks cycles taken:%lf\n", CPU_time);
	printf("Compiler: Total time taken:%lf seconds\n", CPU_time_seconds);
	set_semantic_num();
}

void doCodeGen(){
	demonstrateLexicalAnalysis(0);
	runSyntaxAnalyzer(0);
	createAST();
	generateSymbolTables();
	semanticChecker(&AST);
	generateIntermediateCode(&AST);
	generateASMCode(start_global_ic_instr,outputFilePath);
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
		puts("Missing arguments.\nExample usage: ./compiler code.erplag code.asm");
    puts("Then assemble with: nasm -f elf64 code.asm -o code.o && gcc code.o -o code.out");
		exit(-1);
	}

    initializeEverything();
	printProgressInfo();

	int choice = 0;

	strncpy(sourceFilePath, argv[1], 512);
	strncpy(outputFilePath, argv[2], 512);

	while ((choice = promptUser())) {
		// exit(1);
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
				PrintSymbolTableOrArrays(0);
				break;

			case 6: // print size of each module activation record
				printModuleActivationRecordsSize();
				break;

			case 7:// print static and dynamic array stuff
				PrintSymbolTableOrArrays(1);
				break;
			case 8: //print all errors, and print time taken for compilation
				printAllErrorsAndTime();
				break;
			case 9: //code gen
				doCodeGen();
				printf("Code generation complete!! Generated code written into %s\n",outputFilePath);
				break;
			default:
				printf("\nWrong option! Enter option again[0,1,2,3,4,5,6,7,8,9]\n");
				// exit(0);
		}
	}

	return 0;
}
