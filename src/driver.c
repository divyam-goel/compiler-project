#include "parser.h"
#include<time.h>


int main(int argc, char const *argv[])
{
	int choice = 0;

	do{
		printf("Menu:\n0 --> Exit out of program\n1 --> Remove comments, and print out cleaned code\n");
		printf("2 --> Print out token list from the lexer output\n");
		printf("3 --> Parse the given file and construct the corresponding parse tree. Print out any errors on the console if necessary\n");
		printf("4 --> Print out time taken for lexical analysis and parsing\n");
		printf("Input your choice here:\n");
		fflush(stdout);
		scanf("%d", &choice);
		switch(choice){
			case 0:{
				// exit
				exit(0);
				break;
			}
			case 1:{
				// print the contents of removeComments
				printf("File without the comments:%s\n",argv[1]);
				char *hjh = "";
				strcpy(hjh, argv[1]);
				removeComments(hjh, "/media/nevin/Shared/Studies/3_2(2019)/Compilers/project/github/src/remove_comments_output.txt");
				break;
			}
			case 2:{
				// print out lexical analyser
				break;
			}
			case 3:{
				// print errors in parsing and construct the parse tree 
				break;
			}
			case 4:{
				// lexer and parsing and then print out time taken 
				clock_t start_time,end_time;
				double CPU_time,CPU_time_seconds;
				start_time = clock();
				// code for parsing and lexeical analysis
				end_time = clock();
				CPU_time = (double) (end_time - start_time);
				CPU_time_seconds = CPU_time / CLOCKS_PER_SEC;

				printf("It took %lf seconds.\n", CPU_time_seconds);
				break;
			}
			default:{
				printf("Wrong input,try again:\n\n");
			}
		}
		// char while_cond = 'n'; 
		printf("Continue?(Enter 0 to stop,anything else to continue):\n");
		scanf("%d", &choice);
		system("clear");



	}while(choice !=0 );
	return 0;
}