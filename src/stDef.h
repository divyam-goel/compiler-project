#ifndef STDEF_H

#define STDEF_H

#include "data_structures/str_list.h"

#define ST_KEY_MAX_LEN 255
#define ST_KEY_BUFFER_MAX_LEN (ST_KEY_MAX_LEN + 1)
#define ST_NUMBER_OF_BUCKETS 1009

struct ModuleEntry {

};


struct FunctionEntry {

};


struct VariableEntry {

};


union SymbolTableEntry {
  struct ModuleEntry module;
  struct FunctionEntry function;
  struct VariableEntry variable;
};

enum SymbolTableEntryType {ST_MODULE, ST_FUNCTION, ST_VARIABLE};

struct SymbolTableValue {
  char key[ST_KEY_BUFFER_MAX_LEN];
  union SymbolTableEntry entry;
  enum SymbolTableEntryType entry_type;
  struct SymbolTableValue *next;  /* Next value in the same bucket. */
};

typedef int (*SymbolTableHashFunction)(char key[ST_KEY_BUFFER_MAX_LEN],
                                       int lower_bound, int upper_bound);

struct SymbolTable {
  /* Each scope shall receive it's own symbol table.
   * It's parent is the symbol table in the immediately
   * preceeding (higher) scope. */
  struct SymbolTable *parent;
  struct str_list *keys;
  struct SymbolTableValue values[ST_NUMBER_OF_BUCKETS];
  SymbolTableHashFunction hash;
};

#endif