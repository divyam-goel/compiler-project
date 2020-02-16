#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define CHAR_BUFFER_SIZE 100

// structure to store the two buffers
struct twin_buffer{
	char buffer1[CHAR_BUFFER_SIZE];
	int bytes_written_1;
	char buffer2[CHAR_BUFFER_SIZE];
	int bytes_written_2;
	int empty;

}; 

FILE *getStream(FILE *fp);
