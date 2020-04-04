#include "utils.h"
#include "grammar.h"
#include "data_structures/hashMap_str_int.h"

#ifndef LEXERDEF_H
#define LEXERDEF_H

#define BUFFER_SIZE 512
#define MAX_LEXEME_LEN 10

struct twinBuffer {
	char buffer_1[BUFFER_SIZE];
	char buffer_2[BUFFER_SIZE];

	int num_bytes_1;
	int num_bytes_2;

	int read_ptr_1;
	int read_ptr_2;

	int buffer_ptr;
	bool flag_retract;
	bool eof;
};


union lexeme {
	char str[21];
	int num;
	float rnum;
  bool boolean;
};


struct symbol {
	enum terminal token;
	union lexeme lexeme;
	unsigned int line_no;
};


#endif
