#include <assert.h>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/ast.h"
#include "../src/data_structures/stack.h"
#include "../src/st.h"
#include "../src/semanticCheck.h"

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

    printParseTree("test/test_result_parse_tree_output.txt");
}

void test_createAST(char *grammar_file, char *source_file) {
    test_parseInputSourceCode(grammar_file, source_file);

    printf("Creating AST ...\n");
    createAST();
    
    // printf("Printing AST ...\n");
    // printAST();
}

char *st_value_types[] = {"ST_MODULE", "ST_MODULE", "ST_VARIABLE"};

void printSymbolTableNode(struct SymbolTableNode *node, const char *key) {
    if (node == NULL)
        printf("\tValue for %s does not exist.\n", key);
    else
        printf("\tValue found, type: %s\n", st_value_types[node->value_type]);
}

void symbolTableCoreTests() {
    printf("Running symbol table tests... \n");
    
    char xkey[ST_KEY_BUFFER_MAX_LEN] = "ac";
    char ykey[ST_KEY_BUFFER_MAX_LEN] = "y";
    char scope_tag[] = "<test_scope>";

    struct SymbolTable *st = newSymbolTable(NULL, scope_tag, NULL);
    assert(strcmp(st->scope_tag, scope_tag) == 0);
    printf("Created new scope %s\n", st->scope_tag);
    struct SymbolTableNode *node;

    union SymbolTableValue xval;
    enum SymbolTableValueType xtype = ST_MODULE;
    union SymbolTableValue yval;
    enum SymbolTableValueType ytype;
    assert(strl_len(st->keys) == 0);

    strcpy(xval.module.name, "myModule");
    xval.module.dec_line_number = 1;
    xval.module.def_line_number = 10;
    xval.module.inputplist = NULL;
    xval.module.outputplist = NULL;

    /* Initial set test */
    printf("Initial set test:\n");
    assert(symbolTableSet(st, xkey, xval, xtype, false) == false);
    node = symbolTableGet(st, xkey);
    printSymbolTableNode(node, xkey);
    assert(node->value_type == ST_MODULE);
    node = symbolTableGet(st, ykey);
    printSymbolTableNode(node, ykey);
    assert(node == NULL);
    assert(strl_len(st->keys) == 1);

    /* Test the anti-overwrite guard. */
    printf("Anti-overwrite guard test:n\n");
    xtype = ST_VARIABLE;
    assert(symbolTableSet(st, xkey, xval, xtype, false) == false);
    node = symbolTableGet(st, xkey);
    printSymbolTableNode(node, xkey);
    assert(node->value_type == ST_MODULE);
    node = symbolTableGet(st, ykey);
    printSymbolTableNode(node, ykey);
    assert(node == NULL);
    assert(strl_len(st->keys) == 1);

    /* Test overwritting. */
    printf("Overwrite test:\n");

    struct LeafNode lower_bound, upper_bound;
    lower_bound.type = NUM;
    lower_bound.value.num = 1;
    upper_bound.type = IDENTIFIER;
    strcpy(upper_bound.value.entry, "v");

    xtype = ST_VARIABLE;
    strcpy(xval.variable.name, "myArr");
    xval.module.dec_line_number = 2;
    xval.module.def_line_number = 20;
    xval.variable.value.num = 7;
    xval.variable.datatype = INTEGER;
    xval.variable.isArray = true;
    xval.variable.lower_bound = &lower_bound;
    xval.variable.upper_bound = &upper_bound;
    xval.variable.mem_offset = NULL;
    
    assert(symbolTableSet(st, xkey, xval, xtype, true) == true);
    node = symbolTableGet(st, xkey);
    assert(node->value.variable.isArray == true);
    assert(node->value.variable.isStatic == false);
    printSymbolTableNode(node, xkey);
    assert(node->value_type == ST_VARIABLE);
    node = symbolTableGet(st, ykey);
    printSymbolTableNode(node, ykey);
    assert(node == NULL);
    assert(strl_len(st->keys) == 1);

    /* Now update y and make sure nothing breaks. Also
     * make sure that the symbolTableSet function is returning
     * properly. Specify overwrite=true but because the
     * key does not exist, it should return false. */ 
    printf("Test overwrite when key does not exist: \n");
    ytype = ST_MODULE;
    yval.module.dec_line_number = 5;
    yval.module.def_line_number = 50;
    assert(symbolTableSet(st, ykey, yval, ytype, true) == false);
    node = symbolTableGet(st, ykey);
    printSymbolTableNode(node, ykey);
    assert(node->value_type == ST_MODULE);
    assert(strl_len(st->keys) == 2);

    printSymbolTable(st);

    /* Chaining implementation can be manually tested by setting the
     * ST_NUMBER_OF_BUCKETS to a lower value like 3 and then finding
     * two keys which map to the same value. This could be automated
     * by changing the code to allow overriding the number of buckets,
     * and by storing the number of buckets as a property of the symbol
     * table itself which can then be used in the hash function. */
    deleteSymbolTable(st);
}


void test_createSymbolTables(char *grammar_file, char *source_file) {
    extern bool st_debug_mode;
    st_debug_mode = true;
    test_createAST(grammar_file, source_file);
    printf("Generating symbol tables... \n");
    generateSymbolTables();
}


void test_semanticCheck(char *grammar_file, char *source_file) {
  test_createSymbolTables(grammar_file, source_file);
  printf("Printing AST ...\n");
  printAST();
  extern struct ProgramNode AST;
  printf("Checking AST for Semantic Errors...\n");
  semanticChecker(&AST);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    puts("\nRunning tests...");
    char grammar_file[] = "./docs/grammar/text/grammar.txt";
    char source_file[] = "./test/fixtures/stage 2/basic_tests/test_1.erplag";
    // char source_file[] = "./test/fixtures/stage 2/adv_tests/prhf.erplag";
    // test_removeComments();
    // test_getStream(source_file);
    // test_getNextToken(source_file);
    // test_loadGrammar(grammar_file);
    // test_computeFirstAndFollow(grammar_file);
    // test_parseInputSourceCode(grammar_file, adv_source_file);
    // test_createAST(grammar_file, source_file);
    symbolTableCoreTests();
    // test_createSymbolTables(grammar_file, source_file);
    // test_semanticCheck(grammar_file, source_file);
    printf("\nTests complete!!!\n");
    return 0;
}
