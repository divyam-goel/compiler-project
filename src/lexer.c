#include "../lib/lexer.h"

// initialising buffer - this has to be done in the driver function- for now, done in this file for simplicity
struct twin_buffer buff = {"",0,"",0,1};

// getStream function takes in file pointer fp and :
// 1) fills in the buffer twin_buffer
// 2) returns the fp to user
FILE *getStream(FILE *fp){


	// transfer everytjing from buffer 2 to buffer 1
	for(int i=0;i<CHAR_BUFFER_SIZE;i++){
		buff.buffer1[i]=buff.buffer2[i];
	}	
	memset(buff.buffer2,'$',CHAR_BUFFER_SIZE);
	// filling in buffer 2
	for(int index = 0; index < CHAR_BUFFER_SIZE;index++){
		buff.buffer2[index]= fgetc(fp);
		//  if end of file, get out of loop and return 
		if(feof(fp)){
			buff.bytes_written_2=index+1;
			return fp;			
		}
	}
	buff.bytes_written_2=CHAR_BUFFER_SIZE;

	// now, the respectuve buffers to be filled have been filled in, and we return the file pointer fp
	return fp;

}
