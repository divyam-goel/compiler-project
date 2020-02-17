#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_HASH_NODES 19
#define MAX_TOKEN_SIZE 30

// structure of the hashNode- Node that contains the key, its corresponding value, and pointer to another hashNode
// a string-string mapping
struct hashNode{

	char key[MAX_TOKEN_SIZE];
	char value[MAX_TOKEN_SIZE];
	struct hashNode *next;

};

// initialise the hash list
void initialiseHashList();
// function to get hash mapping of key to its resulting index in the array
int hashFunction(char *key);
// function to add a key to hash list
void putIntoHashMap(char *key,char *value,struct hashNode **hash_list);
// function to find key in hash_list
char *getFromHashMap(char *key, struct hashNode **hash_list);
// function to get keywords and tokens from a file
void getFromFile(char *keyword_file,char *token_file);
// function to print all the key-value pairs present in hash map
void print_all_pairs(struct hashNode **hash_list);
