#ifndef ST_H

#define ST_H

#include "string.h"
#include "stDef.h"
#include "math.h"

#define POLYNOMIAL_ROLLING_HASH_FACTOR 53
#define ST_DEFAULT_HASH_FUNCTION polynomialRollingHashFunction;


int polynomialRollingHashFunction(char key[ST_KEY_BUFFER_MAX_LEN],
                                  int lower_bound, int upper_bound);

struct SymbolTable *newSymbolTable(struct SymbolTable *parent,
                                   SymbolTableHashFunction hash_fn);


void deleteSymbolTable(struct SymbolTable *st);

#endif