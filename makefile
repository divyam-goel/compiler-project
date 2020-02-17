utils: src/utils.h src/utils.c
	gcc src/utils.c -c -o lib/utils.o

clean:
	find . -type f -name '*.o' -delete
