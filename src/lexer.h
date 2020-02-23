#include "lexerDef.h"
#include <ctype.h>

#ifndef LEXER_H
#define LEXER_H


void removeComments(char *testcaseFile, char *cleanFile);
int getNumBytes();
char getChar(int index);
char getNextChar(FILE *fp);
bool isBufferEnd();
void getStream(FILE * fp);
int getNextToken(FILE * fp, struct symbol *symbol);
void defineBuffer();

#endif