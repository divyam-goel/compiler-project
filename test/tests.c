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
    extern struct hashMap *terminalMap;
    extern char terminalStringRepresentations[NUM_TERMINALS][16];  // in parser.c
    extern char terminalLiteralRepresentations[NUM_TERMINALS][16];  // in parser.c
    terminalMap = getTerminalMap();

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


struct rhsNode *newRule(
    enum nonTerminal non_terminal,
    enum terminal terminal,
    enum typeOfSymbol flag) {

    struct rhsNode *ptr = (struct rhsNode *) malloc(sizeof(struct rhsNode));
    ptr->flag = flag;
    if (flag == TERMINAL)
        ptr->symbol.terminal = terminal;
    else
        ptr->symbol.non_terminal = non_terminal;
    ptr->next = NULL;
    return ptr;
}

// void populateGrammar() {
//     extern grammar G;

//     // E -> TE'
//     G[0].non_terminal = E;
//     G[0].head = newRule(T, EPS_, NON_TERMINAL);
//     G[0].head->next = newRule(E_, EPS_, NON_TERMINAL);
//     // E' -> +TE'
//     G[1].non_terminal = E_;
//     G[1].head = newRule(E, PLUS_, TERMINAL);
//     G[1].head->next = newRule(T, EPS_, NON_TERMINAL);
//     G[1].head->next->next = newRule(E_, EPS_, NON_TERMINAL);
//     // E' -> eps
//     G[2].non_terminal = E_;
//     G[2].head = newRule(E, EPS_, TERMINAL);
//     // T -> FT'
//     G[3].non_terminal = T;
//     G[3].head = newRule(F_, EPS_, NON_TERMINAL);
//     G[3].head->next = newRule(T_, EPS_, NON_TERMINAL);
//     // T' -> *FT'
//     G[4].non_terminal = T_;
//     G[4].head = newRule(E, MUL_, TERMINAL);
//     G[4].head->next = newRule(F_, EPS_, NON_TERMINAL);
//     G[4].head->next->next = newRule(T_, EPS_, NON_TERMINAL);
//     // T' -> eps
//     G[5].non_terminal = T_;
//     G[5].head = newRule(E, EPS_, TERMINAL);
//     // F -> id
//     G[6].non_terminal = F_;
//     G[6].head = newRule(E, ID_, TERMINAL);
//     // F -> (E)
//     G[7].non_terminal = F_;
//     G[7].head = newRule(E, BO_, TERMINAL);
//     G[7].head->next = newRule(E, EPS_, NON_TERMINAL);
//     G[7].head->next->next = newRule(E, BC_, TERMINAL);
// }

void initializeFirstAndFollow() {
    extern struct firstAndFollow F;

    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            F.first[i][j] = -1;
            F.follow[i][j] = '0';
        }
    }
}

void intializeParseTable() {
    extern table parseTable;

    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            parseTable[i][j] = -1;
        }
    }
}

void test_computeFirstAndFollow() {
    extern struct firstAndFollow F;
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

void test_strl() {
    llog("\t+ Running test_strl... ");
    char* str;
    struct str_list* strl = strl_allocate();

    strl_append(strl, "hello");
    str = strl_get(strl, 0);
    assert(strcmp(str, "hello") == 0);
    assert(strl_len(strl) == 1);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    str = strl_get(strl, 1);
    assert(str == NULL);

    strl_append(strl, "the");
    str = strl_get(strl, 1);
    assert(strcmp(str, "the") == 0);
    assert(strl_len(strl) == 2);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "quick");
    str = strl_get(strl, 2);
    assert(strcmp(str, "quick") == 0);
    assert(strl_len(strl) == 3);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "brown");
    str = strl_get(strl, 3);
    assert(strcmp(str, "brown") == 0);
    assert(strl_len(strl) == 4);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "fox");
    str = strl_get(strl, 4);
    assert(strcmp(str, "fox") == 0);
    assert(strl_len(strl) == 5);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "jumps");
    str = strl_get(strl, 5);
    assert(strcmp(str, "jumps") == 0);
    assert(strl_len(strl) == 6);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "over");
    str = strl_get(strl, 6);
    assert(strcmp(str, "over") == 0);
    assert(strl_len(strl) == 7);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "the");
    str = strl_get(strl, 7);
    assert(strcmp(str, "the") == 0);
    assert(strl_len(strl) == 8);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "lazy");
    str = strl_get(strl, 8);
    assert(strcmp(str, "lazy") == 0);
    assert(strl_len(strl) == 9);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    strl_append(strl, "dog");
    str = strl_get(strl, 9);
    assert(strcmp(str, "dog") == 0);
    assert(strl_len(strl) == 10);
    assert(strl->capacity == INITIAL_STRL_LENGTH);

    // Doubling of capacity should happen now.
    strl_append(strl, "last straw");
    str = strl_get(strl, 10);
    assert(strcmp(str, "last straw") == 0);
    assert(strl_len(strl) == 11);
    assert(strl->capacity == INITIAL_STRL_LENGTH * 2 + 1);

    strl_append(strl, "and then some");
    str = strl_get(strl, 11);
    assert(strcmp(str, "and then some") == 0);
    assert(strl_len(strl) == 12);
    assert(strl->capacity == INITIAL_STRL_LENGTH * 2 + 1);

    strl_deallocate(strl);
    llog("Success!!!\n");
}

void test_hashMap() {
    llog("\t+ Running test_hashMap... ");
    struct hashMap *hash_map = initialiseHashMap();

    int count = 10;
    char *keys[] = {"hello", "olleh", "the", "quick", "brown",
                    "fox", "jumps", "over", "the", "lazy dog"};
    int vals[] = {9392, 9204, 8809, 8384, 8840,
                  9458, 9647, 9668, 9835, 8460};

    for (int i = 0; i < count; ++i) {
        hashMapPut(keys[i], vals[i], hash_map);
    }

    assert(hashMapGet("hello", hash_map) == 9392);
    assert(hashMapGet("olleh", hash_map) == 9204);
    assert(hashMapGet("quick", hash_map) == 8384);
    assert(hashMapGet("brown", hash_map) == 8840);
    assert(hashMapGet("fox", hash_map) == 9458);
    assert(hashMapGet("jumps", hash_map) == 9647);
    assert(hashMapGet("over", hash_map) == 9668);
    assert(hashMapGet("the", hash_map) == 9835);  // Overwrite the previous value.
    assert(hashMapGet("lazy dog", hash_map) == 8460);

    deallocateHashMap(hash_map);

    llog("Success!!!\n");
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
    
    printf("Populating terminal hash map ...\n");
    extern struct hashMap *terminalMap;
    terminalMap = getTerminalMap();

    printf("Parsing input source code ...\n");
    // parseInputSourceCode("test/fixtures/test_case_4.txt");
    parseInputSourceCode("test/fixtures/stage 1/t4.txt");

    printParseTree("test/test_result_parse_tree_output.txt");
}

int main() {
    puts("Running tests... ");
    // test_getNextToken();
    test_parseInputSourceCode();
    printf("\nTests complete!!!\n");
    return 0;
}
