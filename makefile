utils: src/utils.h src/utils.c
	gcc src/utils.c -c -o lib/utils.o

lexer: utils src/lexer.h src/lexer.c
	gcc src/lexer.c -L lib/utils.o -c -o lib/lexer.o

build-test: utils lexer test/test_lexer.c
	gcc lib/lexer.o lib/utils.o test/test_lexer.c -o build/test

test: build-test
	build/test

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name 'test_result_*' -delete

