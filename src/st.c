#include "st.h"
#include <assert.h>

bool st_debug_mode = false;
int semantic_error_count = 0;
extern struct ProgramNode AST;
struct SymbolTable *global_symbol_table = NULL;
struct SymbolTableLinkedList symbol_table_ll;
extern char terminalStringRepresentations[NUM_TERMINALS][16];  // in parser.c

char *module_redecleration_error_message = "Line %d: (Semantic Error) Redeclaration of module \"%s\" on line number %d \
(originally declared on line number %d)\n";
char *module_redefinition_error_message = "Line %d: (Semantic Error) Redefintion of module \"%s\" on line number %d \
(originally defined on line number %d)\n";
char *module_declared_and_defined_before_call_error_message = "Line %d: (Semantic Error) Module \"%s\" can't be declared \
and defined before it has been called.\n";
char *variable_redecleration_error_message = "Line %d: (Semantic Error) Redeclaration of variable \"%s\" on line number %d \
(originally declared on line number %d)\n";
char *variable_undeclared_error_message = "Line %d: (Semantic Error) Variable \"%s\" used on line number %d has not been \
declared.\n";
char *incorrectly_used_as_array_error_message = "Line %d: (Semantic Error) Variable \"%s\" is declared on line %d, but not \
as an array. On line number %d it is being used as an array.\n";
char *module_undeclared_error_message = "Line %d: (Semantic Error) Module \"%s\" used on line number %d has not been \
declared or defined.\n";
char *semantic_errors_detected_message = "Detected %d semantic error(s) while populating the symbol table (more \
may be detected after resolving the above).\n";
char *invalid_datatype_error_message = "%s is not a valid datatype and does not have a memory requirement value.\n";

void set_semantic_num(){
  semantic_error_count = 0;
}

/**
 * A simple wrapper around the method newSymbolTable (from stDef.c) which also adds
 * the newly created symbol table to symbol_table_ll so what we can later print all
 * of the symbol tables in order of creation for the driver module.
 */
struct SymbolTable *
newTrackedSymbolTable (struct SymbolTable *parent, const char *scope_tag, SymbolTableHashFunction hash_fn)
{
  struct SymbolTable *new_symbol_table = newSymbolTable(parent, scope_tag, hash_fn);
  if (symbol_table_ll.head == NULL)
    {
      symbol_table_ll.head = (struct SymbolTableLinkedListNode *) malloc(sizeof(struct SymbolTableLinkedListNode));
      symbol_table_ll.tail = symbol_table_ll.head;
    }
  else
    {
      symbol_table_ll.tail->next = (struct SymbolTableLinkedListNode *) malloc(sizeof(struct SymbolTableLinkedListNode));
      symbol_table_ll.tail = symbol_table_ll.tail->next;
    }
  symbol_table_ll.tail->symbol_table = new_symbol_table;
  symbol_table_ll.tail->next = NULL;
  symbol_table_ll.count += 1;
  return new_symbol_table;
}


/**
 * This is the compiler's entrypoint into the rest of the symbol table code.
 * If called after creating the global AST, it will generate symbol tables
 * for each scope and point each leaf node in the AST towards it's appropriate
 * symbol table.
 */
void
generateSymbolTables ()
{
  symbol_table_ll.count = 0;
  symbol_table_ll.head = NULL;
  symbol_table_ll.tail = NULL;
  global_symbol_table = newTrackedSymbolTable(NULL, "Global", NULL);
  stAddModuleDeclerations(AST.ptr1);
  stAddModuleDefinitions(AST.ptr2);
  stAddDriverModuleDefinition(AST.ptr3);
  stAddModuleDefinitions(AST.ptr4);
  if (st_debug_mode)
    {
      fprintf(stdout, "Global symbol table after generation of symbol tables:\n");
      printSymbolTable(global_symbol_table);
      printf("\n");
    }
  if (semantic_error_count > 0)
    {
      fprintf(stderr, semantic_errors_detected_message, semantic_error_count);
      // exit(EXIT_FAILURE);
    }
  global_symbol_table->opening_line_no = AST.starting_line_number;
  global_symbol_table->closing_line_no = AST.ending_line_number;
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
          fprintf(stderr, module_redecleration_error_message, current_line_no,
                  module_name, current_line_no, original_line_no);
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
stAddModuleDefinitions (struct OtherModuleNode *module_ll)
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
      original_line_no = current_line_no;
      module->ptr1->scope = global_symbol_table;
      existing_node = symbolTableGet(global_symbol_table, module_name);
      if (existing_node)
        {
          existing_module = existing_node->value.module;
          original_line_no = existing_module.def_line_number;
          /* If there was this is a re-definition then print an error message
           * and continue processing but don't add this to the symbol table.
           * We consider the first definition to be the true definition. */
          if (existing_module.def_line_number != -1)
            {
              fprintf(stderr, module_redefinition_error_message, current_line_no,
                      module_name, current_line_no, original_line_no);
              semantic_error_count += 1;
            }
          /* If the function has been defined before but not called then that's also
           * a special kind of error (check test case "stage 2/semantic/t8.txt" line
           * number 130. I didn't make the rules, I'm just following them. The dec
           * check is kinda redundant. */
          else if (existing_module.dec_line_number != -1 && !existing_module.called)
            {
              fprintf(stderr, module_declared_and_defined_before_call_error_message,
                      current_line_no, module_name);
              semantic_error_count += 1;
            }
        }
      /* We can directly add this module to the symbol table. The false in the symbol table
       * set call will prevent overwriting of an existing definition in the symbol table. */
      new_value = stCreateSymbolTableValueForModule(module_name,
        original_line_no, current_line_no, module->ptr2, module->ptr3);
      symbolTableSet(global_symbol_table, module_name, new_value, ST_MODULE, true);

      /* Even if there's a semantic error, parse it. No harm. */
      module_scope = newTrackedSymbolTable(global_symbol_table, module_name, NULL);
      module_scope->is_module_scope = true;
      module_scope->opening_line_no = module->starting_line_number;
      module_scope->closing_line_no = module->ending_line_number;

      /* Because of the negative memory offset hack to make the implementation of function
       * calls (module reuse statements) easier to implment we have to process the outputplist
       * first then the inputplist. */
      stAddOutputPlistToScope(module->ptr3, module_scope);
      stAddInputPlistToScope(module->ptr2, module_scope);

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
  new_value.module.called = false;
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
 * We also handle updating the memory requirement of the encapsulating module's
 * activation record.
 * @param   varnode       The leafnode containing the variable name.
 * @param   dtnode        The attribute containing the variable datatype.
 * @param   scope         If this is not NULL, then this will be assigned to the
 *                        leafnodes that we come across in this method.
 * @returns               The populated SymbolTableValue struct for the variable.
 */
union SymbolTableValue
stCreateSymbolTableValueForVariable (struct LeafNode *varnode, struct Attribute *dtnode,
                                     struct SymbolTable *scope, bool is_input, bool is_io)
{
  char *name;
  bool is_array, is_static;
  enum terminal basetype;
  int line_number, datasize, memoffset;
  struct LeafNode *lower_bound_leaf_node, *upper_bound_leaf_node;
  struct VariableEntry *lower_bound, *upper_bound;
  struct ModuleEntry *module;
  struct SymbolTable *module_scope;
  
  assert(varnode != NULL);
  assert(varnode->type == IDENTIFIER);
  name = varnode->value.entry;
  line_number = varnode->line_number;
  varnode->scope = scope;

  module_scope = getModuleLevelScope(scope);
  module = getModuleEntry(module_scope->scope_tag);

  if (is_io)
    {
      /* Part of a quick and hack to make implementing function calls easier. */
      module->io_record_size -= 8;
      memoffset = module->io_record_size;
    }
  else
    memoffset = module->activation_record_size;
  module->activation_record_size += 8;

  if (dtnode->type == ARRAY_TYPE_NODE)
    {
      assert(dtnode->node.arr_typ->ptr1->nullData == true);
      assert(dtnode->node.arr_typ->ptr2->ptr1->type == NUM ||
             dtnode->node.arr_typ->ptr2->ptr1->type == IDENTIFIER);
      assert(dtnode->node.arr_typ->ptr2->ptr2->type == NUM ||
             dtnode->node.arr_typ->ptr2->ptr2->type == IDENTIFIER);
      
      is_array = true;
      basetype = dtnode->node.arr_typ->ptr1->type;
      lower_bound_leaf_node = dtnode->node.arr_typ->ptr2->ptr1;
      lower_bound = stNewTemporaryVariable(scope, INTEGER);
      upper_bound_leaf_node = dtnode->node.arr_typ->ptr2->ptr2;
      upper_bound = stNewTemporaryVariable(scope, INTEGER);
      if (lower_bound_leaf_node->type == IDENTIFIER || upper_bound_leaf_node->type == IDENTIFIER) {
        if (lower_bound_leaf_node->type == IDENTIFIER) // We shouldn't need to do this check twice...
          strcpy(lower_bound->value.str, lower_bound_leaf_node->value.entry);
        if (upper_bound_leaf_node->type == IDENTIFIER) // We shouldn't need to do this check twice...
          strcpy(upper_bound->value.str, upper_bound_leaf_node->value.entry);
        is_static = false;
      }
      else {
        is_static = true;
        /* Populate the lower bound and upper bound here itself instead of in the intermediate code.
         * Yes, this IS a hack. */
        lower_bound->datatype = NUM;
        lower_bound->value.num = lower_bound_leaf_node->value.num;
        upper_bound->datatype = NUM;
        upper_bound->value.num = upper_bound_leaf_node->value.num;
      }
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
      lower_bound = NULL;
      upper_bound = NULL;
    }


  if (is_array && is_io)
    module->io_record_size -= (8 + 8);

  datasize = getMemorySizeofDatatype(basetype, is_array);
  if (is_array && is_static) {
    datasize = (upper_bound_leaf_node->value.num - \
                lower_bound_leaf_node->value.num + 1) * 8;
    module->activation_record_size += datasize;
  }

  union SymbolTableValue new_value;
  memset(&new_value, 0, sizeof(union SymbolTableValue));
  assert(strlen(name) <= IDENTIFIER_NAME_MAX_LEN);
  strcpy(new_value.variable.name, name);
  new_value.variable.isArray = is_array;
  new_value.variable.isStatic = is_static;
  new_value.variable.isInput = is_input;
  new_value.variable.isOutput = !is_input && is_io;
  new_value.variable.datatype = basetype;
  new_value.variable.line_number = line_number;
  new_value.variable.lower_bound = lower_bound;
  new_value.variable.upper_bound = upper_bound;
  new_value.variable.mem_offset = memoffset;

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
  struct InputPlistNode *prev = NULL, *next;

  /* reverse the list order temporarily */
  while (plist_ll != NULL)
    {
      next = plist_ll->ptr3;
      plist_ll->ptr3 = prev;
      prev = plist_ll;
      plist_ll = next;
    }
  plist_ll = prev;

  while (plist_ll != NULL)
    {
      variable_node = plist_ll->ptr1;
      variable_datatype = plist_ll->ptr2;
      variable_name = variable_node->value.entry;
      existing_node = symbolTableGet(scope, variable_name);
      if (existing_node)
        {
          fprintf(stderr, variable_redecleration_error_message, new_value.variable.line_number,
                  variable_name, new_value.variable.line_number, existing_node->value.variable.line_number);
          semantic_error_count += 1;
        }
      else
        {
          new_value = stCreateSymbolTableValueForVariable(variable_node, variable_datatype, scope, true, true);
          symbolTableSet(scope, variable_name, new_value, ST_VARIABLE, false);
        }
      plist_ll = plist_ll->ptr3;
    }

  /* reverse the list order again to put things in the right order.
   * We do this in a separate loop for modularity. */
  plist_ll = prev;
  prev = NULL;
  while (plist_ll != NULL)
    {
      next = plist_ll->ptr3;
      plist_ll->ptr3 = prev;
      prev = plist_ll;
      plist_ll = next;
    }
  plist_ll = prev;
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
  struct OutputPlistNode *prev = NULL, *next;

  /* reverse the list order temporarily */
  while (plist_ll != NULL)
    {
      next = plist_ll->ptr3;
      plist_ll->ptr3 = prev;
      prev = plist_ll;
      plist_ll = next;
    }
  plist_ll = prev;

  while (plist_ll != NULL)
    {
      variable_node = plist_ll->ptr1;
      variable_datatype = plist_ll->ptr2;
      variable_name = variable_node->value.entry;
      existing_node = symbolTableGet(scope, variable_name);
      if (existing_node)
        {
          fprintf(stderr, variable_redecleration_error_message, new_value.variable.line_number,
                  variable_name, new_value.variable.line_number, existing_node->value.variable.line_number);
          semantic_error_count += 1;
        }
      else
        {
          new_value = stCreateSymbolTableValueForVariable(variable_node, variable_datatype, scope, false, true);
          symbolTableSet(scope, variable_name, new_value, ST_VARIABLE, false);
        }
      plist_ll = plist_ll->ptr3;
    }
  
  /* reverse the list order again to put things in the right order.
   * We do this in a separate loop for modularity. */
  plist_ll = prev;
  prev = NULL;
  while (plist_ll != NULL)
    {
      next = plist_ll->ptr3;
      plist_ll->ptr3 = prev;
      prev = plist_ll;
      plist_ll = next;
    }
  plist_ll = prev;
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
  char *module_name = "driver";
  union SymbolTableValue new_value;
  struct SymbolTable *driver_scope = newTrackedSymbolTable(global_symbol_table, module_name, NULL);

  driver_scope->is_module_scope = true;
  new_value = stCreateSymbolTableValueForModule("driver", -1, -1, NULL, NULL);
  /* We actually don't have the line number information for the driver module. */
  symbolTableSet(global_symbol_table, module_name, new_value, ST_MODULE, false);
  
  stWalkThroughStatements(statements_ll, driver_scope);
  if (st_debug_mode)
    {
      fprintf(stdout, "Module %s's symbol table:\n", driver_scope->scope_tag);
      printSymbolTable(driver_scope);
      printf("\n");
    }

  driver_scope->opening_line_no = AST.driver_starting_line_number;
  driver_scope->closing_line_no = AST.driver_ending_line_number;
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
  struct SymbolTable *inner_scope;
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
            inner_scope = newTrackedSymbolTable(scope, "Conditional Stmt", NULL);
            stHandleConditionalStatement(statement.con_stm, inner_scope);
            break;
          case FOR_ITERATIVE_STMT_NODE:
            inner_scope = newTrackedSymbolTable(scope, "For Loop", NULL);
            stHandleForLoop(statement.for_ite_stm, inner_scope);
            break;
          case WHILE_ITERATIVE_STMT_NODE:
            inner_scope = newTrackedSymbolTable(scope, "While Loop", NULL);
            stHandleWhileLoop(statement.whi_ite_stm, inner_scope);
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
    assert(st_node->value_type == ST_MODULE);
  else
    return NULL;
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
  stUpdateLeafNode(variable, scope);
}


/**
 * Take a print statement such as
 *    print(x);
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
  stUpdateLeafNode(variable, scope);
}


/**
 * Assignment statements look something like:
 *    x[1] := x[2] + 5 - 2 * y
 * for example.
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
      fprintf(stderr, variable_undeclared_error_message, variable_instance->line_number,
              variable_name, variable_instance->line_number);
      semantic_error_count += 1;
    }
  assert(agn_stmt->ptr2->type == LVALUE_ARR_NODE || agn_stmt->ptr2->type == LVALUE_ID_NODE);
  if (agn_stmt->ptr2->type == LVALUE_ARR_NODE)
    {
      if (resolved_variable != NULL && resolved_variable->isArray == false)
        {
          fprintf(stderr, incorrectly_used_as_array_error_message, variable_instance->line_number,
                  variable_name, resolved_variable->line_number, variable_instance->line_number);
          semantic_error_count += 1;
        }
      agn_stmt->ptr2->node.lva_arr->ptr1->scope = scope;  /* This refers to the array index. */
      // TODO: Implement the isStatic boolean flag.
      stWalkThroughExpression(agn_stmt->ptr2->node.lva_arr->ptr2, scope);
    }
  else
    stWalkThroughExpression(agn_stmt->ptr2->node.lva_id->ptr1, scope);
  stUpdateLeafNode(variable_instance, scope);
}


/**
 * Walk through the parts of a module reuse statement and do two things:
 * 1. Resolve the variables and modules involved, making sure that they exist.
 * 2. Set scopes for all leafnodes.
 * example: 
 *    [x, y] := use module foo with parameters p1, p2;
 *  
 * @param   mr_stmt     The module reuse statement to handle.
 * @param   scope       The scope that this statement belongs to.
 */
void
stHandleModuleReuseStatement (struct ModuleReuseStmtNode *mr_stmt, struct SymbolTable *scope)
{
  int line_number;
  char *module_name;
  struct ModuleEntry *module;
  struct IdListNode *inputs_ll, *outputs_ll;
  assert(mr_stmt->ptr2->type == IDENTIFIER);
  module_name = mr_stmt->ptr2->value.entry;
  line_number = mr_stmt->ptr2->line_number;
  inputs_ll = mr_stmt->ptr3;
  outputs_ll = mr_stmt->ptr1;

  module = resolveModule(module_name);
  if (module == NULL)
    {
      fprintf(stderr, module_undeclared_error_message, line_number, module_name, line_number);
      semantic_error_count += 1;
    }
  else
    {
      module->called = true;
      module->activation_record_size = (8 + 8);
      module->io_record_size = 0;
    }
  mr_stmt->ptr2->scope = global_symbol_table;
  while (inputs_ll != NULL)
    {
      stUpdateLeafNode(inputs_ll->ptr1, scope);
      inputs_ll = inputs_ll->ptr2;
    }
  while (outputs_ll != NULL)
    {
      stUpdateLeafNode(outputs_ll->ptr1, scope);
      outputs_ll = outputs_ll->ptr2;
    }
}



/**
 * Handle a new variable declerations. In a single declare statement, multiple
 * variables of the same type can be declared. We need to ensure that there are
 * no re-declerations and that each leafnode for each variable name is tied to
 * the appropriate scope.
 * @param   dec_stmt    The declare statement to handle.
 * @param   scope       The scope that this statement belongs to.
 */
void
stHandleDeclareStatement (struct DeclareStmtNode *dec_stmt, struct SymbolTable *scope)
{
  char *variable_name;
  struct Attribute *dtnode = dec_stmt->ptr2;
  struct IdListNode *variable_ll = dec_stmt->ptr1;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  bool overwrite = false;
  while (variable_ll != NULL)
    {
      assert(variable_ll->ptr1->type == IDENTIFIER);
      variable_name = variable_ll->ptr1->value.entry;
      /* Set it's scope. */
      variable_ll->ptr1->scope = scope;
      /* Make sure that the variable does not already exist in this scope. */
      existing_node = symbolTableGet(scope, variable_name);
      if (existing_node != NULL)
        {
          if (existing_node->value.variable.isInput) {
            overwrite = true;
          } else {
            fprintf(stderr, variable_redecleration_error_message, variable_ll->ptr1->line_number,
                    variable_name, variable_ll->ptr1->line_number, existing_node->value.variable.line_number);
            semantic_error_count += 1;
          }
        }

      /* If the variable does not already exist in this scope, add it to
       * the symbol table / scope. This is controlled by overwrite */
        assert(dtnode != NULL);
        new_value = stCreateSymbolTableValueForVariable(variable_ll->ptr1, dtnode, scope, false, false);
        symbolTableSet(scope, variable_name, new_value, ST_VARIABLE, overwrite);
        variable_ll = variable_ll->ptr2;
    }
  
}



/**
 * A conditional statment in ERPLAG is basically a switch statement. So for each case we
 * need to create a new scope and then walk through all of the inner statements.
 * @param   con_stmt    The conditional statement to handle.
 * @param   scope       The scope that this statement belongs to.
 */
void
stHandleConditionalStatement (struct ConditionalStmtNode *con_stmt, struct SymbolTable *scope)
{
  struct LeafNode *conditional_variable = con_stmt->ptr1;
  struct CaseStmtNode *cases_ll = con_stmt->ptr2;
  struct StatementNode *default_case_statements = con_stmt->ptr3;

  assert(conditional_variable->type == IDENTIFIER);
  stUpdateLeafNode(conditional_variable, scope);
  while (cases_ll != NULL)
    {
      stUpdateLeafNode(cases_ll->ptr1, scope);
      stWalkThroughStatements(cases_ll->ptr2, scope);
      cases_ll = cases_ll->ptr3;  
    }
  stWalkThroughStatements(default_case_statements, scope);
  if (st_debug_mode)
    {
      fprintf(stdout, "Case statement symbol table:\n");
      printSymbolTable(scope);
      printf("\n");
    }

  scope->opening_line_no = con_stmt->starting_line_number;
  scope->closing_line_no = con_stmt->ending_line_number;
}



/**
 * Create a new scope for the for loop and then walk through it's statements using
 * this new loop. After resolving the loop variable, point it's scope towards the
 * the scope encapsulating the for loop.
 * @param   for_loop    The for loop's statement node.
 * @param   scope       The scope for the for loop.
 */
void
stHandleForLoop (struct ForIterativeStmtNode *for_loop, struct SymbolTable *scope)
{
  struct LeafNode *loop_variable = for_loop->ptr1;
  struct StatementNode *loop_body = for_loop->ptr3;

  for_loop->ptr2->ptr1->scope = scope;  /* Loop lower bound. */
  for_loop->ptr2->ptr2->scope = scope;  /* Loop upper bound. */
  stUpdateLeafNode(loop_variable, scope);
  /* We do this to say that this variable is being used in this scope, not that it
   * belongs in this scope (it would have been declared in a parent scope) */

  /* Checking the type of the loop variable and making sure that it is an integer type will
   * be left to later stages of semantic checking. */

  stWalkThroughStatements(loop_body, scope);

  if (st_debug_mode)
    {
      fprintf(stdout, "For loop symbol table after generation of symbol tables:\n");
      printSymbolTable(scope);
      printf("\n");
    }
  
  scope->opening_line_no = for_loop->starting_line_number;
  scope->closing_line_no = for_loop->ending_line_number;
}



/**
 * Create a new scope for the body of the while loop and walk through it. Also
 * walk through the body of the whlie loop under the context of a new (inner) scope.
 * @param   while_loop    The while loop node to parse.
 * @param   scope         The scope for the while loop.
 */
void
stHandleWhileLoop (struct WhileIterativeStmtNode *while_loop, struct SymbolTable *scope)
{
  struct Attribute *conditional_expression = while_loop->ptr1;
  struct StatementNode *loop_body = while_loop->ptr2;
  stWalkThroughExpression(conditional_expression, scope->parent);
  stWalkThroughStatements(loop_body, scope);
  if (st_debug_mode)
    {
      fprintf(stdout, "While loop symbol table after generation of symbol tables:\n");
      printSymbolTable(scope);
      printf("\n");
    }
  
  scope->opening_line_no = while_loop->starting_line_number;
  scope->closing_line_no = while_loop->ending_line_number;
}


/**
 * Walk through an expression and make sure that every variable has been declared
 * before and then assign the given scope to each variable.
 */
void
stWalkThroughExpression (struct Attribute *expression, struct SymbolTable *scope)
{
  switch (expression->type)
    {
      case U_NODE:
        stWalkThroughExpression(expression->node.u->ptr1, scope);
        break;
      case N7_NODE:
        stWalkThroughExpression(expression->node.n7->ptr1, scope);
        stWalkThroughExpression(expression->node.n7->ptr2, scope);
        break;
      case N8_NODE:
        stWalkThroughExpression(expression->node.n8->ptr1, scope);
        stWalkThroughExpression(expression->node.n8->ptr2, scope);
        break;
      case ARITHMETIC_EXPR_NODE:
        if (expression->node.ari_exp->is_first)
          stWalkThroughExpression(expression->node.ari_exp->ptr1, scope);
        stWalkThroughExpression(expression->node.ari_exp->ptr2, scope);
        stWalkThroughExpression(expression->node.ari_exp->ptr3, scope);
        break;
      case TERM_NODE:
        if (expression->node.ter->is_first)
          stWalkThroughExpression(expression->node.ter->ptr1, scope);
        stWalkThroughExpression(expression->node.ter->ptr2, scope);
        stWalkThroughExpression(expression->node.ter->ptr3, scope);
        break;
      case ARRAY_NODE:
        stUpdateLeafNode(expression->node.arr->ptr1, scope);
        stUpdateLeafNode(expression->node.arr->ptr2, scope);
        break;
      case LEAF_NODE:
        stUpdateLeafNode(expression->node.lea, scope);
        break;
      case NULL_NODE:
        break;
      default:
        fprintf(stderr, "Detected expression with illegal type: %d\n", expression->type);
        exit(EXIT_FAILURE);
    }
}


/**
 * Given an AST leafnode, if it is a variable, resolve assign it the scope.
 * @param   lea     The leafnode.
 * @param   scope   Scope which the statment/expression the leafnode belongs to.
*/
void
stUpdateLeafNode (struct LeafNode *lea, struct SymbolTable *scope)
{
  if (lea->type == IDENTIFIER)
    {
      if (resolveVariable(lea->value.entry, scope) == NULL)
        {
          fprintf(stderr, variable_undeclared_error_message, lea->line_number,
                  lea->value.entry, lea->line_number);
          semantic_error_count += 1;
        }
    }
  lea->scope = scope;
}


/**
 * Given a scope and a datatype, create a new temporary variable and add it to the closest
 * module level symbol table. This function also handles updating the module's entry in the
 * symbol table and other related tasks.
 * @param     scope     The scope of an leaf node - we will find the module that this scope
 *                      belongs to and then use that for creating a new temporary variable.
 * @param     datatype  The datatype to allocate a new temporary variable for.
 * @returns             A pointer to the variable entry that actually lies in the symbol table.
 */
struct VariableEntry *
stNewTemporaryVariable (struct SymbolTable *scope, enum terminal datatype)
{
  int datasize;
  struct ModuleEntry *module;
  char temp_var_name[7];
  union SymbolTableValue new_variable;
  struct SymbolTableNode *new_variable_node;
  
  scope = getModuleLevelScope(scope);
  module = getModuleEntry(scope->scope_tag);
  sprintf(temp_var_name, ST_TEMP_VAR_FORMAT, module->num_temp_var);

  datasize = getMemorySizeofDatatype(datatype, false);

  strcpy(new_variable.variable.name, temp_var_name);
  new_variable.variable.line_number = -1;
  new_variable.variable.datatype = datatype;
  new_variable.variable.isArray = false;
  new_variable.variable.isStatic = false;
  new_variable.variable.isTemporary = true;
  new_variable.variable.isInput = false;
  new_variable.variable.lower_bound = NULL;
  new_variable.variable.upper_bound = NULL;
  new_variable.variable.mem_offset = module->activation_record_size;

  symbolTableSet(scope, temp_var_name, new_variable, ST_VARIABLE, false);
  
  module->num_temp_var += 1;
  module->activation_record_size += datasize;

  /* For convenience, instead of returning the module's name return the 
   * VariableEntry that exists in the symbol table. */
  new_variable_node = symbolTableGet(scope, temp_var_name);
  return &(new_variable_node->value.variable);
}


/**
 * Traverse through parent scopes to find the encapsulating module level
 * scope. This scope is then usually used for updating activation record
 * sizes, for memory offset calculation, and for adding new temporary
 * variables.
 * @param     scope     The scope of the leaf node for which we want to find
 *                      the encapsulating module scope.
 * @returns             The encapsulating module scope itself.
 */
struct SymbolTable *
getModuleLevelScope (struct SymbolTable *scope)
{
  while (scope != NULL && scope->is_module_scope == false)
    scope = scope->parent;
  assert(scope != NULL);
  return scope;
}


/**
 * Given the name of a module, retrieve it's ModuleEntry from the symbol table.
 * @param     module_name     Name of the module
 * @returns                   The corresponding ModuleEntry from the global
 *                            symbol table.
 */
struct ModuleEntry *
getModuleEntry (char *module_name)
{
  struct SymbolTableNode *module_node;
  module_node = symbolTableGet(global_symbol_table, module_name);
  assert(module_node->value_type == ST_MODULE);
  return &(module_node->value.module);
}


/**
 * Basically a static map in the form of a callable function. Maps datatypes
 * to how much memory they require (in bytes).
 * @param     datatype      The datatype itself.
 * @param     is_array      Set to true it's an array, then datatype will be ignored.
 * @returns                 The number of bytes required to store that datatype.
 */
int
getMemorySizeofDatatype (enum terminal datatype, bool is_array)
{
  if (is_array)
    return DT_ARRAY_POINTER_SIZE;

  switch (datatype)
    {
      case (NUM):
      case (INTEGER):
        // return DT_INTEGER_SIZE;
        return 2;
      case (RNUM):
      case (REAL):
        // return DT_REAL_SIZE;
        return 4;
      case (TRUE_):
      case (FALSE_):
      case (BOOLEAN_):
        // return DT_BOOL_SIZE;
        return 1;
      default:
        fprintf(stderr, invalid_datatype_error_message, terminalStringRepresentations[datatype]);
        exit(EXIT_FAILURE);
    }
}


/**
 * Print out all symbol table in a way that meets the requirements for the driver.
 */
void
printSymbolTablesForDriver ()
{
  struct SymbolTableLinkedListNode *cursor = symbol_table_ll.head->next;  // Don't print the global symbol table itself.
  printf("%-15s %-20s %-25s %-10s %-10s %-20s %-15s %-20s %-10s %s\n",
    "variable_name", "scope(module_name)", "scope(line_numbers)", "width", "isArray",
    "static_or_dynamic", "range_lexemes", "type_of_element", "offset", "nesting_level");
  while (cursor != NULL)
    {
      printSymbolTableForDriver(cursor->symbol_table);
      cursor = cursor->next;
    }
}

/**
 * A quick utility function to get the lowercase string version of a datatype.
 */
char *
getLowerCaseDatatypeString (enum terminal datatype)
{
  switch (datatype)
    {
      case (NUM):
      case (INTEGER):
        return "integer";
      case (RNUM):
      case (REAL):
        return "real";
      case (TRUE_):
      case (FALSE_):
      case (BOOLEAN_):
        return "boolean";
      default:
        return "???";
    }
}

/**
 * Print out a single symbol table in a way that meets the requirements for the driver.
 */
void
printSymbolTableForDriver (struct SymbolTable *st)
{
  int data_len, nesting_level = 0;
  char *current_key, *is_array, *is_static, *current_datatype;
  char lower_bound[128], upper_bound[128], range_lexemes[256], scope_range[256];
  struct SymbolTable *module_scope = getModuleLevelScope(st);
  struct SymbolTableNode *current_node;
  struct VariableEntry current_variable;
  
  for (int i = 0; i < strl_len(st->keys); ++i)
    {
      current_key = strl_get(st->keys, i);
      current_node = symbolTableGet(st, current_key);
      current_variable = current_node->value.variable;
      current_datatype = getLowerCaseDatatypeString(current_variable.datatype);
      data_len = getMemorySizeofDatatype(current_variable.datatype, current_variable.isArray);

      if (current_variable.isTemporary)
        continue;
      
      if (current_variable.isArray)
        {
          is_array = "yes";

          if (current_variable.isStatic)
            is_static = "static";
          else
            is_static = "dynamic";

          if (current_variable.lower_bound->datatype == NUM)
            sprintf(lower_bound, "%d", current_variable.lower_bound->value.num);
          else
            strcpy(lower_bound, current_variable.lower_bound->value.str);
          if (current_variable.upper_bound->datatype == NUM)
            sprintf(upper_bound, "%d", current_variable.upper_bound->value.num);
          else
            strcpy(upper_bound, current_variable.upper_bound->value.str);

          sprintf(range_lexemes, "[%s, %s]", lower_bound, upper_bound);
        }
      else
        {
          is_array = "no";
          is_static = "---";
          strcpy(lower_bound, "---");
          strcpy(upper_bound, "---");
          strcpy(range_lexemes, "---");
        }

      sprintf(scope_range, "%d-%d", st->opening_line_no, st->closing_line_no);

      if (current_variable.isInput || current_variable.isOutput)
        nesting_level = 0;
      else
        nesting_level = st->nesting_level;
      

      printf("%-15s %-20s %-25s %-10d %-10s %-20s %-15s %-20s %-10d %d\n",
        current_key, module_scope->scope_tag, scope_range, data_len, is_array, is_static,
        range_lexemes, current_datatype, current_variable.mem_offset, nesting_level);
    }
}
