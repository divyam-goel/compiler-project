#include "utils.h"
#include <ctype.h>
#include "hashMap_str_enum.h"

#ifndef LEXER_H
#define LEXER_H

#define BUFFER_SIZE 512
#define CHAR_BUFFER_SIZE 10

struct twinBuffer {
	char buffer_1[CHAR_BUFFER_SIZE];
	char buffer_2[CHAR_BUFFER_SIZE];

	int num_bytes_1;
	int num_bytes_2;

	int read_ptr_1;
	int read_ptr_2;

	int buffer_ptr;
	bool flag_retract;
};


union lexeme {
	char str[21];
	int num;
	float rnum;
};


struct symbol {
	enum terminal token;
	union lexeme lexeme;
	unsigned int line_no;
};

void removeComments(char *testcaseFile, char *cleanFile);
int getNumBytes();
char getChar(int index);
char getNextChar(FILE *fp);
bool isBufferEnd();
void getStream(FILE * fp);

struct symbol getNextToken();


#endif