#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "lexer.h"

#define MAX_HASH_NODES 19
#define MAX_TOKEN_SIZE 30

// structure of the hashNode- Node that contains the key, its corresponding value, and pointer to another hashNode
// a string-enum mapping- going to be used for mapping keyword string to its enum constant
struct hashNode{

	char key[MAX_TOKEN_SIZE];
	enum terminal keyword_token; 
	struct hashNode *next;

};

// initialise the hash list
struct hashNode **initialiseHashList();
// function to get hash mapping of key to its resulting index in the array
int hashFunction(char *key);
// function to add a key to hash list
void putIntoHashMap(char *key,enum terminal keyword_token, struct hashNode **hash_list);
// function to get back the full structure in hash_list
enum terminal getFromHashMap(char *key, struct hashNode **hash_list);
// function to get keywords and terminal from a file
void getFromFile(char *keyword_file,char *token_file);
// function to print all the key-value pairs present in hash map
void print_all_pairs(struct hashNode **hash_list);
// function to populate the hash list with keyword- token pairings
void add_keywords(struct hashNode **hash_list);