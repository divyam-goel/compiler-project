#include "hashMap_str_enum.h"

// initialization of hashlist
struct hashNode **initialiseHashList(){

	struct hashNode **hash_list;
	hash_list= (struct hashNode **)malloc(sizeof(struct hashNode *) * MAX_HASH_NODES);
	for(int i=0;i<MAX_HASH_NODES;i++){

		hash_list[i]= (struct hashNode *)malloc(sizeof(struct hashNode));
		strcpy(hash_list[i]->key,"");
		hash_list[i]->keyword_token = IDENTIFIER;
		hash_list[i]->next = NULL;
	}

	return hash_list;


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
void putIntoHashMap(char *key,enum terminal token, struct hashNode **hash_list){

	// find index to place the key value in
	int index= hashFunction(key);
	// create node with key and value
	struct hashNode *new_keyword;
	new_keyword= (struct hashNode *)malloc(sizeof(struct hashNode));
	new_keyword->next=NULL;
	strcpy(new_keyword->key,key);
	// copying value array to new_keyword's array
	new_keyword->keyword_token = token;

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
enum terminal getFromHashMap(char *key, struct hashNode **hash_list){

	// hash index of key
	int index= hashFunction(key);
	int found=0;
	// searching through the list[index]
	struct hashNode *head= hash_list[index];

	while(head->next!=NULL){
		// comparing the our key with the ones stored in the hash index
		if(strcmp(head->key,key)==0){
			found =1;
			return head->keyword_token;
		}
		head=head->next;
	}
	// checking for the last one
	if(strcmp(head->key,key)==0){
			found =1;
			return head->keyword_token;
	}
	// if not found, return "$" sign that signifies not found in hash list
	if(found==0){
		return IDENTIFIER;
	}

	return IDENTIFIER;  // Control should NEVER reach this part.
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
			printf("%s -- ",head->key);
			printf("%d\n", head->keyword_token);
			head=head->next;
		}
		// print for the last one
		printf("%s -- ",head->key);
		printf("%d\n", head->keyword_token);

	}
}

void add_keywords(struct hashNode **hash_list){

	// manually add the number of keywords
	int num_keywords=33;
	char *list[100] = {"integer","real","boolean","of","array","start","end","declare","module","driver",
								  "program","record","tagged","union","get_value","print","use","with","parameters",
								  "true","false","takes","input","returns","AND","OR","for","in","switch",
								  "case","break","default","while"};
	enum terminal token_list[100]= {INTEGER,REAL,BOOLEAN_,OF,ARRAY,START,END,DECLARE,MODULE,
							   	DRIVER,PROGRAM,RECORD,TAGGED,UNION,GET_VALUE,PRINT,USE,WITH,PARAMETERS,TRUE_,FALSE_,
							   	TAKES,INPUT,RETURNS,AND,OR,FOR,IN,SWITCH,CASE,BREAK,DEFAULT,WHILE};
	
	for(int i=0;i<num_keywords;i++){
		putIntoHashMap(list[i],token_list[i],hash_list);
	}
}

// test code
// int main(int argc, char const *argv[]){
// 	char *key= "end";
// 	enum terminal value= IDENTIFIER;
// 	char *key2="end1";
// 	enum terminal value2= DEFAULT;
// 	struct hashNode **hash_list=initialiseHashList();

// 	// // getting values of keywords and corresponding terminal
// 	// putIntoHashMap(key,value,hash_list);
// 	// putIntoHashMap(key2,value2,hash_list);
// 	add_keywords(hash_list);
// 	// enum terminal temp= getFromHashMap(key2,hash_list);
// 	// printf("%d\n", temp);
// 	// print all pairs of key-value
// 	print_all_pairs(hash_list);
	
// }

