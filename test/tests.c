#include <assert.h>
#include "../src/lexer.h"
#include "../src/parser.h"
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

void test_getStream() {
    FILE * fp = fopen("test/fixtures/stage 1/t1.txt", "r");
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

void test_getNextToken() {
    //int numberOfTokens = 0;
    //enum terminal expectedTokens[] = {};
    //union lexeme expectedLexemes[] = {};

    // FILE * fp = fopen("test/fixtures/stage 1/t1.txt", "r");
    FILE * fp = fopen("test/fixtures/all_terminals.txt", "r");

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
    printf("             %15s %30s %10s\n", "TOKEN", "LEXEME", "LINE #");
    while (true) {
        if (getNextToken(fp, &token) == 0) {
            break;
        }
        if (token.token == -1) {
            continue;
        }
        if (token.token == IDENTIFIER)
            printf("Token value: %15s %30s %10d\n", terminalStringRepresentations[token.token], token.lexeme.str, token.line_no);
        else if (token.token == NUM)
            printf("Token value: %15s %30d %10d\n", terminalStringRepresentations[token.token], token.lexeme.num, token.line_no);
        else if (token.token == RNUM)
            printf("Token value: %15s %30f %10d\n", terminalStringRepresentations[token.token], token.lexeme.rnum, token.line_no);
        else
            printf("Token value: %15s %30s %10d\n",  terminalStringRepresentations[token.token], terminalLiteralRepresentations[token.token],token.line_no);
    }

    fclose(fp);
}

void test_computeFirstAndFollow() {
    // extern struct firstAndFollow F;
    extern table parseTable;

    // FILE *fp = fopen("test/test_result_first.txt","w+");

    printf("Populating grammar\n");
    // populateGrammar();
    loadGrammar("./docs/grammar/text/grammar.txt");

    printf("Intializing first and follow\n");
    initializeFirstAndFollow();

    printf("Computing first and follow ...\n");
    computeFirstAndFollowSets();

    printf("Printing first and follow ...\n");

    // fprintf(fp, "Printing first and follow ...\n");
    // fprintf(fp, "\nFirst:\n");

    // for (int j = 0; j < NUM_TERMINALS; ++j) {
    //     fprintf(fp, "%5.3d ", j);
    // }
    // fprintf(fp, "\n");

    // for (int i = 0; i < NUM_NON_TERMINALS; i++) {
    //     fprintf(fp, "%.3d ", i);
    //     for (int j = 0; j < NUM_TERMINALS; j++) {
    //         fprintf(fp, "%5d ", F.first[i][j]);
    //     }
    //     fprintf(fp, "\n");
    // }

    // printf("\nFirst:\n");
    // for (int i = 0; i < NUM_NON_TERMINALS; i++) {
    //     printf("%d ", i);
    //     for (int j = 0; j < NUM_TERMINALS; j++) {
    //         printf("%d ", F.first[i][j]);
    //     }
    //     printf("\n");
    // }

    // printf("\nFollow:\n");
    // for (int i = 0; i < NUM_NON_TERMINALS; i++) {
    //     for (int j = 0; j < NUM_TERMINALS; j++) {
    //         printf("%c ", F.follow[i][j]);
    //     }
    //     printf("\n");
    // }

    printf("\nIntializing parse table ...\n");
    intializeParseTable();

    printf("\nCreating parse table....\n");
    createParseTable();

    // code to print out table
    printf("\nParse table:\n");
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            printf("%d\t", parseTable[i][j]);
        }
        printf("\n");
    }

}

void test_loadGrammar() {
    loadGrammar("./docs/grammar/text/grammar.txt");
    printGrammar();
}

void test_stack() {
    initialiseStack();
    struct rule *rule = (struct rule *) malloc(sizeof(struct rule));
    rule->non_terminal = _PROGRAM;
    rule->head = newRule(_VALUE, PLUS, TERMINAL);
    rule->head->next = newRule(_OP1, EPSILON, NON_TERMINAL);
    rule->head->next->next = newRule(_OP2, EPSILON, NON_TERMINAL);

    // struct stackNode *stack_node = (struct stackNode *) malloc(sizeof(struct stackNode));
    // stack_node->symbol.non_terminal = rule->non_terminal;
    // stack_node->flag = NON_TERMINAL;
    // stack_node->next = NULL;
    // push(stack_node);
    // pushRuleIntoStack(rule);
    printStack();
}

void test_parseInputSourceCode() {
    printf("Loading grammar ...\n");
    loadGrammar("./docs/grammar/text/grammar.txt");

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
    // parseInputSourceCode("test/fixtures/test_case_4.txt");
    parseInputSourceCode("test/fixtures/stage 1/t3.txt");

    printParseTree("test/test_result_parse_tree_output.txt");
}

int main() {
    puts("Running tests... ");
    // test_getNextToken();
    test_parseInputSourceCode();
    printf("\nTests complete!!!\n");
    return 0;
}
