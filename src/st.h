#ifndef ST_H

#define ST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "string.h"
#include "stDef.h"
#include "astDef.h"

#define POLYNOMIAL_ROLLING_HASH_FACTOR 7
#define ST_DEFAULT_HASH_FUNCTION polynomialRollingHashFunction;
#define DT_BOOL_SIZE 8
#define DT_INTEGER_SIZE 8
#define DT_REAL_SIZE 8
#define DT_ARRAY_POINTER_SIZE 8
#define ST_TEMP_VAR_FORMAT "@%.5d"
#define ST_SHADOW_VAR_FORMAT "$%s"

void set_semantic_num();

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
void stAddModuleDefinitions(struct OtherModuleNode *module_ll);
union SymbolTableValue stCreateSymbolTableValueForModule(
  char *name, int dec_line_no, int def_line_no,
  struct InputPlistNode *ipl, struct OutputPlistNode *opl);
union SymbolTableValue stCreateSymbolTableValueForVariable(
  struct LeafNode *varnode, struct Attribute *dtnode,
  struct SymbolTable *scope, bool is_input, bool is_io);
void stAddInputPlistToScope (struct InputPlistNode *plist_ll, struct SymbolTable *scope);
void stAddOutputPlistToScope (struct OutputPlistNode *plist_ll, struct SymbolTable *scope);
void stAddDriverModuleDefinition(struct StatementNode *statements_ll);
void stWalkThroughStatements (struct StatementNode *statement_ll, struct SymbolTable *scope);
struct ModuleEntry *resolveModule (char *module_name);
struct VariableEntry *resolveVariable(char *variable_name, struct SymbolTable *scope);
void stHandleInputStatement(struct InputNode *input_stmt, struct SymbolTable *scope);
void stHandlePrintStatement(struct PrintNode *pri_stmt, struct SymbolTable *scope);
void stHandleAssignmentStatement(struct AssignStmtNode *agn_stmt, struct SymbolTable *scope);
void stHandleModuleReuseStatement(struct ModuleReuseStmtNode *mr_stmt, struct SymbolTable *scope);
void stHandleDeclareStatement(struct DeclareStmtNode *dec_stmt, struct SymbolTable *scope);
void stHandleConditionalStatement(struct ConditionalStmtNode *con_stmt, struct SymbolTable *scope);
void stHandleForLoop(struct ForIterativeStmtNode *for_loop, struct SymbolTable *scope);
void stHandleWhileLoop(struct WhileIterativeStmtNode *while_loop, struct SymbolTable *scope);
void stWalkThroughExpression (struct Attribute *expression, struct SymbolTable *scope);
void stUpdateLeafNode(struct LeafNode *lea, struct SymbolTable *scope);
struct VariableEntry *stNewTemporaryVariable(struct SymbolTable *scope, enum terminal datatype);
struct SymbolTable *getModuleLevelScope(struct SymbolTable *scope);
struct ModuleEntry *getModuleEntry(char *module_name);
int getDatatypeSize(enum terminal datatype);
void printSymbolTablesForDriver(int print_arr);
void printSymbolTableOrArraysForDriver(struct SymbolTable *st, int print_arr);
int getWidth(struct VariableEntry variable);
#endif