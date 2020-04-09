#include "st.h"
#include <assert.h>

bool st_debug_mode = false;
int semantic_error_count = 0;
extern struct ProgramNode AST;
struct SymbolTable *global_symbol_table = NULL;

char *module_redecleration_error_message = "Semantic Error: Redeclaration of module \"%s\" on line number %d \
(originally declared on line number %d)\n";
char *module_redefinition_error_message = "Semantic Error: Redefintion of module \"%s\" on line number %d \
(originally defined on line number %d)\n";
char *module_missing_declaration_error_message = "Semantic Error: Module \"%s\" is being defined on line \
number %d but it has not been declared.\n";
char *variable_redecleration_error_message = "Semantic Error: Redeclaration of variable \"%s\" on line number %d \
(originally declared on line number %d)\n";
char *variable_undeclared_error_message = "Semantic Error: Variable \"%s\" used on line number %d has not been \
declared.\n";
char *incorrectly_used_as_array_error_message = "Semantic Error: Variable \"%s\" is declared on line %d, but not \
as an array. On line number %d it is being used as an array.\n";


/**
 * This is the compiler's entrypoint into the rest of the symbol table code.
 * If called after creating the global AST, it will generate symbol tables
 * for each scope and point each leaf node in the AST towards it's appropriate
 * symbol table.
 */
void
generateSymbolTables ()
{
  global_symbol_table = newSymbolTable(NULL, "Global", NULL);
  stAddModuleDeclerations(AST.ptr1);
  stAddModuleDefinitions(AST.ptr2, false);
  stAddModuleDefinitions(AST.ptr4, true);
  if (st_debug_mode)
    {
      fprintf(stdout, "Global symbol table after generation of symbol tables:\n");
      printSymbolTable(global_symbol_table);
      printf("\n");
    }
}


/**
 * Add all module declerations to the global symbol table.
 */
void
stAddModuleDeclerations (struct ModuleDeclarationNode *declaration_ll)
{
  char *module_name;
  int current_line_no, original_line_no;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  while (declaration_ll != NULL)
    {
      module_name = declaration_ll->ptr1->value.entry;
      current_line_no = declaration_ll->ptr1->line_number;
      declaration_ll->ptr1->scope = global_symbol_table;
      existing_node = symbolTableGet(global_symbol_table, module_name);
      if (existing_node != NULL)
        {
          original_line_no = existing_node->value.module.dec_line_number;
          fprintf(stderr, module_redecleration_error_message, module_name,
                  current_line_no, original_line_no);
          semantic_error_count += 1;
        }
      else
        {
          new_value = stCreateSymbolTableValueForModule(module_name, current_line_no, -1, NULL, NULL);
          symbolTableSet(global_symbol_table, module_name, new_value, ST_MODULE, false);
        }
      declaration_ll = declaration_ll->ptr2;
    }
  if (st_debug_mode)
    {
      fprintf(stdout, "Global symbol table after module declarations:\n");
      printSymbolTable(global_symbol_table);
      printf("\n");
    }
}


/**
 * Add symbol table entries for module definitions (OtherModules).
 * @param   module_ll                     The first OtherModuleNode of the OtherModule
 *                                        linked list.
 * @param   requires_prior_decleration    Set this to true if the module must have
 *                                        already been declared before this definition.
 */
void
stAddModuleDefinitions (struct OtherModuleNode *module_ll, bool requires_prior_declaration)
{
  char *module_name;
  int current_line_no, original_line_no;
  struct ModuleNode *module;
  struct SymbolTableNode *existing_node;
  union SymbolTableValue new_value;
  struct SymbolTable *module_scope;
  struct ModuleEntry existing_module;
  while (module_ll != NULL)
    {
      module = module_ll->ptr1;
      module_name = module->ptr1->value.entry;
      current_line_no = module->ptr1->line_number;
      module->ptr1->scope = global_symbol_table;
      existing_node = symbolTableGet(global_symbol_table, module_name);
      if (existing_node)
        {
          existing_module = existing_node->value.module;
          if (requires_prior_declaration && existing_module.def_line_number == -1)
            {
              new_value = stCreateSymbolTableValueForModule(module_name,
                existing_module.dec_line_number, current_line_no,
                module->ptr2, module->ptr3);
              symbolTableSet(global_symbol_table, module_name, new_value, ST_MODULE, true);
            }
          else
            {
              original_line_no = existing_node->value.module.dec_line_number;
              fprintf(stderr, module_redefinition_error_message, module_name,
                      current_line_no, original_line_no);
              semantic_error_count += 1;
            }
        }
       else
        {
          if (requires_prior_declaration)
            {
              fprintf(stderr, module_missing_declaration_error_message, module_name, current_line_no);
              semantic_error_count += 1;
            }
          else
            {
              new_value = stCreateSymbolTableValueForModule(module_name,
                current_line_no, current_line_no, module->ptr2, module->ptr3);
              symbolTableSet(global_symbol_table, module_name, new_value, ST_MODULE, false);
            }
        }
      /* Even if there's a semantic error, parse it. No harm. */
      module_scope = newSymbolTable(global_symbol_table, module_name, NULL);
      stAddInputPlistToScope(module->ptr2, module_scope);
      stAddOutputPlistToScope(module->ptr3, module_scope);
      stWalkThroughStatements(module->ptr4, module_scope);
      if (st_debug_mode)
        {
          fprintf(stdout, "Module %s's symbol table:\n", module_name);
          printSymbolTable(module_scope);
          printf("\n");
        }
      module_ll = module_ll->ptr2;
    }
}


/**
 * This function is sort of like an initializer method for SymbolTableValues
 * for modules.
 * @param   name          The name of the module.
 * @param   dec_line_no   The line number where the module was declared.
 * @param   def_line_no   The line number where the module was defined.
 * @param   ipl           The inputplist of the module. May be NULL declerations.
 * @param   opl           The outputplist of the module. May be NULL declerations.
 * @returns               The populated SymbolTableValue struct for the module.
 */
union SymbolTableValue
stCreateSymbolTableValueForModule (char *name, int dec_line_no, int def_line_no,
                                   struct InputPlistNode *ipl, struct OutputPlistNode *opl)
{
  union SymbolTableValue new_value;
  memset(&new_value, 0, sizeof(union SymbolTableValue));
  assert(strlen(name) <= IDENTIFIER_NAME_MAX_LEN);
  strcpy(new_value.module.name, name);
  new_value.module.dec_line_number = dec_line_no;
  new_value.module.def_line_number = def_line_no;
  new_value.module.inputplist = ipl;
  new_value.module.outputplist = opl;
  return new_value;
}


/**
 * This function is sort of like an initializer method for SymbolTableValues
 * for variables. Unlike the stCreateTableValueForModule method, this is
 * method is not a simple dumb initializer method, it takes the leafnode for
 * the variable and the attribute (node) for the datatype and then extracts the
 * necessary information out of them - so code for stuff like checking if this
 * variable is an array only have to be written once.
 * @param   varnode       The leafnode containing the variable name.
 * @param   dtnode        The attribute containing the variable datatype.
 * @param   scope         If this is not NULL, then this will be assigned to the
 *                        leafnodes that we come across in this method.
 * @returns               The populated SymbolTableValue struct for the variable.
 */
union SymbolTableValue
stCreateSymbolTableValueForVariable (struct LeafNode *varnode, struct Attribute *dtnode,
                                     struct SymbolTable *scope)
{
  char *name;
  bool is_array;
  enum terminal basetype;
  int line_number, lower_bound, upper_bound;
  
  assert(varnode != NULL);
  assert(varnode->type == IDENTIFIER);
  name = varnode->value.entry;
  line_number = varnode->line_number;
  varnode->scope = scope;

  if (dtnode->type == ARRAY_TYPE_NODE)
    {
      assert(dtnode->node.arr_typ->ptr1->nullData == true);
      assert(dtnode->node.arr_typ->ptr2->ptr1->type == NUM);
      assert(dtnode->node.arr_typ->ptr2->ptr2->type == NUM);
      is_array = true;
      basetype = dtnode->node.arr_typ->ptr1->type;
      lower_bound = dtnode->node.arr_typ->ptr2->ptr1->value.num;
      upper_bound = dtnode->node.arr_typ->ptr2->ptr2->value.num;
      if (scope != NULL)
        {
          dtnode->node.arr_typ->ptr1->scope = scope;
          dtnode->node.arr_typ->ptr2->ptr1->scope = scope;
          dtnode->node.arr_typ->ptr2->ptr2->scope = scope;
        }
    }
  else
    {
      assert(dtnode->type == LEAF_NODE);
      assert(dtnode->node.lea->nullData == true);
      is_array = false;
      basetype = dtnode->node.lea->type;
      lower_bound = -1;
      upper_bound = -1;
    }

  union SymbolTableValue new_value;
  memset(&new_value, 0, sizeof(union SymbolTableValue));
  assert(strlen(name) <= IDENTIFIER_NAME_MAX_LEN);
  strcpy(new_value.variable.name, name);
  new_value.variable.isArray = is_array;
  new_value.variable.datatype = basetype;
  new_value.variable.line_number = line_number;
  new_value.variable.lower_bound = lower_bound;
  new_value.variable.upper_bound = upper_bound;
  /* We don't set new_value.variable.value here since that would be done later. */
  new_value.variable.mem_offset = NULL;  /* Not sure of what to do with this yet. */
  return new_value;
}


/**
 * Add the variables defined in the inputplist to the specified
 * symbol table / scope.
 * @param   plist_ll  A pointer to an InputPlistNode.
 * @param   scope     The symbol table / scope that the list belongs to (module scope).   
 */
void
stAddInputPlistToScope (struct InputPlistNode *plist_ll, struct SymbolTable *scope)
{
  char *variable_name;
  struct LeafNode *variable_node;
  struct Attribute *variable_datatype;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  while (plist_ll != NULL)
    {
      variable_node = plist_ll->ptr1;
      variable_datatype = plist_ll->ptr2;
      variable_name = variable_node->value.entry;
      existing_node = symbolTableGet(scope, variable_name);
      if (existing_node)
        {
          fprintf(stderr, variable_redecleration_error_message, variable_name,
                  new_value.variable.line_number, existing_node->value.variable.line_number);
          semantic_error_count += 1;
        }
      else
        {
          new_value = stCreateSymbolTableValueForVariable(variable_node, variable_datatype, scope);
          symbolTableSet(scope, variable_name, new_value, ST_VARIABLE, false);
        }
      plist_ll = plist_ll->ptr3;
    }
}


/**
 * Add the variables defined in the outputplist to the specified
 * symbol table / scope. Basically a replica of stAddInputPlistToScope.
 * @param   plist_ll  A pointer to an InputPlistNode.
 * @param   scope     The symbol table / scope that the list belongs to (module scope).   
 */
void
stAddOutputPlistToScope (struct OutputPlistNode *plist_ll, struct SymbolTable *scope)
{
  char *variable_name;
  struct LeafNode *variable_node;
  struct Attribute *variable_datatype;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  while (plist_ll != NULL)
    {
      variable_node = plist_ll->ptr1;
      variable_datatype = plist_ll->ptr2;
      variable_name = variable_node->value.entry;
      existing_node = symbolTableGet(scope, variable_name);
      if (existing_node)
        {
          fprintf(stderr, variable_redecleration_error_message, variable_name,
                  new_value.variable.line_number, existing_node->value.variable.line_number);
          semantic_error_count += 1;
        }
      else
        {
          new_value = stCreateSymbolTableValueForVariable(variable_node, variable_datatype, scope);
          symbolTableSet(scope, variable_name, new_value, ST_VARIABLE, false);
        }
      plist_ll = plist_ll->ptr3;
    }
}


/**
 * Create symbol tables for the driver module body and add the driver module
 * itself to the global symbol table.
 * @param   statements_ll    The first statement of the statements linked list
 *                           for the driver module.
 */
void
stAddDriverModuleDefinition (struct StatementNode *statements_ll)
{
  struct SymbolTable *driver_scope = newSymbolTable(global_symbol_table, "Driver", NULL);
  stWalkThroughStatements(statements_ll, driver_scope);
  if (st_debug_mode)
    {
      fprintf(stdout, "Module %s's symbol table:\n", driver_scope->scope_tag);
      printSymbolTable(driver_scope);
      printf("\n");
    }
}


/**
 * Walk though each statement (and contained expressions) starting from this
 * statement and appropriately ensure that each LeafNode has it's scope
 * variable set and that all variable declarations are aptly added to the 
 * specified scope.
 * @param   statement   The first statement to begin walking from. Should ideally
 *                      be the first statement of the module.
 * @param   scope       The symbol table / scope of the module itself (it's internal
 *                      scope).
 */
void
stWalkThroughStatements (struct StatementNode *statement_ll, struct SymbolTable *scope)
{
  enum ASTNodesEnum statement_type;
  while (statement_ll != NULL)
    {
      union ASTNodesUnion statement = statement_ll->ptr1->node;
      statement_type = statement_ll->ptr1->type;
      switch (statement_type)
        {
          case INPUT_NODE:
            stHandleInputStatement(statement.inp, scope);
            break;
          case PRINT_NODE:
            stHandlePrintStatement(statement.pri, scope);
            break;
          case ASSIGN_STMT_NODE:
            stHandleAssignmentStatement(statement.agn_stm, scope);
            break;
          case MODULE_REUSE_STMT_NODE:
            stHandleModuleReuseStatement(statement.mod_reu_stm, scope);
            break;
          case DECLARE_STMT_NODE:
            stHandleDeclareStatement(statement.dec_stm, scope);
            break;
          case CONDITIONAL_STMT_NODE:
            stHandleConditionalStatement(statement.con_stm, scope);
            break;
          case FOR_ITERATIVE_STMT_NODE:
            stHandleForLoop(statement.for_ite_stm, scope);
            break;
          case WHILE_ITERATIVE_STMT_NODE:
            stHandleWhileLoop(statement.whi_ite_stm, scope);
            break;
          default:
            fprintf(stderr, "Detected statement with illegal type: %d\n", statement_type);
            exit(EXIT_FAILURE);
        }
      statement_ll = statement_ll->ptr2;
    }
}


/**
 * Given a module name, walk through the symbol tables (moving
 * from child to parent) the find the correct module.
 * NOTE: for variable, use resolveVariable instead.
 */
struct ModuleEntry *
resolveModule (char *module_name)
{
  /* We only need to check the global scope because ERPLAG
   * is single-file based and does not allow nesting of
   * modules/method/functions. */
  struct ModuleEntry *module = NULL;
  struct SymbolTableNode *st_node;
  st_node = symbolTableGet(global_symbol_table, module_name);
  if (st_node != NULL)
    assert(st_node->value_type == ST_VARIABLE);
  module = &(st_node->value.module);
  return module;
}


/**
 * Given a variable name, walk through the symbol tables (moving
 * from child to parent) the find the correct variable.
 * NOTE: for modules, use resolveModule instead.
 */
struct VariableEntry *
resolveVariable (char *variable_name, struct SymbolTable *scope)
{
  /* NOTE: Since ERPLAG does not support global variables we can
   * stop at the global_symbol_table scope. */
  struct VariableEntry *variable = NULL;
  struct SymbolTableNode *st_node;
  while (scope != global_symbol_table)
    {
      st_node = symbolTableGet(scope, variable_name);
      if (st_node == NULL)
        {
          scope = scope->parent;
          continue;
        }
      assert(st_node->value_type == ST_VARIABLE);
      variable = &(st_node->value.variable);
      break;
    }
  return variable;
}


/**
 * Take an input statement such as
 *  `get_value(x);`
 * and update the AST leaf node scope value for the variable appropriately.
 * @param   input_stmt    The input statement node from the AST.
 * @param   scope         The scope that this statement belongs to.
 */
void
stHandleInputStatement (struct InputNode *input_stmt, struct SymbolTable *scope)
{
  struct LeafNode *variable = input_stmt->ptr1;
  variable->scope = scope;
}


/**
 * Take a print statement such as
 *  `print(x);`
 * and update the AST leaf node scope value for the variable appropriately.
 * @param   pri_stmt      The print statement node from the AST.
 * @param   scope         The scope that this statement belongs to.
 */
void
stHandlePrintStatement (struct PrintNode *pri_stmt, struct SymbolTable *scope)
{
  struct LeafNode *variable, *index;
  assert(pri_stmt->ptr1->type == LEAF_NODE || pri_stmt->ptr1->type == ARRAY_NODE);
  if (pri_stmt->ptr1->type == LEAF_NODE)
    variable = pri_stmt->ptr1->node.lea;
  else
    {
      variable = pri_stmt->ptr1->node.arr->ptr1;
      index = pri_stmt->ptr1->node.arr->ptr2;
      index->scope = scope;
    }
  variable->scope = scope;
}


/**
 * Assignment statements look something like:
 *  `x[1] := x[2] + 5 - 2 * y` for example.
 * The grammar rules around expressions are:
 * <assignmentStmt>  ->      ID <whichStmt>
 * <whichStmt>       ->      <lvalueIDStmt> | <lvalueARRStmt>
 * <lvalueIDStmt>    ->      ASSIGNOP <new_expression> SEMICOL
 * <lvalueARRStmt>   ->      SQBO <index> SQBC ASSIGNOP <new_expression> SEMICOL 
 * Though they are better understood as:
 * <assignmentStmt>  ->      ID <arrayPart> ASSIGNOP <new_expression> SEMICOL
 * <arrayPart>       ->      SQBO <index> SQBC | EPSILON
 * Which is what they should have been in the first place (but now it's too late).
 * We can break up the above example as (approximately - just for understanding):
 * AssignStmtNode[LvalueARRNode->ptr1] := LvalueARRNode->ptr2 (which is an Attribute)
 * @param   agn_stmt    The assignment statement to parse.
 * @param   scope       The scope that this statement belongs to.
 */
void
stHandleAssignmentStatement (struct AssignStmtNode *agn_stmt, struct SymbolTable *scope)
{
  struct LeafNode *variable_instance = agn_stmt->ptr1;
  struct VariableEntry *resolved_variable;
  assert(variable_instance->type == IDENTIFIER);
  char *variable_name = variable_instance->value.entry;
  resolved_variable = resolveVariable(variable_name, scope);
  if (resolved_variable == NULL)
    {
      fprintf(stderr, variable_undeclared_error_message, variable_name, variable_instance->line_number);
      semantic_error_count += 1;
      /* Don't even bother parsing the ensuing expression. */
      return;
    }
  assert(agn_stmt->ptr2->type == LVALUE_ARR_NODE || agn_stmt->ptr2->type == LVALUE_ID_NODE);
  if (agn_stmt->ptr2->type == LVALUE_ARR_NODE)
    {
      if (resolved_variable->isArray == false)
        {
          fprintf(stderr, incorrectly_used_as_array_error_message, variable_name,
                  resolved_variable->line_number, variable_instance->line_number);
          semantic_error_count += 1;
          return;
        }
      agn_stmt->ptr2->node.lva_arr->ptr1->scope = scope;  /* This refers to the array index. */
      // TODO: Implement the isStatic boolean flag.
      stWalkThroughExpression(agn_stmt->ptr2->node.lva_arr->ptr2, scope);
    }
  else
    stWalkThroughExpression(agn_stmt->ptr2->node.lva_id->ptr1, scope);
  variable_instance->scope = scope;
}


/**
 * Walk through the parts of a module reuse statement and do two things:
 * 1. Resolve the variables and modules involved, making sure that they exist.
 * 2. Set scopes for all leafnodes.
 */
void
stHandleModuleReuseStatement (struct ModuleReuseStmtNode *mr_stmt, struct SymbolTable *scope)
{

}



/**
 * 
 */
void
stHandleDeclareStatement (struct DeclareStmtNode *dec_stm, struct SymbolTable *scope)
{

}



/**
 * 
 */
void
stHandleConditionalStatement (struct ConditionalStmtNode *con_stm, struct SymbolTable *scope)
{

}



/**
 * 
 */
void
stHandleForLoop (struct ForIterativeStmtNode *for_loop, struct SymbolTable *scope)
{

}



/**
 * 
 */
void
stHandleWhileLoop (struct WhileIterativeStmtNode *while_loop, struct SymbolTable *scope)
{

}


/**
 * 
 */
void
stWalkThroughExpression (struct Attribute *expression, struct SymbolTable *scope)
{
  // TODO.
}
