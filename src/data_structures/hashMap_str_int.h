#include "../utils.h"
#include "str_list.h"

#define MAX_HASH_NODES 19
#define MAX_TOKEN_SIZE 30

#ifndef HASHMAP_INT_H
#define HASHMAP_INT_H

struct hashNode {
	char key[MAX_TOKEN_SIZE];
	int value;
	struct hashNode *next;
};

struct hashMap {
	struct str_list *keys;
	int root_nodes_count;
	struct hashNode **root_nodes;
};

int hashFunction(const char *key, struct hashMap *hash_map);
struct hashNode *initialiseHashNode(const char *key, int value, struct hashNode *next);
struct hashMap *initialiseHashMap();
int hashMapGet(const char *key, struct hashMap *hash_map);
void hashMapPut(const char *key, int value, struct hashMap *hash_map);
void hashMapPrint(struct hashMap *hash_map);
void deallocateHashNodes(struct hashNode *hash_node);
void deallocateHashMap(struct hashMap *hash_map);

#endif
