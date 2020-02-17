#include "lexer.h"

void removeComments(char *testcaseFile, char *cleanFile) {
    int inp_fd;
    inp_fd = open(testcaseFile, O_RDONLY);
    if (inp_fd == -1) {
        die("removeComments: Failed to open the testcase file.");
    }

    int out_fd;
    out_fd = open(cleanFile, O_WRONLY | O_CREAT);
    if (out_fd == -1) {
        die("removeComments: Failed to open the output file.");
    }

    unsigned short int state = 1;
    ssize_t rd = 0, wr = 0;
    int rd_idx = 0, wr_idx = 0;
    char rd_buf[BUFFER_SIZE], wr_buf[BUFFER_SIZE];
    memset(rd_buf, 0, BUFFER_SIZE);
    memset(wr_buf, 0, BUFFER_SIZE);

    // Outer loop for filling the data buffers.
    while ((rd = read(inp_fd, rd_buf, BUFFER_SIZE)) > 0) {
        // Inner loops for moving through the read buffer. We have 4 states
        // that we maintain across buffer flushes so we can continue without
        // worrying about the fact that at buffer replacement was just done.
        // State 1: Not in a comment segment.
        // State 2: Received a * now deciding if this is a comment or not.
        // State 4: In a comment segment.
        // State 8: Received a * now deciding if we are exiting a comment or not.
        for (rd_idx = 0; rd_idx < rd; ++rd_idx) {

            char ch = rd_buf[rd_idx];

            switch (state) {

                case 1:
                    if (ch == '*') {
                        state = state << 1;
                        break;
                    }
                    wr_buf[wr_idx] = ch;
                    wr_idx += 1;
                    break;

                case 2:
                    if (ch == '*') {
                        state = state << 1;
                        break;
                    }
                    wr_buf[wr_idx] = '*';
                    wr_idx += 1;
                    wr_buf[wr_idx] = ch;
                    wr_idx += 1;
                    state = state >> 1;
                    break;

                case 4:
                    if (ch == '\n') {
                        wr_buf[wr_idx] = '\n';
                        wr_idx += 1;
                    } else if (ch == '*') {
                        state = state << 1;
                    }
                    break;

                case 8:
                    if (ch == '\n') {
                        wr_buf[wr_idx] = '\n';
                        wr_idx += 1;
                        state = state >> 1;
                    } else if (ch == '*') {
                        state = state >> 3;
                    }
                    break;

                default:
                    die("removeComments: Code has reached an invalid state.");

            }

        }

        // Flush out both buffers
        memset(rd_buf, 0, BUFFER_SIZE);
        rd_idx = 0;
        if (write(out_fd, wr_buf, wr_idx) != wr_idx) {
            die("Error in writing to the output file.");
        }
        memset(wr_buf, 0, BUFFER_SIZE);
        wr_idx = 0;
    }

    if (rd < 0) {
        die("removeComments: Error in reading from testcase file.");
    }

    return;
}

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

// test code 
// int main(){

// 	FILE *fp= fopen("utils.c","r");
// 	printf("File opened:\n");
// 	while(!feof(fp)){
// 		fp=getStream(fp);
// 		// printf("KK");
// 		for(int i=0;i<buff.bytes_written_2;i++){
// 			printf("%c",buff.buffer2[i]);
// 		}

// 	}

// 	return 0;
// }