#include "hashMap.h"

// global hashMap list: for now
struct hashNode **hash_list;

// the five following functions are for a general string-string mapping for key-value pairs of the hash table

// initialization of hashlist
void initialiseHashList(){

	hash_list= (struct hashNode **)malloc(sizeof(struct hashNode *) * MAX_HASH_NODES);
	for(int i=0;i<MAX_HASH_NODES;i++){

		hash_list[i]= (struct hashNode *)malloc(sizeof(struct hashNode));
		strcpy(hash_list[i]->key,"");
		strcpy(hash_list[i]->value,"");
		hash_list[i]->next = NULL;
	}

}

// function to getFromHashMap hash mapping of key to its resulting index in the array
int hashFunction(char *key){

	// for now,using a simple implementation: the remainder when total ascii value is dvided by MAX_HASH_NODES
	int sum=0;
	for(int i=0;i<strlen(key);i++){
		sum+= key[i];
	}
	return sum % MAX_HASH_NODES;
}

// function to add a key to hash list
void putIntoHashMap(char *key,char *value, struct hashNode **hash_list){

	// find index to place the key value in
	int index= hashFunction(key);
	// create node with key and value
	struct hashNode *new_keyword;
	new_keyword= (struct hashNode *)malloc(sizeof(struct hashNode));
	new_keyword->next=NULL;
	strcpy(new_keyword->key,key);
	strcpy(new_keyword->value,value);
	
	struct hashNode *head= hash_list[index];
	// now,adding this to the end of head
	while(head->next!=NULL){
		// check if the next node contains the key we are trying to add
		if(strcmp((head->next)->key,key)==0){
			// replace the current next with the new,updated one
			struct hashNode *temp = head->next;
			new_keyword->next = temp->next;
			head->next = new_keyword;
			free(temp);
			// now that its been added, we exit
			return;
		}
		head=head->next;
	}
	// at the end, we have the last node
	head->next= new_keyword;
}

// function to find key and corresponding value in hash_list - it returns "$" if not in hash  
char * getFromHashMap(char *key, struct hashNode **hash_list){

	char value[30];
	// hash index of key
	int index= hashFunction(key);
	int found=0;
	// searching through the list[index]
	struct hashNode *head= hash_list[index];

	while(head->next!=NULL){
		// comparing the our key with the ones stored in the hash index
		if(strcmp(head->key,key)==0){
			found =1;
			strcpy(value,head->value);
			break;
		}
		head=head->next;
	}
	// checking for the last one
	if(strcmp(head->key,key)==0){
			found =1;
			strcpy(value,head->value);
	}
	char *r= (char *) malloc(sizeof(char) * strlen(value));
	strcpy(r, value);
	// if not found, return "$" sign that signifies not found in hash list
	if(found==0){
		return "$";
	}
	else{
		return r;
	}
}

void print_all_pairs(struct hashNode **hash_list){

	for(int i=0;i<MAX_HASH_NODES;i++){

		struct hashNode *head= hash_list[i];
		head=head->next;
		if(head==NULL){
			continue;
		}
		while(head->next!=NULL){
			// print the values of key-value
			printf("%s -- %s\n",head->key,head->value);
			head=head->next;
		}
		// print for the last one
		printf("%s -- %s\n",head->key,head->value);

	}
}

// this takes in keywords and tokens and stores in hash list
void getFromFile(char *keyword_file,char *token_file){
	FILE *token_fp= fopen(token_file,"r");
	FILE *keyword_fp= fopen(keyword_file,"r");

	while(!feof(keyword_fp)){
		char key[MAX_TOKEN_SIZE];
		char value[MAX_TOKEN_SIZE];
		fscanf(token_fp,"%s\n",value);
		fscanf(keyword_fp,"%s\n",key);
		// adding to the hash list
		putIntoHashMap(key,value,hash_list);
	}
	fclose(token_fp);
	fclose(keyword_fp);
}

// test code
int main(int argc, char const *argv[])
{
	char *key= "end";
	char *value = "A_symbol";
	initialiseHashList();

	// getting values of keywords and corresponding tokens
	getFromFile("../docs/keywords/keywords.txt","../docs/keywords/tokens.txt");
	printf("value for key (%s) is %s\n", key, getFromHashMap(key,hash_list));

	putIntoHashMap("default","FLUFFF",hash_list);
	// print all pairs of key-value
	print_all_pairs(hash_list);
}
