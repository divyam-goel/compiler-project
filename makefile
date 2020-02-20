CC = gcc
CFLAGS = -Wall -g
OBJ_CFLAGS = $(CFLAGS) -c

test: build-test
	build/tests

memtest: build-test
	valgrind build/tests

build-test: test.o utils.o lexer.o parser.o hashMap_str_int.o str_list.o
	$(CC) lib/tests.o lib/utils.o lib/lexer.o lib/parser.o lib/hashMap_str_int.o lib/str_list.o -o build/tests

test.o: test/tests.c \
		src/utils.h \
		src/lexer.h src/lexerDef.h \
		src/parser.h src/parserDef.h \
		src/data_structures/str_list.h \
		src/data_structures/hashMap_str_int.h
	$(CC) test/tests.c -o lib/tests.o $(OBJ_CFLAGS)

utils.o: src/utils.c src/utils.h
	$(CC) src/utils.c -o lib/utils.o $(OBJ_CFLAGS)

lexer.o: src/lexer.c src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_int.h
	$(CC) src/lexer.c -o lib/lexer.o $(OBJ_CFLAGS)

parser.o: src/parser.c src/parser.h src/parserDef.h \
			src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_int.h
	$(CC) src/parser.c -o lib/parser.o $(OBJ_CFLAGS)

hashMap_str_int.o: src/data_structures/hashMap_str_int.c src/data_structures/hashMap_str_int.h \
					src/utils.h src/data_structures/str_list.h
	$(CC) src/data_structures/hashMap_str_int.c -o lib/hashMap_str_int.o $(OBJ_CFLAGS)

str_list.o: src/data_structures/str_list.c src/data_structures/str_list.h src/utils.h
	$(CC) src/data_structures/str_list.c -o lib/str_list.o $(OBJ_CFLAGS)

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.out' -delete
	rm build/*
