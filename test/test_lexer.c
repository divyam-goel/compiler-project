#include <assert.h>
#include "../src/lexer.h"
#include "../src/str_list.h"
#include "../src/utils.h"
// #include "../src/hashMap_str_enum.h"

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

void test_strl() {
    llog("Running test_strl... ");
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
    printf("\n");

    //test_removeComments();
    // test_getStream();
    test_getNextToken();
    // test_structSymbol();
    // test_removeComments();
    // test_strl();
    return 0;
}
