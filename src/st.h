#ifndef ST_H

#define ST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "string.h"
#include "stDef.h"

#define POLYNOMIAL_ROLLING_HASH_FACTOR 53
#define ST_DEFAULT_HASH_FUNCTION polynomialRollingHashFunction;


int polynomialRollingHashFunction(char key[ST_KEY_BUFFER_MAX_LEN],
  int lower_bound, int upper_bound);

struct SymbolTable *newSymbolTable(struct SymbolTable *parent,
  SymbolTableHashFunction hash_fn);

void deleteSymbolTable(struct SymbolTable *st);

struct SymbolTableNode *symbolTableGet(struct SymbolTable *st,
  char key[ST_KEY_BUFFER_MAX_LEN]);

bool symbolTableSet(struct SymbolTable *st, char key[ST_KEY_BUFFER_MAX_LEN],
  union SymbolTableValue value, enum SymbolTableValueType value_type,
  bool overwrite);

#endif