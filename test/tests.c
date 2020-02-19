#include <assert.h>
#include "../src/lexer.h"

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

int main() {
    puts("Running tests... ");
    puts("Tests complete... ");
    return 0;
}
