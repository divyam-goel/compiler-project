#ifndef ST_H

#define ST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "string.h"
#include "stDef.h"
#include "astDef.h"

#define POLYNOMIAL_ROLLING_HASH_FACTOR 17
#define ST_DEFAULT_HASH_FUNCTION polynomialRollingHashFunction;

/* Functions from stCore.c */
int polynomialRollingHashFunction(char key[ST_KEY_BUFFER_MAX_LEN],
  int lower_bound, int upper_bound);
struct SymbolTable *newSymbolTable(struct SymbolTable *parent,
  const char *scope_tag,
  SymbolTableHashFunction hash_fn);
void deleteSymbolTable(struct SymbolTable *st);
struct SymbolTableNode *symbolTableGet(struct SymbolTable *st,
  char key[ST_KEY_BUFFER_MAX_LEN]);
bool symbolTableSet(struct SymbolTable *st, char key[ST_KEY_BUFFER_MAX_LEN],
  union SymbolTableValue value, enum SymbolTableValueType value_type,
  bool overwrite);
void printSymbolTable(struct SymbolTable *st);

/* Functions from st.c */

void generateSymbolTables();
void stAddModuleDeclerations(struct ModuleDeclarationNode *declaration_ll);
void stAddModuleDefinitions(struct OtherModuleNode *module_ll,
  bool requires_prior_declaration);
union SymbolTableValue stCreateSymbolTableValueForModule(
  char *name, int dec_line_no, int def_line_no,
  struct InputPlistNode *ipl, struct OutputPlistNode *opl);
union SymbolTableValue stCreateSymbolTableValueForVariable(
  struct LeafNode *varnode, struct Attribute *dtnode,
  struct SymbolTable *scope);
void stAddInputPlistToScope (struct InputPlistNode *plist_ll,
  struct SymbolTable *scope);
void stAddOutputPlistToScope (struct OutputPlistNode *plist_ll,
  struct SymbolTable *scope);
void stAddDriverModuleDefinition(struct StatementNode *statements_ll);

#endif