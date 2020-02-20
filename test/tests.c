#include <assert.h>
#include "../src/lexer.h"
#include "../src/parser.h"

void test_removeComments() {
    /*
    *  Test the removeComments function against a manually generated expected file
    *  to ensure that the function indeed works as expected.
    */
    llog("Running test_getOutputFileName... ");
    char *cleanFile = "./test/fixtures/test_result_program_without_comments.erplag";
    char *testcaseFile = "./test/fixtures/program_with_comments.erplag";
    char *expectedFile = "./test/fixtures/expected_program_without_comments.erplag";
    removeComments(testcaseFile, cleanFile);
    assert(file_compare(cleanFile, expectedFile) == 0);
    llog("Success!!!\n");
}

void defineBuffer() {
    extern struct twinBuffer buffer;
    buffer.num_bytes_1 = 0;
    buffer.num_bytes_2 = 0;
    buffer.read_ptr_1 = -1;
    buffer.read_ptr_2 = -1;
    buffer.buffer_ptr = 2;
    buffer.flag_retract = false;
}

void defineHashMap() {
    extern struct hashNode **hash_map;
    hash_map = initialiseHashList();
    add_keywords(hash_map);
}

void test_getStream() {
    FILE * fp = fopen("test/fixtures/test_case_1.txt", "r");

    if(fp == NULL) {
        printf("Error in opening the file!\n");
        return;
    }

    defineBuffer();
    
    getStream(fp);
    while(!feof(fp)) {
        for(int i = 0; i < getNumBytes(); i++)
            printf("%c", getNextChar(fp));
    }
}

void test_getNextToken() {
    FILE * fp = fopen("test/fixtures/test_case_3.txt", "r");

    if(fp == NULL) {
        printf("Error in opening the file!\n");
        return;
    }

    defineBuffer();
    defineHashMap();
    
    struct symbol token;
    while (!feof(fp)) {
        token = getNextToken(fp);
        if (token.token == 0)
            printf("Token value: %d %s %d\n", token.token, token.lexeme.str, token.line_no);
        else if (token.token == 1)
            printf("Token value: %d %d %d\n", token.token, token.lexeme.num, token.line_no);
        else if (token.token == 2)
            printf("Token value: %d %f %d\n", token.token, token.lexeme.rnum, token.line_no);
        else
            printf("Token value: %d %d\n", token.token, token.line_no);
    }
}

void test_structSymbol() {
    struct symbol s1;
    s1.token = 10;

    s1.lexeme.num = 3;
    printf("%d %d\n", s1.token, s1.lexeme.num);

    s1.lexeme.rnum = 3.4;
    printf("%d %f\n", s1.token, s1.lexeme.rnum);

    strcpy(s1.lexeme.str, "Hello, world!");
    printf("%d %s\n", s1.token, s1.lexeme.str);
}

struct rhsNode *newRule(
    enum nonTerminals non_terminal,
    enum terminals terminal,
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

void test_computeFirstAndFollow() {
    extern grammar G;
    extern struct firstAndFollow F;

    printf("Populating grammar\n");
    // E -> TE'
    G[0].non_terminal = E;
    G[0].head = newRule(T, EPS_, NON_TERMINAL);
    G[0].head->next = newRule(E_, EPS_, NON_TERMINAL);
    // E' -> +TE'
    G[1].non_terminal = E_;
    G[1].head = newRule(E, PLUS_, TERMINAL);
    G[1].head->next = newRule(T, EPS_, NON_TERMINAL);
    G[1].head->next->next = newRule(E_, EPS_, NON_TERMINAL);
    // E' -> eps
    G[2].non_terminal = E_;
    G[2].head = newRule(E, EPS_, TERMINAL);
    // T -> FT'
    G[3].non_terminal = T;
    G[3].head = newRule(F_, EPS_, NON_TERMINAL);
    G[3].head->next = newRule(T_, EPS_, NON_TERMINAL);
    // T' -> *FT'
    G[4].non_terminal = T_;
    G[4].head = newRule(E, MUL_, TERMINAL);
    G[4].head->next = newRule(F_, EPS_, NON_TERMINAL);
    G[4].head->next->next = newRule(T_, EPS_, NON_TERMINAL);
    // T' -> eps
    G[5].non_terminal = T_;
    G[5].head = newRule(E, EPS_, TERMINAL);
    // F -> id
    G[6].non_terminal = F_;
    G[6].head = newRule(E, ID_, TERMINAL);
    // F -> (E)
    G[7].non_terminal = F_;
    G[7].head = newRule(E, BO_, TERMINAL);
    G[7].head->next = newRule(E, EPS_, NON_TERMINAL);
    G[7].head->next->next = newRule(E, BC_, NON_TERMINAL);

    printf("Intializing first and follow\n");
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            F.first[i][j] = -1;
            F.follow[i][j] = '0';
        }
    }

    printf("Computing first and follow ...\n");
    computeFirstAndFollow();

    printf("Printing first and follow ...\n");
    printf("\nFirst:\n\n");
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            printf("%d\t", F.first[i][j]);
        }
        printf("\n");
    }

    printf("\nFollow:\n\n");
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            printf("%c\t", F.follow[i][j]);
        }
        printf("\n");
    }
}

int main() {
    puts("Running tests... ");
    puts("Tests complete... ");
    test_computeFirstAndFollow();
    return 0;
}
