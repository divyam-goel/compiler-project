#define BUFFER_SIZE 256

char * arr = (char *) malloc(sizeof(char) * BUFFER_SIZE)

char * getNextToken();

int main(int argc, char const *argv[])
{
	FILE * fp = fopen("read.txt", "r");
	
	if(fp == NULL) {
		printf("Error opening the file!\n");
		return 0;
	}
	
	while(!feof(fp)) {
		fread(arr, size(char), BUFFER_SIZE, fp);
		for(int i = 0; i < BUFFER_SIZE; i++)
			printf("%s\n", arr[i]);
	}
	return 0;
}

