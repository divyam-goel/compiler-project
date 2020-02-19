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
    ssize_t rd = 0;
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

struct twinBuffer buffer;
int line_no = 1;

struct hashNode **hash_map;

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

/* START -  struct symbol helper coder */

void populateSymbol(struct symbol * symbol, int token, char *str) {
	symbol->token = token;
	symbol->line_no = line_no;
	if(token == IDENTIFIER)
		strcpy(symbol->lexeme.str, str);
	else if (token == NUM)
		symbol->lexeme.num = atoi(str);
	else if (token == RNUM)
		symbol->lexeme.rnum = atof(str);
}

/* END -  struct symbol helper coder */

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
		    else {
		    	buffer.flag_retract = false;
			}
			break;

		case 2:
			buffer.buffer_ptr = 1;
		    buffer.read_ptr_1 = -1;
		    if (!buffer.flag_retract)
		    	buffer.num_bytes_1 = fread(buffer.buffer_1, sizeof(char), CHAR_BUFFER_SIZE, fp);
		    else {
		    	buffer.flag_retract = false;
			}
			break;
	}
}

struct symbol getNextToken(FILE * fp) {
	struct symbol symbol;
	
	int state = 1; // starting state
	char str[40];
	int num = 0;
	char ch;

	while(true) {
		ch = getNextChar(fp);		
		// printf("%c %d %d\n", ch, state, line_no);
		
		switch(state) {
			// start state
			case 1:
				// identifiers and keywords
				if (ch == ' ' || ch == '\t')
					break;

				else if (ch == '\n') {
					line_no += 1;
					break;
				}

				else if (isalpha(ch)) {
					state = 2;
					str[num] = ch;
					num += 1;
				}

				// numbers
				else if (isdigit(ch)) {
					state = 3;
					str[num++] = ch;
				}

				// arithmetic
				else if (ch == '+') {
					symbol.token = PLUS;
					populateSymbol(&symbol, PLUS, NULL);
					return symbol;
				}
				else if (ch == '-') 
					state = 9;
				else if (ch == '*') {
					state = 18;
					// populateSymbol(&symbol, MUL, NULL);
					// return symbol;
				}
				else if (ch == '/') {
					symbol.token = DIV;
					populateSymbol(&symbol, DIV, NULL);
					return symbol;
				}

				// relational
				else if (ch == '<')
					state = 10;
				else if (ch == '>')
					state = 12;
				else if (ch == '=')
					state = 14;
				else if (ch == '!')
					state = 15;

				// brackets
				else if (ch == '[') {
					populateSymbol(&symbol, SQBO, NULL);
					return symbol;
				}
				else if (ch == ']') {
					populateSymbol(&symbol, SQBC, NULL);
					return symbol;
				}
				else if (ch == '(') {
					populateSymbol(&symbol, BO, NULL);
					return symbol;
				}
				else if (ch == ')') {
					populateSymbol(&symbol, BC, NULL);
					return symbol;
				}

				// others
				else if (ch == '.') {
					state = 16;
					// printf("DOT DETECTED\n");
				}
				else if (ch == ':')
					state = 17;
				else if (ch == ';') {
					populateSymbol(&symbol, SEMICOL, NULL);
					return symbol;
				}
				else if (ch == ',') {
					populateSymbol(&symbol, COMMA, NULL);
					return symbol;
				}

				break;

			case 2: // recognize identifier & keywords
				// fix the identifier length condition
				if ((isalnum(ch) == 0 && ch != '_') || num == 21) {
					retractRead(1); // retract
					str[num] = '\0';
					enum terminal token = getFromHashMap(str, hash_map);
					populateSymbol(&symbol, token, str);
					return symbol;
				}
				str[num++] = ch;
				break;

			case 3:
				if (isdigit(ch)) {
					str[num++] = ch;
					break;
				}
				if (ch == '.') {
					state = 4; // check decimal
					str[num++] = ch;
					break;
				}
				retractRead(1); // retract
				str[num] = '\0';
				populateSymbol(&symbol, NUM, str);
				return symbol;

			case 4:
				if (isdigit(ch)) {
					state = 5; // decimal should have atleast one digit after '.'
					str[num++] = ch;
					break;
				}
				retractRead(2); // double retraction
				str[--num] = '\0';
				populateSymbol(&symbol, NUM, str);
				return symbol;

			case 5:
				if (isdigit(ch)) {
					str[num++] = ch;
					break;
				}
				if (ch == 'e' || ch == 'E') {
					state = 6;
					str[num++] = ch;
					break;
				}
				retractRead(1); // retract
				str[num] = '\0';
				populateSymbol(&symbol, RNUM, str);
				return symbol;

			case 6:
				if (isdigit(ch)) {
					state = 8;
					str[num++] = ch;
					break;
				}
				if (ch == '+' || ch == '-') {
					state = 7;
					str[num++] = ch;
					break;
				}
				retractRead(2); // double retract
				str[--num] = '\0';
				populateSymbol(&symbol, RNUM, str);
				return symbol;

			case 7:
				if (isdigit(ch)) {
					state = 8;
					str[num++] = ch;
					break;
				}
				retractRead(3); // triple retract
				num -= 2;
				str[num] = '\0';
				populateSymbol(&symbol, RNUM, str);
				return symbol;

			case 8:
				if (isdigit(ch)) {
					str[num++] = ch;
					break;
				}
				retractRead(1); // retract
				str[--num] = '\0';
				populateSymbol(&symbol, RNUM, str);
				return symbol;

			case 9:
				if (isdigit(ch)) {
					str[num++] = ch;
					state = 3;
					break;
				}
				retractRead(1); // retract
				populateSymbol(&symbol, MINUS, NULL);
				return symbol;

			case 10:
				if (ch == '<') {
					state = 11;
					break;
				}
				if (ch == '=')
					populateSymbol(&symbol, LE, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(&symbol, LT, NULL);
				}
				return symbol;

			case 11:
				if (ch == '<')
					populateSymbol(&symbol, DRIVERDEF, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(&symbol, DEF, NULL);
				}
				return symbol;

			case 12:
				if (ch == '>') {
					state = 13;
					break;
				}
				if (ch == '=')
					populateSymbol(&symbol, GE, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(&symbol, GT, NULL);
				}
				return symbol;

			case 13:
				if (ch == '>')
					populateSymbol(&symbol, DRIVERENDDEF, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(&symbol, ENDDEF, NULL);
				}
				return symbol;

			case 14:
				if (ch == '=') {
					populateSymbol(&symbol, EQ, NULL);
					return symbol;
				}
				retractRead(1); // retract
				// ERROR

			case 15:
				if (ch == '=') {
					populateSymbol(&symbol, NE, NULL);
					return symbol;
				}
				retractRead(1); // retract
				// ERROR

			case 16:
				if (ch == '.') {
					populateSymbol(&symbol, RANGEOP, NULL);
					return symbol;
				}
				retractRead(1); // retract
				// ERROR

			case 17:
				if (ch == '=')
					populateSymbol(&symbol, ASSIGNOP, NULL);
				else {
					retractRead(1);
					populateSymbol(&symbol, COLON, NULL);
				}
				return symbol;

			case 18:
				if (ch == '*') {
					state = 19;
					break;
				}
				populateSymbol(&symbol, MUL, NULL);
				return symbol;

			case 19:
				if (ch == '*')
					state = 20;
				else if (ch == '\n')
					line_no += 1;
				break;

			case 20:
				if (ch == '*')
					state = 1;
				else {
					if (ch == '\n')
						line_no += 1;
					state = 19;
				}
				break;

			default:
				printf("%c %d\n", ch, ch);
				symbol.token = -1;
				return symbol;

		}
	}
}
