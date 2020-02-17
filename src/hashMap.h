#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_HASH_NODES 20
#define MAX_TOKEN_SIZE 30

// structure of the hashNode- Node that contains the keyword, its corresponding token, and pointer to another hashNode
struct hashNode{

	char keyword[MAX_HASH_NODES];
	char token[MAX_HASH_NODES];
	struct hashNode *next;

};

// initialise the hash list
void initialiseHashList();
// function to get hash mapping of keyword to its resulting index in the array
int hashFunction(char *keyword);
// function to add a keyword to hash list
void addKeyword(char *keyword,char *token,struct hashNode **hash_list);
// function to find keyword in hash_list
char *findKeywordAndTokens(char *keyword, struct hashNode **hash_list);
// function to get keywords and tokens from a file
void getFromFile(char *keyword_file,char *token_file);
// function to print all the keyword-token pairs 
void print_all_pairs(struct hashNode **hash_list);
