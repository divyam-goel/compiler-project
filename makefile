CC = gcc
CFLAGS = -Wall -g -lm
OBJ_CFLAGS = $(CFLAGS) -c

# TODO: Fix and cleanup this makefile.

# test: build-test
# 	build/tests

# memtest: build-test
# 	valgrind build/tests

# driver: driver.o  utils.o lexer.o parser.o hashMap_str_int.o str_list.o stack.o ast.o
# 	$(CC) lib/driver.o lib/utils.o lib/lexer.o lib/parser.o lib/hashMap_str_int.o lib/stack.o lib/str_list.o -o build/driver

build-test: test.o utils.o lexer.o parser.o hashMap_str_int.o str_list.o stack.o ast.o stCore.o st.o \
	semanticCheck.o
	$(CC) lib/tests.o lib/utils.o lib/lexer.o lib/parser.o lib/hashMap_str_int.o lib/stack.o lib/str_list.o lib/ast.o \
	lib/stCore.o lib/st.o lib/semanticCheck.o -o build/tests $(CFLAGS)

test.o: test/tests.c \
		src/utils.h \
		src/lexer.h src/lexerDef.h \
		src/parser.h src/parserDef.h \
		src/data_structures/str_list.h \
		src/data_structures/hashMap_str_int.h \
		src/data_structures/stack.h
	$(CC) test/tests.c -o lib/tests.o $(OBJ_CFLAGS)

driver.o: src/driver.c src/utils.h \
		src/lexer.h src/lexerDef.h \
		src/parser.h src/parserDef.h \
		src/data_structures/str_list.h \
		src/data_structures/hashMap_str_int.h \
		src/data_structures/stack.h
	$(CC) src/driver.c -o lib/driver.o $(OBJ_CFLAGS)

utils.o: src/utils.c src/utils.h
	$(CC) src/utils.c -o lib/utils.o $(OBJ_CFLAGS)

lexer.o: src/lexer.c src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_int.h
	$(CC) src/lexer.c -o lib/lexer.o $(OBJ_CFLAGS)

parser.o: src/parser.c src/parser.h src/parserDef.h src/data_structures/stack.h\
			src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_int.h \
			src/astDef.h
	$(CC) src/parser.c -o lib/parser.o $(OBJ_CFLAGS)

hashMap_str_int.o: src/data_structures/hashMap_str_int.c src/data_structures/hashMap_str_int.h \
					src/utils.h src/data_structures/str_list.h
	$(CC) src/data_structures/hashMap_str_int.c -o lib/hashMap_str_int.o $(OBJ_CFLAGS)

str_list.o: src/data_structures/str_list.c src/data_structures/str_list.h src/utils.h
	$(CC) src/data_structures/str_list.c -o lib/str_list.o $(OBJ_CFLAGS)

stack.o: src/data_structures/stack.c src/data_structures/stack.h src/parserDef.h src/lexer.h src/lexerDef.h src/utils.h \
			src/data_structures/hashMap_str_int.h
	$(CC) src/data_structures/stack.c -o lib/stack.o $(OBJ_CFLAGS)

# This actually depends on more header files:
ast.o: src/ast.c src/ast.h src/astDef.h
	$(CC) src/ast.c -o lib/ast.o $(OBJ_CFLAGS)

# This actually depends on more header files:
stCore.o: src/stCore.c src/st.h src/stDef.h
	$(CC) src/stCore.c -o lib/stCore.o $(OBJ_CFLAGS)

# This actually depends on more header files:
st.o: src/st.c src/st.h src/stDef.h
	$(CC) src/st.c -o lib/st.o $(OBJ_CFLAGS)

# This actually depends on more header files:
semanticCheck.o: src/semanticCheck.c src/semanticCheck.h src/st.h src/stDef.h src/ast.h src/astDef.h
	$(CC) src/semanticCheck.c -o lib/semanticCheck.o $(OBJ_CFLAGS)

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.out' -delete
	rm build/*
