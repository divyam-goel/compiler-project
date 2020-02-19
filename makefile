test: build-test
	build/test

utils: src/utils.h src/utils.c
	gcc src/utils.c -c -g -o lib/utils.o

hashmap: src/hashMap.h src/hashMap.c
	gcc src/hashMap.c -c -g -o lib/hashMap.o

lexer: utils hashmap-str-enum src/lexer.h src/lexer.c
	gcc src/lexer.c -L lib/hashMap_str_enum.o -L lib/utils.o -c -g -o lib/lexer.o

strl: utils src/str_list.h src/str_list.c
	gcc src/str_list.c -L lib/utils.o -c -g -o lib/str_list.o

hashmap-str-enum: utils src/hashMap_str_enum.h src/hashMap_str_enum.c
	gcc src/hashMap_str_enum.c -L lib/utils.o -c -g -o lib/hashMap_str_enum.o

parser: utils lexer strl src/parser.h src/parser.c
	gcc src/parser.c -L lib/utils.o -L lib/lexer.o -L lib/str_list.o -c -g -o lib/parser.o

build-test: lexer test/tests.c
	gcc lib/lexer.o lib/utils.o lib/str_list.o lib/hashMap_str_enum.o test/tests.c -g -o build/test

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.out' -delete
	find . -type f -name 'test_result_*' -delete
