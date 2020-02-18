#include "hashMap_str_array.h"

// global hashMap list: for now
struct hashNode **hash_list;

// the five following functions are for a general string-string mapping for key-value pairs of the hash table

// initialization of hashlist
void initialiseHashList(){

	hash_list= (struct hashNode **)malloc(sizeof(struct hashNode *) * MAX_HASH_NODES);
	for(int i=0;i<MAX_HASH_NODES;i++){

		hash_list[i]= (struct hashNode *)malloc(sizeof(struct hashNode));
		strcpy(hash_list[i]->key,"");
		hash_list[i]->num_elements = 0;

		// strcpy(hash_list[i]->value,"");
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
void putIntoHashMap(char *key,char *value[MAX_TOKEN_SIZE], int num_elements, struct hashNode **hash_list){

	// find index to place the key value in
	int index= hashFunction(key);
	// create node with key and value
	struct hashNode *new_keyword;
	new_keyword= (struct hashNode *)malloc(sizeof(struct hashNode));
	new_keyword->next=NULL;
	strcpy(new_keyword->key,key);
	new_keyword->num_elements = num_elements;
	// copying value array to new_keyword's array
	new_keyword->token_set = (char **)malloc(num_elements * sizeof(char *));
	for(int i=0;i<num_elements;i++){

		new_keyword->token_set[i] = (char *)malloc( MAX_TOKEN_SIZE * sizeof(char));
		strcpy(new_keyword->token_set[i], value[i]);
	}
	// for(int i=0;i<num_elements;i++){
	// 	printf("%s ", new_keyword->token_set[i]);
	// }

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
		// if not, we continue iterating through list
		head=head->next;
	}
	// at the end, we have the last node- key hasn't been found- so we add at the end
	head->next= new_keyword;
}

// function to find key and returns the structure in hash_list - it returns NULL if not in hash  
struct hashNode *getFromHashMap(char *key, struct hashNode **hash_list){

	struct hashNode *value;
	// hash index of key
	int index= hashFunction(key);
	int found=0;
	// searching through the list[index]
	struct hashNode *head= hash_list[index];

	while(head->next!=NULL){
		// comparing the our key with the ones stored in the hash index
		if(strcmp(head->key,key)==0){
			found =1;
			return head;
		}
		head=head->next;
	}
	// checking for the last one
	if(strcmp(head->key,key)==0){
			found =1;
			return head;

	}
	// if not found, return "$" sign that signifies not found in hash list
	if(found==0){
		return NULL;
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
			printf("%s -- {",head->key);
			for(int i=0;i < head->num_elements; i++){
				printf("%s," , head->token_set[i]);
			}
			printf("} \n");
			head=head->next;
		}
		// print for the last one
		printf("%s -- {",head->key);
		for(int i=0;i < head->num_elements; i++){
			printf("%s," , head->token_set[i]);
		}
		printf("} \n");

	}
}

// test code
int main(int argc, char const *argv[]){
	char *key= "end";
	char *value[30] = {"A_symbol","Wheeee"};
	char *key2="end1";
	char *value2[30] = {"A_symefeffffol","Wheeeffdfde"};
	initialiseHashList();

	// // getting values of keywords and corresponding tokens
	// getFromFile("../docs/keywords/keywords.txt","../docs/keywords/tokens.txt");
	// printf("value for key (%s) is %s\n", key, getFromHashMap(key,hash_list));

	putIntoHashMap(key,value,2,hash_list);
	putIntoHashMap(key2,value2,2,hash_list);

	// struct hashNode *temp= getFromHashMap(key2,hash_list);
	// for(int i=0;i<temp->num_elements;i++){
	// 	printf("%s ", temp->token_set[i]);
	// }
	// print all pairs of key-value
	print_all_pairs(hash_list);
	
}

