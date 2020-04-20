# A super simple makefile. Because we want to keep things simple and 
# short, we are going to ignore the slight inefficiency.

.SILENT:

CC = gcc
CFLAGS = -Wall -g -lm
CFILES = ./src/ast.c \
./src/codeGen.c \
./src/codeGenArchive.c \
./src/data_structures/hashMap_str_int.c \
./src/data_structures/stack.c \
./src/data_structures/str_list.c \
./src/intCode.c \
./src/lexer.c \
./src/parser.c \
./src/semanticCheck.c \
./src/st.c \
./src/stCore.c \
./src/utils.c
DRIVER_OUTPUT_FILE = "compiler"
TESTS_OUTPUT_FILE = "tests"

default: driver

test: tests
	./$(TESTS_OUTPUT_FILE)

folders:
	mkdir -p output/

driver: folders
	$(CC) src/driver.c $(CFILES) $(CFLAGS) -o $(DRIVER_OUTPUT_FILE)

tests: folders
	$(CC) test/tests.c $(CFILES) $(CFLAGS) -o $(TESTS_OUTPUT_FILE)

clean:
	rm -rdf output/
	find -wholename "./$(DRIVER_OUTPUT_FILE)" -delete
	find -wholename "./$(TESTS_OUTPUT_FILE)" -delete
	find -wholename "./*.asm" -delete
	find -wholename "./*.out" -delete
