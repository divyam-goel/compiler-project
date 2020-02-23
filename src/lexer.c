#include "lexer.h"

int line_no = 1;
struct twinBuffer buffer;
extern struct hashMap *terminalMap;  // in parser.c
extern char terminalStringRepresentations[NUM_TERMINALS][16];  // in parser.c
extern char terminalLiteralRepresentations[NUM_TERMINALS][16];  // in parser.c

void removeComments(char *testcaseFile, char *cleanFile) {
    int inp_fd;
    inp_fd = open(testcaseFile, O_RDONLY);
    if (inp_fd == -1) {
        die("removeComments: Failed to open the testcase file.");
    }

    int out_fd;
	if (cleanFile == NULL) {
		out_fd = STDOUT_FILENO;
	} else {
    	out_fd = open(cleanFile, O_WRONLY | O_CREAT);
    	if (out_fd == -1) {
    	    die("removeComments: Failed to open the output file.");
   		}
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


/* START - buffer helper code */
void defineBuffer() {
    buffer.num_bytes_1 = 0;
    buffer.num_bytes_2 = 0;
    buffer.read_ptr_1 = -1;
    buffer.read_ptr_2 = -1;
    buffer.buffer_ptr = 2;
    buffer.flag_retract = false;
    buffer.eof = false;
}


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
	if (isBufferEnd()) {
		if (buffer.eof) {
			return '\0';
		}
		getStream(fp);
	}

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
		buffer.read_ptr_2 = BUFFER_SIZE + buffer.read_ptr_1 - val;
		buffer.flag_retract = true;
	}

	else if (buffer.buffer_ptr == 2 && (buffer.read_ptr_2 - val) < -1) {
		buffer.buffer_ptr = 1;
		buffer.read_ptr_1 = BUFFER_SIZE + buffer.read_ptr_2 - val;
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
	/*	getStream function takes in file pointer fp fills in the buffer twin_buffer. */

	switch (buffer.buffer_ptr) {
		case 1:
			buffer.buffer_ptr = 2;
		    buffer.read_ptr_2 = -1;
		    if (!buffer.flag_retract)
		    	buffer.num_bytes_2 = fread(buffer.buffer_2, sizeof(char), BUFFER_SIZE, fp);
				if (buffer.num_bytes_2 < BUFFER_SIZE) {
					buffer.eof = true;
				}
		    else {
		    	buffer.flag_retract = false;
			}
			break;

		case 2:
			buffer.buffer_ptr = 1;
		    buffer.read_ptr_1 = -1;
		    if (!buffer.flag_retract)
		    	buffer.num_bytes_1 = fread(buffer.buffer_1, sizeof(char), BUFFER_SIZE, fp);
				if (buffer.num_bytes_1 < BUFFER_SIZE) {
					buffer.eof = true;
				}
		    else {
		    	buffer.flag_retract = false;
			}
			break;
	}
}

int getNextToken(FILE * fp, struct symbol *symbol) {
	int state = 1; // starting state
	char str[40];
	int num = 0;
	char ch;

	while(true) {
		ch = getNextChar(fp);

		if (ch == '\0') {
			buffer.eof = false;
			populateSymbol(symbol, DOLLAR, NULL);
			return 0;
		}

		switch(state) {
			// start state
			case 1:
				// identifiers
				if (isalpha(ch)) {
					state = 2;
					str[num++] = ch;
					break;
				}

				// numbers
				else if (isdigit(ch)) {
					state = 3;
					str[num++] = ch;
					break;
				}

				switch (ch) {
					// identifiers and keywords
					case ' ':
						break;

					case '\t':
						break;

					case '\n':
						line_no += 1;
						break;

					// arithmetic
					case '+':
						populateSymbol(symbol, PLUS, NULL);
						return 1;
						break;

					case '-':
						populateSymbol(symbol, DIV, NULL);
						return 1;
						break;

					case '*':
						state = 18;
						break;

					case '/':
						populateSymbol(symbol, DIV, NULL);
						return 1;

					// relational
					case '<':
						state = 10;
						break;

					case '>':
						state = 12;
						break;

					case '=':
						state = 14;
						break;

					case '!':
						state = 15;
						break;

					// brackets
					case '[':
						populateSymbol(symbol, SQBO, NULL);
						return 1;
						break;

					case ']':
						populateSymbol(symbol, SQBC, NULL);
						return 1;
						break;

					case '(':
						populateSymbol(symbol, BO, NULL);
						return 1;
						break;

					case ')':
						populateSymbol(symbol, BC, NULL);
						return 1;
						break;

					// others
					case '.':
						state = 16;
						break;

					case ':':
						state = 17;
						break;

					case ';':
						populateSymbol(symbol, SEMICOL, NULL);
						return 1;
						break;

					case ',':
						populateSymbol(symbol, COMMA, NULL);
						return 1;
						break;
				}
				break;

			case 2: // recognize identifier & keywords
				// fix the identifier length condition
				if ((isalnum(ch) == 0 && ch != '_') || num == 21) {
					retractRead(1); // retract
					str[num] = '\0';
					enum terminal token = (enum terminal) hashMapGet(str, terminalMap);
					if (token == -1) {
						token = IDENTIFIER;
					}
					populateSymbol(symbol, token, str);
					return 1;
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
				populateSymbol(symbol, NUM, str);
				return 1;

			case 4:
				if (isdigit(ch)) {
					state = 5; // decimal should have atleast one digit after '.'
					str[num++] = ch;
					break;
				}
				retractRead(2); // double retraction
				str[--num] = '\0';
				populateSymbol(symbol, NUM, str);
				return 1;

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
				populateSymbol(symbol, RNUM, str);
				return 1;

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
				populateSymbol(symbol, RNUM, str);
				return 1;

			case 7:
				if (isdigit(ch)) {
					state = 8;
					str[num++] = ch;
					break;
				}
				retractRead(3); // triple retract
				num -= 2;
				str[num] = '\0';
				populateSymbol(symbol, RNUM, str);
				return 1;

			case 8:
				if (isdigit(ch)) {
					str[num++] = ch;
					break;
				}
				retractRead(1); // retract
				str[--num] = '\0';
				populateSymbol(symbol, RNUM, str);
				return 1;

			// case 9:
			// 	if (isdigit(ch)) {
			// 		str[num++] = ch;
			// 		state = 3;
			// 		break;
			// 	}
			// 	retractRead(1); // retract
			// 	populateSymbol(symbol, MINUS, NULL);
			// 	return 1;

			case 10:
				if (ch == '<') {
					state = 11;
					break;
				}
				if (ch == '=')
					populateSymbol(symbol, LE, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(symbol, LT, NULL);
				}
				return 1;

			case 11:
				if (ch == '<')
					populateSymbol(symbol, DRIVERDEF, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(symbol, DEF, NULL);
				}
				return 1;

			case 12:
				if (ch == '>') {
					state = 13;
					break;
				}
				if (ch == '=')
					populateSymbol(symbol, GE, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(symbol, GT, NULL);
				}
				return 1;

			case 13:
				if (ch == '>')
					populateSymbol(symbol, DRIVERENDDEF, NULL);
				else {
					retractRead(1); // retract
					populateSymbol(symbol, ENDDEF, NULL);
				}
				return 1;

			case 14:
				if (ch == '=') {
					populateSymbol(symbol, EQ, NULL);
					return 1;
				}
				retractRead(1); // retract
				// ERROR
				state = 100;
				break;

			case 15:
				if (ch == '=') {
					populateSymbol(symbol, NE, NULL);
					return 1;
				}
				retractRead(1); // retract
				// ERROR
				state = 100;
				break;

			case 16:
				if (ch == '.') {
					populateSymbol(symbol, RANGEOP, NULL);
					return 1;
				}
				retractRead(1); // retract
				// ERROR
				state = 100;
				break;

			case 17:
				if (ch == '=')
					populateSymbol(symbol, ASSIGNOP, NULL);
				else {
					retractRead(1);
					populateSymbol(symbol, COLON, NULL);
				}
				return 1;

			case 18:
				if (ch == '*') {
					state = 19;
					break;
				}
				retractRead(1);
				populateSymbol(symbol, MUL, NULL);
				return 1;

			case 19: // Comment enter
				if (ch == '*')
					state = 20;
				else if (ch == '\n')
					line_no += 1;
				break;

			case 20: // Comment exit
				if (ch == '*') {
					state = 1;
				}
				else {
					if (ch == '\n')
						line_no += 1;
					state = 19;
				}
				break;

			default:
            	printf("Error condt: %15s %30c %10d\n",  "ERROR", ch, line_no);
				symbol->token = -1;
				return 1;
		}
	}
}
