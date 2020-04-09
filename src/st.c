#include "st.h"
#include <assert.h>

bool st_debug_mode = false;
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
            }
        }
       else
        {
          if (requires_prior_declaration)
              fprintf(stderr, module_missing_declaration_error_message, module_name, current_line_no);
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
      // TODO: Handle the statements
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
        fprintf(stderr, variable_redecleration_error_message, variable_name,
                new_value.variable.line_number, existing_node->value.variable.line_number);
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
        fprintf(stderr, variable_redecleration_error_message, variable_name,
                new_value.variable.line_number, existing_node->value.variable.line_number);
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
}
