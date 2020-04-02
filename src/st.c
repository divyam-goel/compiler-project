#include "st.h"

int polynomialRollingHashFunction(char key[ST_KEY_BUFFER_MAX_LEN],
                                  int lower_bound, int upper_bound) {
  /* Algorithm from: https://cp-algorithms.com/string/string-hashing.html */
  int hash_value = 0, key_len = strlen(key);
  for (int i = 0; i < key_len; ++i)
    hash_value += key[i] * pow(POLYNOMIAL_ROLLING_HASH_FACTOR, i);
  return (hash_value % upper_bound) + lower_bound;
}


struct SymbolTable *newSymbolTable(struct SymbolTable *parent,
                                   SymbolTableHashFunction hash_fn) {
  /* Create a new symbol table (memory allocated on the heap) and return
   * it's pointer. If hash_fn is NULL, then the default hash function for
   * symbol tables as defined by ST_DEFAULT_HASH_FUNCTION will be used. */
  struct SymbolTable *st = (struct SymbolTable *) malloc(
    sizeof(struct SymbolTable)
  );

  st->parent = parent;
  st->keys = strl_allocate();
  
  if (hash_fn == NULL) {
    st->hash = ST_DEFAULT_HASH_FUNCTION;
  } else {
    st->hash = hash_fn;
  }

  for (int i = 0; i < ST_NUMBER_OF_BUCKETS; ++i) {
    memset(&(st->values[i]), 0, sizeof(struct SymbolTableValue));
    st->values[i].next = NULL;  /* Formally initialize this pointer. */
  }

  return st;
}


void deleteSymbolTable(struct SymbolTable *st) {
  /* De-allocate memory for a given symbol table. */
  struct SymbolTableValue *curr_value, *next_value;

  strl_deallocate(st->keys);

  for (int i = 0; i < ST_NUMBER_OF_BUCKETS; ++i) {
    curr_value = st->values[i].next;
    /* st->values[i] will be freed along with st. */
    while (curr_value != NULL) {
      next_value = curr_value->next;
      free(curr_value);
      curr_value = next_value;
    }
  }

  free(st);
}
