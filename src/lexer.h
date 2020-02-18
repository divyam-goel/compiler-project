#include "utils.h"
#include <ctype.h>

#ifndef LEXER_H
#define LEXER_H

#define BUFFER_SIZE 512
#define CHAR_BUFFER_SIZE 10

void removeComments(char *testcaseFile, char *cleanFile);
int getNumBytes();
char getChar(int index);
char getNextChar(FILE *fp);
bool isBufferEnd();
void getStream(FILE * fp);

int getNextToken();


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


enum TOKENS {
	IDENTIFIER,
	NUM, RNUM, // numbers
	PLUS, MINUS, MUL, DIV, // arithmetic
	LT, LE, GE, GT, EQ, NE, // relational
	DEF, DRIVERDEF, ENDDEF, DRIVERENDDEF, //  module
	SQBO, SQBC, BO, BC, // brackets
	// others
	RANGEOP,
	COLON,
	ASSIGNOP,
	SEMICOL,
	COMMA
};

#endif