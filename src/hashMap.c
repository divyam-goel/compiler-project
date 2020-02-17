#include "hashMap.h"

// global hashMap list: for now
struct hashNode **hash_list;

// initialization
void initialiseHashList(){

	hash_list= (struct hashNode **)malloc(sizeof(struct hashNode *) * MAX_HASH_NODES);
	for(int i=0;i<MAX_HASH_NODES;i++){

		hash_list[i]= (struct hashNode *)malloc(sizeof(struct hashNode));
		strcpy(hash_list[i]->keyword,"");
		strcpy(hash_list[i]->token,"");
		hash_list[i]->next = NULL;
	}

}
// function to get hash mapping of keyword to its resulting index in the array
int hashFunction(char *keyword){

	// for now,using a simple implementation: the remainder when total ascii value is dvided by MAX_HASH_NODES
	int sum=0;
	for(int i=0;i<strlen(keyword);i++){
		sum+= keyword[i];
	}
	return sum % MAX_HASH_NODES;
}

// function to add a keyword to hash list
void addKeyword(char *keyword,char *token, struct hashNode **hash_list){

	// find index to place the keyword token in
	int index= hashFunction(keyword);
	// create node with keyword and token
	struct hashNode *new_keyword;
	new_keyword= (struct hashNode *)malloc(sizeof(struct hashNode));
	new_keyword->next=NULL;
	strcpy(new_keyword->keyword,keyword);
	strcpy(new_keyword->token,token);
	
	struct hashNode *head= hash_list[index];
	// now,adding this to the end of head
	while(head->next!=NULL){
		head=head->next;
	}
	// at the end, we have the last node, at the end
	head->next= new_keyword;
}

// function to find keyword in hash_list- 
char * findKeywordAndToken(char *keyword, struct hashNode **hash_list){

	char token[30];
	// hash index of keyword
	int index= hashFunction(keyword);
	int found=0;
	// searching through the list[index]
	struct hashNode *head= hash_list[index];

	while(head->next!=NULL){
		// comparing the our keyword with the ones stored in the hash index
		if(strcmp(head->keyword,keyword)==0){
			found =1;
			strcpy(token,head->token);
			break;
		}
		head=head->next;
	}
	// checking for the last one
	if(strcmp(head->keyword,keyword)==0){
			found =1;
			strcpy(token,head->token);
	}
	char *r= (char *) malloc(sizeof(char) * strlen(token));
	strcpy(r, token);
	// if not found, return "$" sign that signifies not found in hash list
	if(found==0){
		return "$";
	}
	else{
		return r;
	}


}

void getFromFile(char *keyword_file,char *token_file){
	FILE *token_fp= fopen(token_file,"r");
	FILE *keyword_fp= fopen(keyword_file,"r");

	while(!feof(keyword_fp)){
		char keyword[MAX_TOKEN_SIZE];
		char token[MAX_TOKEN_SIZE];
		fscanf(token_fp,"%s\n",token);
		fscanf(keyword_fp,"%s\n",keyword);
		// adding to the hash list
		addKeyword(keyword,token,hash_list);
	}
	fclose(token_fp);
	fclose(keyword_fp);
}

void print_all_pairs(struct hashNode **hash_list){

	for(int i=0;i<MAX_HASH_NODES;i++){

		struct hashNode *head= hash_list[i];
		head=head->next;
		if(head==NULL){
			continue;
		}
		while(head->next!=NULL){
			// print the values of keyword-token
			printf("%s -- %s\n",head->keyword,head->token);
			head=head->next;
		}
		// print for the last one
		printf("%s -- %s\n",head->keyword,head->token);

	}
}

// test code
int main(int argc, char const *argv[])
{
	char *keyword= "end";
	char *token = "A_symbol";
	initialiseHashList();

	// getting values of keywords and corresponding tokens
	getFromFile("../docs/keywords/keywords.txt","../docs/keywords/tokens.txt");
	printf("Token for keyword (%s) is %s\n", keyword, findKeywordAndToken(keyword,hash_list));

	// print all pairs of keyword-token
	print_all_pairs(hash_list);
}
