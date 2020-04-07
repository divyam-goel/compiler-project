#include "st.h"
#include <assert.h>

extern struct ProgramNode AST;
struct SymbolTable *global_symbol_table = NULL;
extern char terminalStringRepresentations[NUM_TERMINALS][16];  /* From parser.c */

bool st_debug_mode = false;

union lexeme getDefaultValueForType(enum terminal datatype) {
  union lexeme default_value;

  switch (datatype) {
    case INTEGER:
      default_value.num = 0;
      break;

    case REAL:
      default_value.rnum = 0.0;
      break;

    case BOOLEAN_:
      default_value.boolean = false;
      break;

    default:
      fprintf(stderr, "getDefaultValueForType: Got an unexpected datatype: %s.\n",
              terminalStringRepresentations[datatype]);
      exit(EXIT_FAILURE);
  }

  return default_value;
}


int getNumVarVal(char *identifier_name, struct SymbolTable *current_scope, int line_number) {
  struct SymbolTableNode *node;

  node = symbolTableGet(current_scope, identifier_name);
  if (node == NULL) {
    printf("The variable %s was not declared but it's being used on line number %d.\n",
           identifier_name, line_number);
    exit(EXIT_FAILURE);
  }
  if (node->value.variable.datatype != NUM) {
    printf("The variable %s is being used in a dynamic range on line number %d and must be an integer.\n",
           identifier_name, line_number);
    exit(EXIT_FAILURE);
  }

  return node->value.variable.value.num;
}


int getIndexValue(struct LeafNode *lea, struct SymbolTable *current_scope, int line_number) {
  if (lea->type == IDENTIFIER)
    return getNumVarVal(lea->value.entry, current_scope, line_number);
  else if (lea->type == NUM) {
    return lea->value.num;
  }
  printf("Dynamic range values on line number %d and must be integers.\n", line_number);
  exit(EXIT_FAILURE);
}



void handleVariablesDecleration(struct DeclareStmtNode *declaration,
                                struct SymbolTable *current_scope) {
  bool is_array;
  int line_number;
  char *identifier_name;
  enum terminal base_datatype;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  struct IdListNode *current_identifier;
  struct ArrayTypeNode *arr_type_node;
  struct DynamicRangeNode *dyn_range_node;
  struct Attribute *datatype_node = declaration->ptr2;

  current_identifier = declaration->ptr1;
  line_number = current_identifier->ptr1->line_number;
  if (datatype_node->type == ARRAY_TYPE_NODE) {
     is_array = true;
     arr_type_node = datatype_node->node.arr_typ;
     base_datatype = arr_type_node->ptr1->type;
     dyn_range_node = arr_type_node->ptr2;
   } else {
     is_array = false;
     arr_type_node = NULL;
     base_datatype = datatype_node->node.lea->type;
     dyn_range_node = NULL;
   }

  while (current_identifier != NULL) {

    identifier_name = current_identifier->ptr1->value.entry;
    existing_node = symbolTableGet(current_scope, identifier_name);
    if (existing_node != NULL) {
      fprintf(stderr, "SEMANTIC ERROR: Detected a redecleration of variable \"%s\" on line %d. \
Previously declared on line %d.\n", identifier_name, line_number,
existing_node->value.variable.line_number);
      exit(EXIT_FAILURE);
    }

    strcpy(new_value.variable.name, identifier_name);
    new_value.variable.line_number = line_number;
    new_value.variable.value = getDefaultValueForType(base_datatype);  // TODO: Rework this for arrays.
    new_value.variable.datatype = base_datatype;
    new_value.variable.isArray = is_array;
    if (is_array) {
      new_value.variable.lower_bound = getIndexValue(dyn_range_node->ptr1,
        current_scope, line_number); 
      new_value.variable.upper_bound = getIndexValue(dyn_range_node->ptr2,
        current_scope, line_number); 
    } else {
      new_value.variable.lower_bound = 0;
      new_value.variable.upper_bound = 0;
    }
    new_value.variable.mem_offset = NULL; // TODO: Figure out what to do with the memory offset.

    symbolTableSet(current_scope, new_value.variable.name, new_value, ST_VARIABLE, false);

    current_identifier->ptr1->scope = current_scope;
    current_identifier = current_identifier->ptr2;

  }
}


void handleExpression(struct Attribute *expression,
                      struct SymbolTable *current_scope) {
  /* Deal with updating the scope of every leafnode of the input expression. */
  switch (expression->type) {
    case N7_NODE:
      handleExpression(expression->node.n7->ptr1, current_scope);
      handleExpression(expression->node.n7->ptr2, current_scope);
      break;
    case N8_NODE:
      handleExpression(expression->node.n8->ptr1, current_scope);
      handleExpression(expression->node.n8->ptr2, current_scope);
      break;
    case ARITHMETIC_EXPR_NODE:
      handleExpression(expression->node.ari_exp->ptr1, current_scope);
      handleExpression(expression->node.ari_exp->ptr2, current_scope);
      handleExpression(expression->node.ari_exp->ptr3, current_scope);
      break;
    case TERM_NODE:
      handleExpression(expression->node.ter->ptr1, current_scope);
      handleExpression(expression->node.ter->ptr2, current_scope);
      handleExpression(expression->node.ter->ptr3, current_scope);
      break;
    case LEAF_NODE:
      /* Base Case. */
      expression->node.lea->scope = current_scope;
      break;
    case ARRAY_NODE:
      /* Also a Base Case. */
      expression->node.arr->ptr1->scope = current_scope;
      expression->node.arr->ptr2->scope = current_scope;
    case NULL_NODE:
      /* Also a Base Case. */
      break;
    default:
      fprintf(stderr, "Received an unexpected type for expression \
attribute: %d\n", expression->type);
      exit(EXIT_FAILURE);
  }
}


void handleStatement(struct StatementNode *current_statement,
                     struct SymbolTable *current_scope) {
  /* Handle updating the symbol table for a single given statement. We also
   * handle updating the scope for each leafnode instance of a variable here,
   * as painful as it might be, it's necessary. */
  struct SymbolTable *inner_scope;
  struct StatementNode *inner_statement;  /* First stmt of the inner scope. */
  struct LeafNode *index;
  struct Attribute *current_attribute = current_statement->ptr1;
  struct WhileIterativeStmtNode *while_loop;
  struct ForIterativeStmtNode *for_loop;
  struct DeclareStmtNode *declare_stmt;
  struct ConditionalStmtNode *cond_stmt;

  handleAttribute:
  switch (current_attribute->type) {
    case WHILE_ITERATIVE_STMT_NODE:
      /* Semantic checking of the while loop conditional expression
        * will not be handled at this step. */
      inner_scope = newSymbolTable(current_scope, "<WhileLoop>", NULL);
      while_loop = current_attribute->node.whi_ite_stm;
      inner_statement = while_loop->ptr2;
      walkThoughStatements(inner_statement, inner_scope);
      if (st_debug_mode) {
        printf("DEBUG WHILE LOOP SCOPE: \n");
        printSymbolTable(inner_scope);
        printf("\n");
      }
      break;

    case FOR_ITERATIVE_STMT_NODE:
      /* Semantic checking of the for loop conditional expression
        * will not be handled at this step. */
      inner_scope = newSymbolTable(current_scope, "<ForLoop>", NULL);
      for_loop = current_attribute->node.for_ite_stm;
      inner_statement = for_loop->ptr3;
      walkThoughStatements(inner_statement, inner_scope);
      if (st_debug_mode) {
        printf("DEBUG FOR LOOP SCOPE: \n");
        printSymbolTable(inner_scope);
        printf("\n");
      }
      break;

    case DECLARE_STMT_NODE:
      declare_stmt = current_attribute->node.dec_stm;
      handleVariablesDecleration(declare_stmt, current_scope);
      break;

    case INPUT_NODE:
      current_attribute->node.inp->ptr1->scope = current_scope;
      break;

    case PRINT_NODE:
      switch (current_attribute->node.pri->ptr1->type) {
        case LEAF_NODE:
          current_attribute->node.pri->ptr1->node.lea->scope = current_scope;
          break;
        case ARRAY_NODE:
          current_attribute->node.pri->ptr1->node.arr->ptr1->scope = current_scope;
          break;
        default:
          fprintf(stderr, "Invalid case for PrintNode attribute.\n");
          exit(EXIT_FAILURE);
          break;
      }
      break;

    case ASSIGN_STMT_NODE:
      current_attribute->node.agn_stm->ptr1->scope = current_scope;
      current_attribute = current_attribute->node.agn_stm->ptr2; /* One of the last two cases. */
      goto handleAttribute;  // One of the few times goto is acceptable. Go back to switch.
      break;

    case MODULE_REUSE_STMT_NODE:
      current_attribute->node.mod_reu_stm->ptr2->scope = current_scope;
      break;

    case CONDITIONAL_STMT_NODE:
      inner_scope = newSymbolTable(current_scope, "<Conditional>", NULL);
      cond_stmt = current_attribute->node.con_stm;
      inner_statement = cond_stmt->ptr3;
      walkThoughStatements(inner_statement, inner_scope);
      if (st_debug_mode) {
        printf("DEBUG CONDITIONAL STATEMENT SCOPE: \n");
        printSymbolTable(inner_scope);
        printf("\n");
      }
      break;

    case LVALUE_ARR_NODE:
      index = current_attribute->node.lea;
      index->scope = current_scope;
      handleExpression(current_attribute->node.lva_arr->ptr2, current_scope);
      break;

    case LVALUE_ID_NODE:
      handleExpression(current_attribute->node.lva_id->ptr1, current_scope);
      break;

    default:
      fprintf(stderr, "Invalid case for handling the attribute of a statement.\n");
      exit(EXIT_FAILURE);
      break;
  }

}


void walkThoughStatements(struct StatementNode *current_statement,
                          struct SymbolTable *current_scope) {
  /* Walk through all statments under the current scope and all
   * nested inner scopes by employing a recursive walk. */
  while (current_statement != NULL) {
    handleStatement(current_statement, current_scope);
    current_statement = current_statement->ptr2;
  }
}


void generateSymbolTableForModule(struct ModuleNode *module) {
  /* Run through the module's body of statements and create and populate
   * a new symbol table for each scope. */
  struct SymbolTable *current_scope = module->ptr1->scope;
  module->ptr1->scope = current_scope;
  assert(current_scope != NULL);
  struct StatementNode *current_statement = module->ptr4;
  walkThoughStatements(current_statement, current_scope);
  if (st_debug_mode) {
    printf("DEBUG MODULE (%s) SCOPE:\n", module->ptr1->value.entry);
    printSymbolTable(current_scope);
    printf("\n");
  }
}


void addModuleEntry(struct ModuleDeclarationNode *module_dec) {
  /* Add a module's information to the symbol table. At this point we can only add the
   * name of the module and the line number of decleration. We will populate the
   * InputPlist and OutputPlist later on. */
  bool exists = false;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  
  strcpy(new_value.module.name, module_dec->ptr1->value.entry);
  new_value.module.dec_line_number = module_dec->ptr1->line_number;
  new_value.module.def_line_number = -1;
  new_value.module.inputplist = NULL;
  new_value.module.outputplist = NULL;
  
  existing_node = symbolTableGet(global_symbol_table, new_value.module.name);
  if (existing_node != NULL)
    exists = true;
  if (exists) {
    fprintf(stderr, "SEMANTIC ERROR: Detected a redecleration of module \"%s\" on line %d. \
Previously declared on line %d.\n", new_value.module.name, new_value.module.dec_line_number,
existing_node->value.module.dec_line_number);
    exit(EXIT_FAILURE);
  }
  symbolTableSet(global_symbol_table, new_value.module.name, new_value, ST_MODULE, false);

  return;
}


void handleInputPlistWithDuplicatedCode(struct InputPlistNode *inpl,
                                        struct SymbolTable *current_scope) {
  bool is_array;
  int line_number;
  char *identifier_name;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  struct Attribute *datatype_node;
  struct ArrayTypeNode *arr_type_node;
  struct DynamicRangeNode *dyn_range_node;
  enum terminal base_datatype;

  while (inpl != NULL) {
    identifier_name = inpl->ptr1->value.entry;
    datatype_node = inpl->ptr2;

    if (datatype_node->type == ARRAY_TYPE_NODE) {
       is_array = true;
       arr_type_node = datatype_node->node.arr_typ;
       base_datatype = arr_type_node->ptr1->type;
       dyn_range_node = arr_type_node->ptr2;
     } else {
       is_array = false;
       arr_type_node = NULL;
       base_datatype = datatype_node->node.lea->type;
       dyn_range_node = NULL;
     }

    existing_node = symbolTableGet(current_scope, identifier_name);
    if (existing_node != NULL) {
      fprintf(stderr, "SEMANTIC ERROR: Detected a redecleration of variable \"%s\" on line %d. \
Previously declared on line %d.\n", identifier_name, line_number,
existing_node->value.variable.line_number);
      exit(EXIT_FAILURE);
    }

    line_number = inpl->ptr1->line_number;
    strcpy(new_value.variable.name, identifier_name);
    new_value.variable.line_number = line_number;
    new_value.variable.value = getDefaultValueForType(base_datatype);  // TODO: Rework this for arrays.
    new_value.variable.datatype = base_datatype;
    new_value.variable.isArray = is_array;
    if (is_array) {
      new_value.variable.lower_bound = getIndexValue(dyn_range_node->ptr1,
        current_scope, line_number); 
      new_value.variable.upper_bound = getIndexValue(dyn_range_node->ptr2,
        current_scope, line_number); 
    } else {
      new_value.variable.lower_bound = 0;
      new_value.variable.upper_bound = 0;
    }
    new_value.variable.mem_offset = NULL; // TODO: Figure out what to do with the memory offset.

    symbolTableSet(current_scope, new_value.variable.name, new_value, ST_VARIABLE, false);

    inpl->ptr1->scope = current_scope;
    inpl = inpl->ptr3;
  }
}


void handleOutputPlistWithDuplicatedCode(struct OutputPlistNode *outpl,
                                         struct SymbolTable *current_scope) {
  bool is_array;
  int line_number;
  char *identifier_name;
  union SymbolTableValue new_value;
  struct SymbolTableNode *existing_node;
  struct Attribute *datatype_node;
  struct DynamicRangeNode *dyn_range_node;
  enum terminal base_datatype;

  while (outpl != NULL) {
    identifier_name = outpl->ptr1->value.entry;
    datatype_node = outpl->ptr2;

    is_array = false;
    base_datatype = datatype_node->node.lea->type;
    dyn_range_node = NULL;

    existing_node = symbolTableGet(current_scope, identifier_name);
    if (existing_node != NULL) {
      fprintf(stderr, "SEMANTIC ERROR: Detected a redecleration of variable \"%s\" on line %d. \
Previously declared on line %d.\n", identifier_name, line_number,
existing_node->value.variable.line_number);
      exit(EXIT_FAILURE);
    }

    line_number = outpl->ptr1->line_number;
    strcpy(new_value.variable.name, identifier_name);
    new_value.variable.line_number = line_number;
    new_value.variable.value = getDefaultValueForType(base_datatype);  // TODO: Rework this for arrays.
    new_value.variable.datatype = base_datatype;
    new_value.variable.isArray = is_array;
    if (is_array) {
      new_value.variable.lower_bound = getIndexValue(dyn_range_node->ptr1,
        current_scope, line_number); 
      new_value.variable.upper_bound = getIndexValue(dyn_range_node->ptr2,
        current_scope, line_number); 
    } else {
      new_value.variable.lower_bound = 0;
      new_value.variable.upper_bound = 0;
    }
    new_value.variable.mem_offset = NULL; // TODO: Figure out what to do with the memory offset.

    symbolTableSet(current_scope, new_value.variable.name, new_value, ST_VARIABLE, false);

    outpl->ptr1->scope = current_scope;
    outpl = outpl->ptr3;
  }
}


void addModuleInpOutLists(struct ModuleNode *module) {
  struct SymbolTable *current_scope = newSymbolTable(global_symbol_table,
    module->ptr1->value.entry, NULL);
  module->ptr1->scope = current_scope;

  /* Brace yourself, more duplicated code up ahead. First between what's
   * below and handleVariablesDeclerations, and then between inpl
   * and outpl. With more time and preferably a language supporting OOP,
   * I would have avoided this duplication. But for now, I have to keep
   * things moving. */

  /* I just moved the atrocities into separate functions */
  handleInputPlistWithDuplicatedCode(module->ptr2, current_scope);
  handleOutputPlistWithDuplicatedCode(module->ptr3, current_scope);
}


void updateModuleEntry(struct ModuleNode *module, bool post_driver) {
  /* If a module was previously declared then an entry would have been created
   * for it in the symbol table already. Now what we need to do is update that
   * existing decleration (the InputPlist and OutputPlist) or raise an error if
   * it doesn't exit.*/
  char *module_name = module->ptr1->value.entry;
  struct SymbolTableNode *existing_node;

  existing_node = symbolTableGet(global_symbol_table, module_name);
  if (existing_node == NULL) {
    if (post_driver) {
      fprintf(stderr, "SEMANTIC ERROR: The module decleration for \"%s\" does not exist yet. However \
a definition was given on line number: %d\n", module_name, module->ptr1->line_number);
      exit(EXIT_FAILURE);
    } else {
      /* Create a new node. Then populate everything at once. Ideally we should
       * avoid duplication of code against addModuleEntry, but due to time
       * constaints, we'll just accept the duplicated code. Or we should at least
       * rename this function to represent that it may also create a new entry. */
      union SymbolTableValue new_value;

      strcpy(new_value.module.name, module->ptr1->value.entry);
      new_value.module.dec_line_number = module->ptr1->line_number;
      new_value.module.def_line_number = module->ptr1->line_number;
      new_value.module.inputplist = module->ptr2;
      new_value.module.outputplist = module->ptr3;

      symbolTableSet(global_symbol_table, new_value.module.name, new_value, ST_MODULE, false);
    }
  }

  else {
    if (existing_node->value.module.def_line_number != -1) {
      printf("SEMANTIC ERROR: The module \"%s\" was already defined at line number %d, and is being \
  redeclared on line number %d.\n", module_name, existing_node->value.module.def_line_number,
  module->ptr1->line_number);
      exit(EXIT_FAILURE);
    }

    existing_node->value.module.def_line_number = module->ptr1->line_number;
    existing_node->value.module.inputplist = module->ptr2;
    existing_node->value.module.outputplist = module->ptr3;

    symbolTableSet(global_symbol_table, module_name, existing_node->value, existing_node->value_type, true);
  }

  addModuleInpOutLists(module);
  generateSymbolTableForModule(module);

  return;
}


void generateSymbolTables() {
  global_symbol_table = newSymbolTable(NULL, "<Global>", NULL);
  struct ModuleDeclarationNode *moduleDec = AST.ptr1;
  
  /* Record each pre-driver module decleration. */
  while (moduleDec != NULL) {
    addModuleEntry(moduleDec);
    moduleDec = moduleDec->ptr2;
  }

  /* Record each non-driver module's information. The module should have
   * already been declared though. */
  struct OtherModuleNode *omodule = AST.ptr2;
  while (omodule != NULL) {
    updateModuleEntry(omodule->ptr1, false);
    omodule = omodule->ptr2;
  }

  /* Handle the driver module. */
  struct SymbolTable *driver_module_scope = newSymbolTable(global_symbol_table,
    "<DriverModule>", NULL);
  if (AST.ptr3 != NULL) {
    walkThoughStatements(AST.ptr3, driver_module_scope);
    if (st_debug_mode) {
      printf("DEBUG DRIVER SCOPE:\n");
      printSymbolTable(driver_module_scope);
      printf("\n");
    }
  }


  omodule = AST.ptr4;
  while (omodule != NULL) {
    updateModuleEntry(omodule->ptr1, true);
    omodule = omodule->ptr2;
  }

  return;
}