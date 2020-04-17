#include <assert.h>
#include "st.h"

int polynomialRollingHashFunction(char key[ST_KEY_BUFFER_MAX_LEN],
                                  int lower_bound, int upper_bound) {
  /* Algorithm from: https://cp-algorithms.com/string/string-hashing.html */
  long long hash_value = 0;
  int key_len = strlen(key);
  for (int i = 0; i < key_len; ++i)
    hash_value += key[i] * pow(POLYNOMIAL_ROLLING_HASH_FACTOR, i);
  return (int)(hash_value % upper_bound) + lower_bound;
}


struct SymbolTable *newSymbolTable(struct SymbolTable *parent,
                                   const char *scope_tag,
                                   SymbolTableHashFunction hash_fn) {
  /* Create a new symbol table (memory allocated on the heap) and return
   * it's pointer. If hash_fn is NULL, then the default hash function for
   * symbol tables as defined by ST_DEFAULT_HASH_FUNCTION will be used. */
  struct SymbolTable *st = (struct SymbolTable *) malloc(
    sizeof(struct SymbolTable)
  );

  st->parent = parent;
  st->keys = strl_allocate();

  if (scope_tag == NULL) {
    strcpy(st->scope_tag, ST_DEFAULT_SCOPE_TAG);
  } else {
    if (strlen(scope_tag) > 125) {
      fprintf(stderr, "Scope tag: <%s> is too long. The current limit is: %d\n",
        scope_tag, ST_SCOPE_TAG_MAX_LEN);
      exit(E2BIG);
    }
    strcpy(st->scope_tag, scope_tag);
  }

  if (hash_fn == NULL) {
    st->hash = ST_DEFAULT_HASH_FUNCTION;
  } else {
    st->hash = hash_fn;
  }

  for (int i = 0; i < ST_NUMBER_OF_BUCKETS; ++i) {
    memset(&(st->nodes[i]), 0, sizeof(struct SymbolTableNode));
    /* This should also make the key a zero-length string. */
    st->nodes[i].next = NULL;  /* Formally initialize this pointer. */
  }

  st->is_module_scope = false; /* If needed set this to true afterwards. */

  return st;
}


void deleteSymbolTable(struct SymbolTable *st) {
  /* De-allocate memory for a given symbol table. */
  struct SymbolTableNode *curr_node, *next_node;

  strl_deallocate(st->keys);

  for (int i = 0; i < ST_NUMBER_OF_BUCKETS; ++i) {
    curr_node = st->nodes[i].next;
    /* st->nodes[i] will be freed along with st. */
    while (curr_node != NULL) {
      next_node = curr_node->next;
      free(curr_node);
      curr_node = next_node;
    }
  }

  free(st);
}


void checkThrowEmptyKeyError(char key[ST_KEY_BUFFER_MAX_LEN]) {
  /* Print an error message to stderr and exit if the key is empty. */
  if (strlen(key) == 0) {
    fprintf(stderr, "symbol table: empty key not accepted.");
    exit(EXIT_FAILURE);
  }
}


bool symbolTableNodeIsSet(struct SymbolTableNode *st_node) {
  /* Return true if the node has been initialized, else false. */
  if (st_node == NULL || strlen(st_node->key) == 0)
    return false;
  return true;
}


bool symbolTableNodeKeyMatches(struct SymbolTableNode *st_node,
                      char key[ST_KEY_BUFFER_MAX_LEN]) {
  /* Return true if the keys match else false. Basically !strcmp().*/
  if (strcmp(st_node->key, key) == 0)
    return true;
  return false;
}


struct SymbolTableNode *symbolTableGet(struct SymbolTable *st,
                                       char key[ST_KEY_BUFFER_MAX_LEN]) {
  /* Using the key and the symbol table's hash function, map into
    * the symbol table and find the corresponding node. If a node
    * does not exist for the key, then return NULL. */
  struct SymbolTableNode *curr_node;
  int bucket_number = st->hash(key, 0, ST_NUMBER_OF_BUCKETS);
  
  checkThrowEmptyKeyError(key);


  curr_node = &(st->nodes[bucket_number]);
  while (curr_node != NULL) {
    if (!symbolTableNodeIsSet(curr_node))
      break;
    if (symbolTableNodeKeyMatches(curr_node, key))
      return curr_node;
    curr_node = curr_node->next;
  }
  
  return NULL;
}


bool symbolTableSet(struct SymbolTable *st,
                    char key[ST_KEY_BUFFER_MAX_LEN],
                    union SymbolTableValue value,
                    enum SymbolTableValueType value_type,
                    bool overwrite) {
  /* Set the node hashed to by the key (via. the symbol table's hash
   * function) to st_node. We WILL occupy the first node of the bucket
   * if it does not yet have a key. If overwrite is set to true, then
   * if the key exists, the node will be updated. The return value
   * indicates if overwritting happened; true if it did, else false. */
  bool exists = false;
  struct SymbolTableNode *curr_node;
  int bucket_number = st->hash(key, 0, ST_NUMBER_OF_BUCKETS);

  checkThrowEmptyKeyError(key);

  curr_node = &(st->nodes[bucket_number]);
  if (symbolTableNodeIsSet(curr_node)) {

    while (true) {
      if (symbolTableNodeKeyMatches(curr_node, key)) {
        exists = true;
        break;
      }
      
      if (curr_node->next != NULL) {
        curr_node = curr_node->next;
        continue;
      }

      curr_node->next = (struct SymbolTableNode *) malloc(
        sizeof(struct SymbolTableNode)
      );
      curr_node = curr_node->next;
      memset(curr_node, 0, sizeof(struct SymbolTableNode));
      curr_node->next = NULL;  /* Formally initialize this pointer. */
      break;
    }

  }

  if (!exists) {
    strcpy(curr_node->key, key);
    strl_append(st->keys, key);
  }

  if (overwrite || !exists) {
    curr_node->value = value;
    curr_node->value_type = value_type;
    curr_node->next = NULL;
  }

  return (overwrite & exists);
}


char *getDataTypeString(struct VariableEntry variable) {
    int lower_bound, upper_bound;
    char *datatype_str;
    char *complete_datatype_str = malloc(sizeof(char) * 128);
    /* Free this string on the user end. */

    if (variable.datatype == INTEGER)
      datatype_str = "INT";
    else if (variable.datatype == REAL)
      datatype_str = "REAL";
    else if (variable.datatype == BOOLEAN_)
      datatype_str = "BOOL";
    else
      datatype_str = "???";
    
    if (variable.isArray) {
      lower_bound = variable.lower_bound->value.num;
      upper_bound = variable.upper_bound->value.num;
      sprintf(complete_datatype_str, "%s[%d..%d]", datatype_str,
              lower_bound, upper_bound);
    }
    else
      sprintf(complete_datatype_str, "%s", datatype_str);
    return complete_datatype_str;
}


void printKey(struct SymbolTable *st, char *key, bool last_key) {
  int line_num, def_line_num, dec_line_num;
  char *comma, *datatype_str;
  struct SymbolTableNode *data;
  
  data = symbolTableGet(st, key);
  assert(data != NULL);

  if (last_key)
    comma = "";
  else
    comma = ",";

  if (data->value_type == ST_MODULE) {
    dec_line_num = data->value.module.dec_line_number;
    def_line_num = data->value.module.def_line_number;
    printf("    %s: (Module, dec line: %d, def line: %d)%s\n", key,
           dec_line_num, def_line_num, comma);
  }
  
  else if (data->value_type == ST_VARIABLE) {
    line_num = data->value.variable.line_number;
    datatype_str = getDataTypeString(data->value.variable);
    printf("    %s: (Variable, line: %d, type: %s)%s\n", key, line_num,
           datatype_str, comma);
    free(datatype_str);
  }

  else {
    fprintf(stderr, "printKey: Invalid value_type for key %s.\n", key);
    exit(EXIT_FAILURE);
  }
}


void printSymbolTable(struct SymbolTable *st) {
  char *key;
  int num_keys = st->keys->filled;

  if (num_keys == 0) {
    printf("{}\n");
    return;
  }

  printf("{\n");
  for(int i = 0; i < num_keys - 1; ++i) {
    key = strl_get(st->keys, i);
    printKey(st, key, false);
  }
  key = strl_get(st->keys, num_keys - 1);
  printKey(st, key, true);
  printf("}\n");
}