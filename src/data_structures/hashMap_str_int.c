#include "hashMap_str_int.h"


struct hashNode *initialiseHashNode(const char *key, int value, struct hashNode *next) {
	struct hashNode *new_node;
	new_node = (struct hashNode *) malloc(sizeof(struct hashNode));
	strcpy(new_node->key, key);
	new_node->value = value;
	new_node->next = next;
	return new_node;
}


struct hashMap *initialiseHashMap(){
	struct hashMap *hash_map = (struct hashMap *) malloc(sizeof(struct hashMap));
	hash_map->keys = strl_allocate();
	hash_map->root_nodes_count = MAX_HASH_NODES;
	hash_map->root_nodes = (struct hashNode **) malloc(sizeof(struct hashNode *) * hash_map->root_nodes_count);

	for (int i = 0; i < hash_map->root_nodes_count; ++i) {
		hash_map->root_nodes[i] = initialiseHashNode("", -1, NULL); 
	}

	return hash_map;
}


int hashFunction(const char *key, struct hashMap *hash_map){
	int sum = 0;
	for (int i = 0; i < strlen(key); ++i) {
		sum += key[i];
	}
	return sum % hash_map->root_nodes_count;
}


/* @returns          the int value if present in the hashmap else -1.*/
int hashMapGet(const char *key, struct hashMap *hash_map){
	int value = -1;
	int index = hashFunction(key, hash_map);
	
	struct hashNode *cur = hash_map->root_nodes[index];
	while (cur != NULL) {
		if (strcmp(cur->key, key) == 0) {
			value = cur->value;
			break; // There should only be one node for each value.
		}
		cur = cur->next;
	}

	return value;
}


void hashMapPut(const char *key, int value, struct hashMap *hash_map){
	int index = hashFunction(key, hash_map);
	struct hashNode *cur = hash_map->root_nodes[index];

	// If the root node is not holding a key then fill it.
	// Also add the new key to the list of keys.
	if (strcmp(cur->key, "") == 0) {
		strcpy(cur->key, key);
		cur->value = value;
		strl_append(hash_map->keys, key);
		return;
	}

	// See if the key's node already exists somewhere in the chain.
	while (cur != NULL) {
		if (strcmp(cur->key, key) == 0) {
			cur->value = value;
			return;
		}
		if (cur->next == NULL) {
			break;
		}
		cur = cur->next;
	}

	// Add a new node for the key at the end of the chain since it doesn't already exist.
	// Also update the hash map's keys.
	cur->next = initialiseHashNode(key, value, NULL);
	strl_append(hash_map->keys, key);
}


void hashMapPrint(struct hashMap *hash_map){
	struct hashNode *cur;

	printf("{");
	for (int i = 0; i < hash_map->root_nodes_count; ++i) {
		cur = hash_map->root_nodes[i];
		while (cur != NULL) {
			if (strcmp(cur->key, "") != 0) {
				printf("\n\t\"%s\": %d", cur->key, cur->value);
			}
			cur = cur->next;
		}
	}
	puts("\n}");
}


void deallocateHashNodes(struct hashNode *hash_node) {
	struct hashNode* cur = hash_node;
	struct hashNode* nxt = hash_node->next;
	while (nxt != NULL) {
		free(cur);
		cur = nxt;
		nxt = cur->next;
	}
	free(cur);
}


void deallocateHashMap(struct hashMap *hash_map) {
	for (int i = 0; i < hash_map->root_nodes_count; ++i) {
		deallocateHashNodes(hash_map->root_nodes[i]);
	}
	strl_deallocate(hash_map->keys);
	free(hash_map->root_nodes);
	free(hash_map);
}