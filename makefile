test: build-test
	build/test

utils: src/utils.h src/utils.c
	gcc src/utils.c -c -g -o lib/utils.o

hashmap: src/hashMap.h src/hashMap.c
	gcc src/hashMap.c -c -g -o lib/hashMap.o

lexer: utils src/lexer.h src/lexer.c
	gcc src/lexer.c -L lib/utils.o -c -g -o lib/lexer.o

strl: utils src/str_list.h src/str_list.c
	gcc src/str_list.c -L lib/utils.o -c -g -o lib/str_list.o

build-test: utils lexer test/test_lexer.c
	gcc lib/lexer.o lib/utils.o test/test_lexer.c -o build/test

build-test: lexer test/test_lexer.c
	gcc lib/lexer.o lib/utils.o lib/str_list.o test/test_lexer.c -g -o build/test

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.out' -delete
	find . -type f -name 'test_result_*' -delete

