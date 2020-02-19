CC = gcc
CFLAGS = -Wall -g
OBJ_CFLAGS = $(CFLAGS) -c

test: build-test
	build/tests

build-test: test.o utils.o lexer.o parser.o hashMap_str_enum.o
	$(CC) lib/tests.o lib/utils.o lib/lexer.o lib/parser.o lib/hashMap_str_enum.o -o build/tests

test.o: src/parser.c src/parser.h src/parserDef.h \
			src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_enum.h
	$(CC) test/tests.c -o lib/tests.o $(OBJ_CFLAGS)

utils.o: src/utils.c src/utils.h
	$(CC) src/utils.c -o lib/utils.o $(OBJ_CFLAGS)

lexer.o: src/lexer.c src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_enum.h
	$(CC) src/lexer.c -o lib/lexer.o $(OBJ_CFLAGS)

parser.o: src/parser.c src/parser.h src/parserDef.h \
			src/lexer.h src/lexerDef.h src/utils.h src/data_structures/hashMap_str_enum.h
	$(CC) src/parser.c -o lib/parser.o $(OBJ_CFLAGS)

hashMap_str_enum.o: src/data_structures/hashMap_str_enum.c src/data_structures/hashMap_str_enum.h
	$(CC) src/data_structures/hashMap_str_enum.c -o lib/hashMap_str_enum.o $(OBJ_CFLAGS)

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.out' -delete
	find . -type f -name 'test_result_*' -delete
	rm build/*
