#include "ast.h"
#include "st.h"


void semanticCheckerModuleDefinition(struct StatementNode *statement_node);
void semanticCheckerModule(struct ModuleNode *module_node);

void semanticChecker(struct ProgramNode *AST) {
  semanticCheckerModule(AST->ptr2);
  semanticCheckerModuleDefinition(AST->ptr3);
  semanticCheckerModule(AST->ptr4);
}


enum terminal leafType(struct LeafNode *leaf) {
  enum terminal data_type;
  
  switch (leaf->type) {
    case NUM:
      data_type = INTEGER;
      break;
    case RNUM:
      data_type = REAL;
      break;
    case IDENTIFIER:
      struct SymbolTableNode *symbol_table_entry;
      symbol_table_entry = symbolTableGet(leaf->symbol_table,
        leaf->value.entry);
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
  enum terminal lhs_type, rhs_type;

  lhs_type = leafType(assignment_node->ptr1);
    
  struct Attribute *rhs_expression = NULL;
  if (assignment_node->ptr2->type == LVALUE_ID_NODE)
    rhs_expression = assignment_node->ptr2->node.lva_id;
  else
    rhs_expression = assignment_node->ptr2->node.lva_arr;
  rhs_type = expressionTypeChecker(rhs_expression);

  if (lhs_type != rhs_type)
    printf("Type Error: \n");
}


void moduleReuseTypeChecker(struct ModuleReuseStmtNode *module_reuse_node) {
  enum terminal module_reuse_type, module_definition_type;
  
  struct InputPlistNode *input_module_reuse, *input_module_definition;
  struct OutputPlistNode *output_module_reuse, *output_module_definition;

  struct SymbolTableNode *symbol_table_entry;
  symbol_table_entry = symbolTableGet(module_reuse_node->ptr2->symbol_table,
    module_reuse_node->ptr2->value.entry);
  input_module_definition = symbol_table_entry->value.module.inputplist;
  output_module_definition = symbol_table_entry->value.module.outputplist;

  input_module_reuse = module_reuse_node->ptr3;
  output_module_reuse = module_reuse_node->ptr1;  

  while (input_module_definition != NULL && input_module_reuse != NULL) {
    module_definition_type = attributeType(input_module_definition->ptr2);
    module_reuse_type = attributeType(input_module_reuse->ptr2);
    if (module_definition_type != module_reuse_type) {
      printf("Type Error: \n");
    }
    input_module_definition = input_module_definition->ptr3;
    input_module_reuse = input_module_reuse->ptr3;
  }
  if (input_module_definition != NULL || input_module_reuse != NULL) {
    printf("Type Error: \n");
  }

  while (output_module_definition != NULL && output_module_reuse != NULL) {
    module_definition_type = attributeType(output_module_definition->ptr2);
    module_reuse_type = attributeType(output_module_reuse->ptr2);
    if (module_definition_type != module_reuse_type) {
      printf("Type Error: \n");
    }
    output_module_definition = output_module_definition->ptr3;
    output_module_reuse = output_module_reuse->ptr3;
  }
  if (output_module_definition != NULL || output_module_reuse != NULL) {
    printf("Type Error: \n");
  }
}


void semanticCheckerStatement(struct StatementNode *statement_node) {
  switch (statement_node->ptr1->type) {
    case ASSIGN_STMT_NODE:
      assignmentStatementTypeChecker(statement_node->ptr1->node.agn_stm);
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


void semanticCheckerModuleDefinition(struct StatementNode *statement_node) {
  while (statement_node->ptr1->type != NULL_NODE) {
    semanticCheckerStatement(statement_node->ptr1->node.stm);
    statement_node = statement_node->ptr2;
  }
}


void semanticCheckerModule(struct ModuleNode *module_node) {
  semanticCheckerModuleDefinition(module_node->ptr4);
}
