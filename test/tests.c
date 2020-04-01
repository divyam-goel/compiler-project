#include <assert.h>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/ast.h"
#include "../src/data_structures/stack.h"

void test_removeComments(){
    /*
    *  Test the removeComments function against a manually generated expected file
    *  to ensure that the function indeed works as expected.
    */
    llog("\t+ Running test_getOutputFileName... ");
    char *cleanFile = "./test/fixtures/test_result_program_without_comments.erplag";
    char *testcaseFile = "./test/fixtures/program_with_comments.erplag";
    char *expectedFile = "./test/fixtures/expected_program_without_comments.erplag";
    removeComments(testcaseFile, cleanFile);
    assert(file_compare(cleanFile, expectedFile) == 0);
    llog("Success!!!\n");
}

void test_getStream(char *file_name) {
    FILE * fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error in opening the file!\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the twin buffer to a blank state.
    defineBuffer();

    getStream(fp);
    int num_bytes = getNumBytes();
    while(!feof(fp)) {
        for(int i = 0; i < num_bytes; i++) {
            printf("%c", getNextChar(fp));
        }
        num_bytes = getNumBytes();
    }
}

void test_getNextToken(char *file_name) {
    FILE * fp = fopen(file_name, "r");

    if (fp == NULL) {
        printf("Error in opening the file!\n");
        return;
    }


    // Set up the global twin buffer and fill the global terminals hashmap (in lexer.c).
    defineBuffer();
    extern char terminalStringRepresentations[NUM_TERMINALS][16];  // in parser.c
    extern char terminalLiteralRepresentations[NUM_TERMINALS][16];  // in parser.c

    extern struct hashMap *terminalMap;
    terminalMap = getTerminalMap();

    extern struct hashMap *terminalLiteralMap;
    terminalLiteralMap = getTerminalLiteralMap();

    struct symbol token;
    printf("             %10s %30s %30s\n\n", "LINE #", "LEXEME", "TOKEN");
    while (true) {
        if (getNextToken(fp, &token) == 0) {
            break;
        }
        if (token.token == -1) {
            continue;
        }
        if (token.token == IDENTIFIER)
            printf("Token value: %10d %30s %30s\n",
                token.line_no, token.lexeme.str, terminalStringRepresentations[token.token]);
        else if (token.token == NUM)
            printf("Token value: %10d %30d %30s\n",
                token.line_no, token.lexeme.num, terminalStringRepresentations[token.token]);
        else if (token.token == RNUM)
            printf("Token value: %10d %30f %30s\n",
                token.line_no, token.lexeme.rnum, terminalStringRepresentations[token.token]);
        else
            printf("Token value: %10d %30s %30s\n",
                token.line_no, terminalLiteralRepresentations[token.token], terminalStringRepresentations[token.token]);
    }

    fclose(fp);
}

void test_loadGrammar(char *file_name) {
    loadGrammar(file_name);
    printGrammar();
}

void test_computeFirstAndFollow(char *file_name) {
    extern struct firstAndFollow F;
    // extern table parseTable;

    extern char terminalStringRepresentations[NUM_TERMINALS][16];  // in parser.c
    extern char nonTerminalStringRepresentations[NUM_NON_TERMINALS][32];
    
    FILE *fp = fopen("test/test_result_follow.txt","w+");

    printf("Populating grammar\n");
    // populateGrammar();
    loadGrammar(file_name);

    printf("Intializing first and follow\n");
    initializeFirstAndFollow();

    printf("Computing first and follow ...\n");
    computeFirstAndFollowSets();

    printf("Printing first and follow ...\n");

    // fprintf(fp, "Printing first and follow ...\n");
    // fprintf(fp, "\nFirst:\n");

    fprintf(fp, "%27s ", terminalStringRepresentations[0]);
    for (int j = 1; j < NUM_TERMINALS; ++j) {
        fprintf(fp, "%7s ", terminalStringRepresentations[j]);
    }
    fprintf(fp, "\n");

    // for (int i = 0; i < NUM_NON_TERMINALS; i++) {
    //     fprintf(fp, "%20s ", nonTerminalStringRepresentations[i]);
    //     for (int j = 0; j < NUM_TERMINALS; j++) {
    //         fprintf(fp, "%7d ", F.first[i][j]);
    //     }
    //     fprintf(fp, "\n");
    // }

    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        fprintf(fp, "%20s ", nonTerminalStringRepresentations[i]);
        for (int j = 0; j < NUM_TERMINALS; j++) {
            fprintf(fp, "%7c ", F.follow[i][j]);
        }
        fprintf(fp, "\n");
    }

    // printf("Intializing parse table ...\n");
    // intializeParseTable();

    // printf("Creating parse table....\n");
    // createParseTable();

    // code to print out table
    // printf("\nParse table:\n");
    // for (int i = 0; i < NUM_NON_TERMINALS; i++) {
    //     for (int j = 0; j < NUM_TERMINALS; j++) {
    //         printf("%d\t", parseTable[i][j]);
    //     }
    //     printf("\n");
    // }
}

void test_parseInputSourceCode(char *grammar_file, char *source_file) {
    printf("Loading grammar ...\n");
    loadGrammar(grammar_file);

    printf("Intializing first and follow\n");
    initializeFirstAndFollow();

    printf("Computing first and follow ...\n");
    computeFirstAndFollowSets();

    printf("Intializing parse table ...\n");
    intializeParseTable();

    printf("Creating parse table ...\n");
    createParseTable();

    printf("Creating a buffer ...\n");
    defineBuffer();

    printf("Populating terminal and terminal literal hash map ...\n");
    extern struct hashMap *terminalMap;
    terminalMap = getTerminalMap();

    extern struct hashMap *terminalLiteralMap;
    terminalLiteralMap = getTerminalLiteralMap();

    printf("Parsing input source code ...\n");
    parseInputSourceCode(source_file);

    // printParseTree("test/test_result_parse_tree_output.txt");
}

void test_createAST(char *grammar_file, char *source_file) {
    test_parseInputSourceCode(grammar_file, source_file);

    printf("Creating AST ...\n");
    createAST();
    
    printf("Printing AST ...\n");
    printAST();
}

int main() {
    puts("\nRunning tests...");
    char grammar_file[] = "./docs/grammar/text/grammar.txt";
    char source_file[] = "./test/basic_tests/test_3.erplag";
    // test_removeComments();
    // test_getStream(source_file);
    // test_getNextToken(source_file);
    // test_loadGrammar(grammar_file);
    // test_computeFirstAndFollow(grammar_file);
    // test_parseInputSourceCode(grammar_file, source_file);
    test_createAST(grammar_file, source_file);
    printf("\nTests complete!!!\n");
    return 0;
}
