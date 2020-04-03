#ifndef STDEF_H

#define STDEF_H

#include "data_structures/str_list.h"

#define ST_KEY_MAX_LEN 255
#define ST_KEY_BUFFER_MAX_LEN (ST_KEY_MAX_LEN + 1)
#define ST_NUMBER_OF_BUCKETS 1009
#define ST_SCOPE_TAG_MAX_LEN 125
#define ST_DEFAULT_SCOPE_TAG "null"

struct ModuleEntry {

};


struct FunctionEntry {

};


struct VariableEntry {

};


union SymbolTableValue {
  struct ModuleEntry module;
  struct FunctionEntry function;
  struct VariableEntry variable;
};

enum SymbolTableValueType {ST_MODULE, ST_FUNCTION, ST_VARIABLE};

struct SymbolTableNode {
  char key[ST_KEY_BUFFER_MAX_LEN];
  union SymbolTableValue value;
  enum SymbolTableValueType value_type;
  struct SymbolTableNode *next;  /* Next value in the same bucket. */
};

typedef int (*SymbolTableHashFunction)(char key[ST_KEY_BUFFER_MAX_LEN],
                                       int lower_bound, int upper_bound);

struct SymbolTable {
  /* Each scope shall receive it's own symbol table.
   * It's parent is the symbol table in the immediately
   * preceeding (higher) scope. */
  struct SymbolTable *parent;
  struct str_list *keys;
  struct SymbolTableNode nodes[ST_NUMBER_OF_BUCKETS];
  SymbolTableHashFunction hash;
  char scope_tag[ST_SCOPE_TAG_MAX_LEN + 1];
  /* The scope tag is just for better visibility of what's going
   * on to make debugging easier. E.g. for functions, it's just
   * the function name.*/
};

#endif