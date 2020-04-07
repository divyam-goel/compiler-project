#include "ast.h"
#include "st.h"
#include "semanticCheck.h"


extern struct SymbolTable *global_symbol_table;
void moduleDefinitionSemanticChecker(struct StatementNode *statement_node);
void modulesSemanticChecker(struct OtherModuleNode *other_module_node);


void semanticChecker(struct ProgramNode *AST) {
  modulesSemanticChecker(AST->ptr2);
  moduleDefinitionSemanticChecker(AST->ptr3);
  modulesSemanticChecker(AST->ptr4);
}


enum terminal leafType(struct LeafNode *leaf) {
  enum terminal data_type;
  struct SymbolTableNode *symbol_table_entry = NULL;
  
  switch (leaf->type) {
    case NUM:
      data_type = INTEGER;
      break;
    case RNUM:
      data_type = REAL;
      break;
    case IDENTIFIER:
      symbol_table_entry = symbolTableGet(leaf->scope, leaf->value.entry);
      printf("DEBUG: Checkpoint 1!\n");
      data_type = symbol_table_entry->value.variable.datatype;
      break;
    case TRUE_:
    case FALSE_:
      data_type = BOOLEAN_;
      break;
    default:
      printf("Invalid Leaf Type: %d\n", leaf->type);
      break;
  }

  return data_type;
}


enum terminal attributeType(struct Attribute *attribute) {
  switch (attribute->type) {
    case ARRAY_TYPE_NODE:
      return leafType(attribute->node.arr->ptr1);
    case LEAF_NODE:
      return leafType(attribute->node.lea);
      break;
    default:
      printf("Attribute Type %d doesn't have a data type", attribute->type);
      return EPSILON;
  }
}


enum terminal expressionType(struct Attribute *expression) {  
  printf("DEBUG: Checkpoint 1!\n");
  enum terminal left_operand_type, right_operand_type;
  
  switch (expression->type) {
    case U_NODE:
      return expressionType(expression->node.u->ptr1);
      break;
    case N7_NODE:
      left_operand_type = expressionType(expression->node.n7->ptr1);
      right_operand_type = expressionType(expression->node.n7->ptr2);
      break;
    case N8_NODE:
      left_operand_type = expressionType(expression->node.n8->ptr1);
      right_operand_type = expressionType(expression->node.n8->ptr2);
      break;
    case ARITHMETIC_EXPR_NODE:
      right_operand_type = expressionType(expression->node.ari_exp->ptr2);
      if (expression->node.ari_exp->ptr1->type == LEAF_NODE) {
        left_operand_type = expressionType(expression->node.ari_exp->ptr1);
        if (left_operand_type != right_operand_type) {
          printf("Type Error: \n"); // TO DO: print line number
          return EPSILON;
        }
      left_operand_type = expressionType(expression->node.ari_exp->ptr3);
      }
      break;
    case TERM_NODE:
      right_operand_type = expressionType(expression->node.ter->ptr2);
      if (expression->node.ter->ptr1->type == LEAF_NODE) {
        left_operand_type = expressionType(expression->node.ter->ptr1);
        if (left_operand_type != right_operand_type) {
          printf("Type Error: \n"); // TO DO: print line number
          return EPSILON;
        }
      left_operand_type = expressionType(expression->node.ter->ptr3);
      }
      break;
    case ARRAY_NODE:
      return leafType(expression->node.arr->ptr1);
      break;
    case LEAF_NODE:
      return leafType(expression->node.lea);
      break;
    default:
      printf("Invalid Expression Type %d! ", expression->type);
      break;
  }

  if (left_operand_type == EPSILON || right_operand_type == EPSILON)
    return EPSILON;

  if (left_operand_type != right_operand_type) {
    printf("Type Error: \n"); // TO DO: print line number
    return EPSILON;
  }

  return left_operand_type;
}


void assignmentTypeChecker(struct AssignStmtNode *assignment_node) {
  if (assignment_node == NULL) {
    // printf("DEBUG: Don't know what's happening.\n");
    return;
  }
  
  enum terminal lhs_type, rhs_type;

  lhs_type = leafType(assignment_node->ptr1);
    
  struct Attribute *rhs_expression = NULL;
  if (assignment_node->ptr2->type == LVALUE_ID_NODE)
    rhs_expression = assignment_node->ptr2->node.lva_id->ptr1;
  else
    rhs_expression = assignment_node->ptr2->node.lva_arr->ptr2;
  rhs_type = expressionType(rhs_expression);

  if (lhs_type != rhs_type)
    printf("Type Error: \n");
}


void moduleReuseTypeChecker(struct ModuleReuseStmtNode *module_reuse_node) {
  if (module_reuse_node == NULL) {
    return;
  }
  
  enum terminal module_reuse_type, module_definition_type;
  
  struct InputPlistNode *input_module_definition;
  struct OutputPlistNode *output_module_definition;
  struct IdListNode *input_module_reuse, *output_module_reuse;

  // get the first entry for input and output parameter list from module definition
  struct SymbolTableNode *symbol_table_entry;
  symbol_table_entry = symbolTableGet(global_symbol_table,
    module_reuse_node->ptr2->value.entry);
  input_module_definition = symbol_table_entry->value.module.inputplist;
  output_module_definition = symbol_table_entry->value.module.outputplist;

  // get the first entry for input and output identifier list from module reuse
  input_module_reuse = module_reuse_node->ptr3;
  output_module_reuse = module_reuse_node->ptr1;  

  // iterate over input parameter and identifier list from module definition and reuse
  // respectively, to compare corresponding data types
  while (input_module_definition != NULL && input_module_reuse != NULL) {
    module_definition_type = attributeType(input_module_definition->ptr2);
    module_reuse_type = leafType(input_module_reuse->ptr1);
    if (module_definition_type != module_reuse_type) {
      printf("Type Error: \n");
    }
    input_module_definition = input_module_definition->ptr3;
    input_module_reuse = input_module_reuse->ptr2;
  }
  if (input_module_definition != NULL || input_module_reuse != NULL) {
    printf("Type Error: \n");
  }

  // iterate over input parameter and identifier list from module definition and reuse
  // respectively, to compare corresponding data types
  while (output_module_definition != NULL && output_module_reuse != NULL) {
    module_definition_type = attributeType(output_module_definition->ptr2);
    module_reuse_type = leafType(output_module_reuse->ptr1);
    if (module_definition_type != module_reuse_type) {
      printf("Type Error: \n");
    }
    output_module_definition = output_module_definition->ptr3;
    output_module_reuse = output_module_reuse->ptr2;
  }
  if (output_module_definition != NULL || output_module_reuse != NULL) {
    printf("Type Error: \n");
  }
}


void statementSemanticChecker(struct StatementNode *statement_node) {
  if (statement_node == NULL)
    return;
  
  struct AssignStmtNode *debug_assignment_node;
  printf("DEBUG: Statement Type - %d\n", statement_node->ptr1->type);
  
  switch (statement_node->ptr1->type) {
    case ASSIGN_STMT_NODE:
      debug_assignment_node = statement_node->ptr1->node.agn_stm;
      if (debug_assignment_node != NULL)
        printf("DEBUG: Don't know what's happening.\n");
      assignmentTypeChecker(debug_assignment_node);
      break;
    case MODULE_REUSE_STMT_NODE:
      moduleReuseTypeChecker(statement_node->ptr1->node.mod_reu_stm);
      break;
    case DECLARE_STMT_NODE:
      printf("TO DO:\n");
      break;
    case PRINT_NODE:
      printf("TO DO:\n");
      break;
    case INPUT_NODE:
      printf("TO DO:\n");
      break;
    case CONDITIONAL_STMT_NODE:
      printf("TO DO:\n");
      break;
    case FOR_ITERATIVE_STMT_NODE:
      printf("TO DO:\n");
      break;
    case WHILE_ITERATIVE_STMT_NODE:
      printf("TO DO:\n");
      break;
    default:
      printf("Invalid Type for Statement Node %d\n", statement_node->ptr1->type);
      break;
  }
}


void moduleDefinitionSemanticChecker(struct StatementNode *statement_node) {
  if (statement_node == NULL)
    return;

  while (statement_node->ptr1->type != NULL_NODE) {
    statementSemanticChecker(statement_node);
    statement_node = statement_node->ptr2;
  }
}


void modulesSemanticChecker(struct OtherModuleNode *other_module_node) {
  while(other_module_node != NULL) {
    moduleDefinitionSemanticChecker(other_module_node->ptr1->ptr4);
    other_module_node = other_module_node->ptr2;
  }
}
