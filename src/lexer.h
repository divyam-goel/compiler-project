#include  "utils.h"

#ifndef LEXER_H
#define LEXER_H

#define BUFFER_SIZE 512
#define CHAR_BUFFER_SIZE 100

void removeComments(char *testcaseFile, char *cleanFile);

// structure to store the two buffers
struct twin_buffer{
	char buffer1[CHAR_BUFFER_SIZE];
	int bytes_written_1;
	char buffer2[CHAR_BUFFER_SIZE];
	int bytes_written_2;
	int empty;

}; 

FILE *getStream(FILE *fp);

#endif
