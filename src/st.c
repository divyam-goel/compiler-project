#include "st.h"

extern struct ProgramNode AST;
struct SymbolTable *global_symbol_table = NULL;


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
  symbolTableSet(global_symbol_table, new_value.module.name, new_value, ST_MODULE, true);

  return;
}


void updateModuleEntry(struct ModuleNode *module) {
  /* If a module was previously declared then an entry would have been created
   * for it in the symbol table already. Now what we need to do is update that
   * existing decleration (the InputPlist and OutputPlist) or raise an error if
   * it doesn't exit.*/
  char *module_name = module->ptr1->value.entry;
  struct SymbolTableNode *existing_node;

  existing_node = symbolTableGet(global_symbol_table, module_name);
  if (existing_node == NULL) {
    fprintf(stderr, "SEMANTIC ERROR: The module decleration for \"%s\" does not exist yet. However \
a definition was given on line number: %d\n", module_name, module->ptr1->line_number);
    exit(EXIT_FAILURE);
  }

  if (existing_node->value.module.def_line_number != -1) {
    printf("SEMANTIC ERROR: The module \"%s\" was already defined at line number %d, and is being \
redeclared on line number %d.\n", module_name, existing_node->value.module.def_line_number,
module->ptr1->line_number);
  }

  existing_node->value.module.def_line_number = module->ptr1->line_number;
  existing_node->value.module.inputplist = module->ptr2;
  existing_node->value.module.outputplist = module->ptr3;

  symbolTableSet(global_symbol_table, module_name, existing_node->value, existing_node->value_type, true);

  return;
}


void generateSymbolTables() {
  global_symbol_table = newSymbolTable(NULL, "<Global>", NULL);
  struct ModuleDeclarationNode *moduleDec = AST.ptr1;
  
  /* Record each module decleration. */
  while (moduleDec != NULL) {
    addModuleEntry(moduleDec);
    moduleDec = moduleDec->ptr2;
  }

  /* Record each non-driver module's information. The module should have
   * already been declared though*/
  struct OtherModuleNode *omodule = AST.ptr2;
  while (omodule != NULL) {
    updateModuleEntry(omodule->ptr1);
    omodule = omodule->ptr2;
  }
  omodule = AST.ptr4;
  while (omodule != NULL) {
    updateModuleEntry(omodule->ptr1);
    omodule = omodule->ptr2;
  }

  return;
}