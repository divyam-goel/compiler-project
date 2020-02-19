#include <assert.h>
#include "../src/lexer.h"
#include "../src/str_list.h"
#include "../src/utils.h"

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

void define_buffer() {
    extern struct twinBuffer buffer;
    buffer.num_bytes_1 = 0;
    buffer.num_bytes_2 = 0;
    buffer.read_ptr_1 = -1;
    buffer.read_ptr_2 = -1;
    buffer.buffer_ptr = 2;
    buffer.flag_retract = false;
}

void test_getStream() {
    FILE * fp = fopen("src/read.txt", "r");

    if(fp == NULL) {
        printf("Error in opening the file!\n");
        return;
    }

    define_buffer();
    
    getStream(fp);
    while(!feof(fp)) {
        for(int i = 0; i < getNumBytes(); i++)
            printf("%c", getNextChar(fp));
    }
}

void test_getNextToken() {
    FILE * fp = fopen("src/read.txt", "r");

    if(fp == NULL) {
        printf("Error in opening the file!\n");
        return;
    }

    define_buffer();
    
    int token_value;
    while (!feof(fp)) {
        token_value = getNextToken(fp);
        printf("Token value: %d\n", token_value);
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

int main(int argc, char* argv[], char* envp[]) {
    test_removeComments();
    test_strl();
    return 0;
}
