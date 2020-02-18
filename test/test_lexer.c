#include <assert.h>
#include "../src/lexer.h"
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

int main() {
    printf("\n");

    //test_removeComments();
    // test_getStream();
    test_getNextToken();

    printf("\n");
    return 0;
}
