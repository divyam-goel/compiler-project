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
// struct twinBuffer buffer = {"", "", 0, -1};
struct twinBuffer buffer;

/* START - buffer helper code */

int getNumBytes() {
	if (buffer.buffer_ptr == 1)
		return buffer.num_bytes_1;
	return buffer.num_bytes_2;
}

char getChar(int index) {
	if (buffer.buffer_ptr == 1)
		return buffer.buffer_1[index];
	return buffer.buffer_2[index];
}

char getNextChar(FILE *fp) {
	if (isBufferEnd())
		getStream(fp);

	if (buffer.buffer_ptr == 1) {
		buffer.read_ptr_1 += 1;
		return buffer.buffer_1[buffer.read_ptr_1];
	}
	
	else {
		buffer.read_ptr_2 += 1;
		return buffer.buffer_2[buffer.read_ptr_2];
	}
}

void retractRead(int val) {
	if (buffer.buffer_ptr == 1 && (buffer.read_ptr_1 - val) < -1) {
		buffer.buffer_ptr = 2;
		buffer.read_ptr_2 = CHAR_BUFFER_SIZE + buffer.read_ptr_1 - val;
		buffer.flag_retract = true;
	}
	
	else if (buffer.buffer_ptr == 2 && (buffer.read_ptr_2 - val) < -1) {
		buffer.buffer_ptr = 1;
		buffer.read_ptr_1 = CHAR_BUFFER_SIZE + buffer.read_ptr_2 - val;
		buffer.flag_retract = true;
	}
	
	else {
		if (buffer.buffer_ptr == 1)
			buffer.read_ptr_1 -= val;
		else
			buffer.read_ptr_2 -= val;		
	}
}

bool isBufferEnd() {
	if (buffer.buffer_ptr == 1)
		if (buffer.read_ptr_1 == (buffer.num_bytes_1 - 1) || buffer.num_bytes_1 == 0)
			return true;

	if (buffer.buffer_ptr == 2)
		if (buffer.read_ptr_2 == (buffer.num_bytes_2 - 1) || buffer.num_bytes_2 == 0)
			return true;

	return false;
}


/* END - buffer helper code */

void getStream(FILE *fp){
	/*	getStream function takes in file pointer fp and :
		1) fills in the buffer twin_buffer
		2) returns the fp to user
	*/

	switch (buffer.buffer_ptr) {
		case 1:
			buffer.buffer_ptr = 2;
		    buffer.read_ptr_2 = -1;
		    if (!buffer.flag_retract)
		    	buffer.num_bytes_2 = fread(buffer.buffer_2, sizeof(char), CHAR_BUFFER_SIZE, fp);
		    else
		    	buffer.flag_retract = false;
			break;

		case 2:
			buffer.buffer_ptr = 1;
		    buffer.read_ptr_1 = -1;
		    if (!buffer.flag_retract)
		    	buffer.num_bytes_1 = fread(buffer.buffer_1, sizeof(char), CHAR_BUFFER_SIZE, fp);
		    else
		    	buffer.flag_retract = false;
			break;
	}
}

int getNextToken(FILE * fp) {
	char ch = '\0';
	int state = 1; // starting state

	// fix initializatin for start_position
	// int start_ptr = buffer.read_ptr;

	while(true) {
		ch = getNextChar(fp);
		// printf("%c %d\n", ch, ch);
		
		switch(state) {
			// start state
			case 1:
				// whitespace
				if (ch == ' ' || ch == '\n' || ch == '\t') {
					state = 100;
					break;
				}

				// identifiers and keywords
				if (isalpha(ch)) {
					state = 2;
					break;
				}

				// numbers
				if (isdigit(ch)) {
					state = 3;
					break;
				}

				// arithmetic
				if (ch == '+')
					return PLUS;
				if (ch == '-')
					state = 9;
				if (ch == '*')
					return MUL;
				if (ch == '/')
					return DIV;

				// relational
				if (ch == '<')
					state = 10;
				if (ch == '>')
					state = 12;
				if (ch == '=')
					state = 14;
				if (ch == '!')
					state = 15;

				// brackets
				if (ch == '[')
					return SQBO;
				if (ch == ']')
					return SQBC;
				if (ch == '(')
					return BO;
				if (ch == ')')
					return BC;

				// others
				if (ch == '.')
					state = 16;
				if (ch == ':')
					state = 17;
				if (ch == ';')
					return SEMICOL;
				if (ch == ',')
					return COMMA;

				break;

			case 2: // recognize identifier & keywords
				// fix the identifier length condition
				if ((isalnum(ch) == 0 && ch != '_')) { //|| ((buffer.read_ptr - start_ptr) > 20)) {
					retractRead(1); // retract
					return IDENTIFIER;
				}
				break;

			case 3:
				if (isdigit(ch))
					break;
				if (ch == '.') {
					state = 4; // check decimal
					break;
				}
				retractRead(1); // retract
				return NUM;

			case 4:
				if (isdigit(ch)) {
					state = 5; // decimal should have atleast one digit after '.'
					break;
				}
				retractRead(2); // double retraction
				return NUM;

			case 5:
				if (isdigit(ch))
					break;
				if (ch == 'e' || ch == 'E') {
					state = 6;
					break;
				}
				retractRead(1);
				return RNUM;

			case 6:
				if (isdigit(ch)) {
					state = 8;
					break;
				}
				if (ch == '+' || ch == '-') {
					state = 7;
					break;
				}
				retractRead(2); // double retract
				return RNUM;

			case 7:
				if (isdigit(ch)) {
					state = 8;
					break;
				}
				retractRead(3); // triple retract
				return RNUM;

			case 8:
				if (isdigit(ch))
					break;
				retractRead(1); // retract
				return RNUM;

			case 9:
				if (isdigit(ch)) {
					state = 3;
					break;
				}
				retractRead(1); // retract
				return MINUS;

			case 10:
				if (ch == '<')
					state = 11;
				if (ch == '=')
					return LE;
				retractRead(1); // retract
				return LT;

			case 11:
				if (ch == '<')
					return DRIVERDEF;
				retractRead(1); // retract
				return DEF;

			case 12:
				if (ch == '>')
					state = 13;
				if (ch == '=')
					return GE;
				retractRead(1); // retract
				return GT;

			case 13:
				if (ch == '>')
					return DRIVERENDDEF;
				retractRead(1); // retract
				return ENDDEF;

			case 14:
				if (ch == '=')
					return EQ;
				retractRead(1); // retract
				// ERROR

			case 15:
				if (ch == '=')
					return NE;
				retractRead(1); // retract
				// ERROR

			case 16:
				if (ch == '.')
					return RANGEOP;
				retractRead(1); // retract
				// ERROR

			case 17:
				if (ch == '=')
					return ASSIGNOP;
				retractRead(1);
				return COLON;
		}

		if (state == 1)
			return -1;

		if (state == 100)
			state = 1;

	}
}
