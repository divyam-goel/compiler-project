#include "ast.h"
#include "st.h"
#include "semanticCheck.h"

bool is_input_code_error_free = true;

extern char terminalStringRepresentations[NUM_TERMINALS][16];
extern struct SymbolTable *global_symbol_table;
void statementListSemanticChecker(struct StatementNode *statement_node);
void moduleDefinitionSemanticChecker(struct StatementNode *statement_node);
void modulesSemanticChecker(struct OtherModuleNode *other_module_node);


enum terminal leafType(struct LeafNode *leaf) {
  enum terminal data_type;
  struct SymbolTableNode *symbol_table_entry = NULL;

  switch (leaf->type) {
    case NUM:
    case INTEGER:
      data_type = INTEGER;
      break;
    case RNUM:
    case REAL:
      data_type = REAL;
      break;
    case IDENTIFIER:
      symbol_table_entry = symbolTableGet(leaf->scope, leaf->value.entry);
      if (symbol_table_entry == NULL) {
        printf("Type Error: Undeclared variable %s\n", (char *)leaf->value.entry);
        data_type = EPSILON;
        is_input_code_error_free = false;
      }
      else if (symbol_table_entry->value.variable.isArray) {
        data_type = ARRAY;
      }
      else {
        data_type = symbol_table_entry->value.variable.datatype;
      }
      break;
    case TRUE_:
    case FALSE_:
    case BOOLEAN_:
      data_type = BOOLEAN_;
      break;
    default:
      printf("Invalid Leaf Type: %d\n", leaf->type);
      data_type = EPSILON;
      break;
  }

  return data_type;
}


enum terminal arrayType(struct ArrayNode *array) {
  enum terminal data_type_var, data_type_idx;
  struct SymbolTableNode *symbol_table_entry = NULL;

  symbol_table_entry = symbolTableGet(array->ptr1->scope, array->ptr1->value.entry);
  if (symbol_table_entry == NULL) {
    printf("Type Error: Undeclared variable %s\n", (char *)array->ptr1->value.entry);
    is_input_code_error_free = false;
    return EPSILON;
  }

  data_type_var = symbol_table_entry->value.variable.datatype;
  data_type_idx = leafType(array->ptr2);

  if (data_type_idx != INTEGER) {
    printf("Type Error: Array index should be of type INTEGER, found %s\n",
    terminalStringRepresentations[data_type_idx]);
    is_input_code_error_free = false;
  }

  if (symbol_table_entry->value.variable.isStatic && array->ptr2->type == NUM) {
    int lower_bound, upper_bound, index;
    lower_bound = symbol_table_entry->value.variable.lower_bound->value.num;
    upper_bound = symbol_table_entry->value.variable.upper_bound->value.num;
    index = array->ptr2->value.num;
    if (index < lower_bound || index > upper_bound) {
      printf("Semantic Error: Array out of bounds\n");
      is_input_code_error_free = false;
    }
  }

  return data_type_var;
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


enum terminal logicalExpressionType(struct N7Node *logical_expression) {
  enum terminal left_operand_type, right_operand_type;
  left_operand_type = expressionType(logical_expression->ptr1);
  right_operand_type = expressionType(logical_expression->ptr2);
  
  if (left_operand_type != BOOLEAN_) {
    printf("Type Error: Operator %s expects type BOOLEAN, found %s\n",
      terminalStringRepresentations[logical_expression->logicalOp],
      terminalStringRepresentations[left_operand_type]); // TO DO: print line number
    is_input_code_error_free = false;
    return EPSILON;
  }
  if (right_operand_type != BOOLEAN_) {
    printf("Type Error: Operator %s expects type BOOLEAN, found %s\n",
      terminalStringRepresentations[logical_expression->logicalOp],
      terminalStringRepresentations[right_operand_type]); // TO DO: print line number
    is_input_code_error_free = false;
    return EPSILON;
  }
  
  return BOOLEAN_;
}


enum terminal relationalExpressionType(struct N8Node *relational_expression) {
  enum terminal left_operand_type, right_operand_type;
  left_operand_type = expressionType(relational_expression->ptr1);
  right_operand_type = expressionType(relational_expression->ptr2);
  
  if (left_operand_type != INTEGER && left_operand_type != REAL) {
    printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
      terminalStringRepresentations[relational_expression->relationalOp],
      terminalStringRepresentations[left_operand_type]); // TO DO: print line number
    is_input_code_error_free = false;
    return EPSILON;
  }
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
      terminalStringRepresentations[relational_expression->relationalOp],
      terminalStringRepresentations[right_operand_type]); // TO DO: print line number
    is_input_code_error_free = false;
    return EPSILON;
  }
  
  return BOOLEAN_;
}


enum terminal ArithmeticExpressionType(struct ArithmeticExprNode *arithmetic_expression) {
  enum terminal left_operand_type, right_operand_type;
  
  right_operand_type = expressionType(arithmetic_expression->ptr2);
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
      terminalStringRepresentations[arithmetic_expression->op],
      terminalStringRepresentations[right_operand_type]); // TO DO: print line number
    is_input_code_error_free = false;
    return EPSILON;
  }
  
  if (arithmetic_expression->is_first) {
    left_operand_type = expressionType(arithmetic_expression->ptr1);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
        terminalStringRepresentations[arithmetic_expression->op],
        terminalStringRepresentations[left_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      printf("Type Error: Mismatching operand types while evaluating expression, found %s and %s\n",
        terminalStringRepresentations[left_operand_type],
        terminalStringRepresentations[right_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
  }
  
  if (arithmetic_expression->ptr3->type != NULL_NODE) {
    left_operand_type = expressionType(arithmetic_expression->ptr3);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
        terminalStringRepresentations[arithmetic_expression->op],
        terminalStringRepresentations[left_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      printf("Type Error: Mismatching operand types while evaluating expression, found %s and %s\n",
        terminalStringRepresentations[left_operand_type],
        terminalStringRepresentations[right_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
  }

  arithmetic_expression->data_type = right_operand_type;
  return right_operand_type;
}


enum terminal TermExpressionType(struct TermNode *term_expression) {
  enum terminal left_operand_type, right_operand_type;

  right_operand_type = expressionType(term_expression->ptr2);
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
      terminalStringRepresentations[term_expression->op],
      terminalStringRepresentations[right_operand_type]); // TO DO: print line number
    is_input_code_error_free = false;
    return EPSILON;
  }

  if (term_expression->is_first) {
    left_operand_type = expressionType(term_expression->ptr1);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
        terminalStringRepresentations[term_expression->op],
        terminalStringRepresentations[left_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      printf("Type Error: Mismatching operand types while evaluating expression, found %s and %s\n",
        terminalStringRepresentations[left_operand_type],
        terminalStringRepresentations[right_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
  }
  
  if (term_expression->ptr3->type != NULL_NODE) {
    left_operand_type = expressionType(term_expression->ptr3);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      printf("Type Error: Operator %s expects type INTEGER or REAL, found %s\n",
        terminalStringRepresentations[term_expression->op],
        terminalStringRepresentations[left_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      printf("Type Error: Mismatching operand types while evaluating expression, found %s and %s\n",
        terminalStringRepresentations[left_operand_type],
        terminalStringRepresentations[right_operand_type]); // TO DO: print line number
      is_input_code_error_free = false;
      return EPSILON;
    }
  }

  term_expression->data_type = right_operand_type;
  return right_operand_type;
}


enum terminal expressionType(struct Attribute *expression) {
  switch (expression->type) {
    case U_NODE:
      expression->node.u->data_type = expressionType(expression->node.u->ptr1);
      return expression->node.u->data_type;
      break;
    case N7_NODE:
      return logicalExpressionType(expression->node.n7);
      break;
    case N8_NODE:
      return relationalExpressionType(expression->node.n8);
      break;
    case ARITHMETIC_EXPR_NODE:
      return ArithmeticExpressionType(expression->node.ari_exp);
      break;
    case TERM_NODE:
      return TermExpressionType(expression->node.ter);
      break;
    case ARRAY_NODE:
      return arrayType(expression->node.arr);
      break;
    case LEAF_NODE:
      return leafType(expression->node.lea);
      break;
    default:
      printf("Invalid Expression Type %d!\n", expression->type);
      return EPSILON;
      break;
  }
}


void assignmentTypeChecker(struct AssignStmtNode *assignment_node) {  
  enum terminal lhs_type, rhs_type;

  struct SymbolTableNode *symbol_table_node;
  struct VariableEntry *lhs_symbol_table_entry, *rhs_symbol_table_entry;

  lhs_type = leafType(assignment_node->ptr1);
    
  struct Attribute *rhs_expression = NULL;
  if (assignment_node->ptr2->type == LVALUE_ID_NODE) {
    rhs_expression = assignment_node->ptr2->node.lva_id->ptr1;}
  else {
    if (lhs_type != EPSILON) {
      symbol_table_node = symbolTableGet(assignment_node->ptr1->scope, assignment_node->ptr1->value.entry);
      lhs_symbol_table_entry = &symbol_table_node->value.variable;
      lhs_type = lhs_symbol_table_entry->datatype;
    }
    rhs_expression = assignment_node->ptr2->node.lva_arr->ptr2;}
  rhs_type = expressionType(rhs_expression);

  if (lhs_type == rhs_type && rhs_type == ARRAY) {
    symbol_table_node = symbolTableGet(assignment_node->ptr1->scope, assignment_node->ptr1->value.entry);
    lhs_symbol_table_entry = &symbol_table_node->value.variable;

    symbol_table_node = symbolTableGet(assignment_node->ptr2->node.lea->scope,
                                      assignment_node->ptr2->node.lva_id->ptr1->node.lea->value.entry);
    rhs_symbol_table_entry = &symbol_table_node->value.variable;

    if (lhs_symbol_table_entry->lower_bound == rhs_symbol_table_entry->lower_bound &&
        lhs_symbol_table_entry->upper_bound == rhs_symbol_table_entry->upper_bound &&
        lhs_symbol_table_entry->datatype == rhs_symbol_table_entry->datatype) {
          return;
    }

    printf("Type Error: Mismatching types for array assignment, found %s and %s\n",
      terminalStringRepresentations[lhs_symbol_table_entry->datatype],
      terminalStringRepresentations[rhs_symbol_table_entry->datatype]);
    
    is_input_code_error_free = false;
  }

  if (lhs_type != EPSILON && rhs_type != EPSILON && lhs_type != rhs_type) {
    printf("Type Error: expression evaluates to type %s, expected %s\n",
      terminalStringRepresentations[rhs_type], terminalStringRepresentations[lhs_type]);
    is_input_code_error_free = false;
  }
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
      printf("Type Error: 5\n");
      is_input_code_error_free = false;
    }
    input_module_definition = input_module_definition->ptr3;
    input_module_reuse = input_module_reuse->ptr2;
  }
  if (input_module_definition != NULL || input_module_reuse != NULL) {
    printf("Type Error: 6\n");
    is_input_code_error_free = false;
  }

  // iterate over input parameter and identifier list from module definition and reuse
  // respectively, to compare corresponding data types
  while (output_module_definition != NULL && output_module_reuse != NULL) {
    module_definition_type = attributeType(output_module_definition->ptr2);
    module_reuse_type = leafType(output_module_reuse->ptr1);
    if (module_definition_type != module_reuse_type) {
      printf("Type Error: 7\n");
      is_input_code_error_free = false;
    }
    output_module_definition = output_module_definition->ptr3;
    output_module_reuse = output_module_reuse->ptr2;
  }
  if (output_module_definition != NULL || output_module_reuse != NULL) {
    printf("Type Error: 8\n");
    is_input_code_error_free = false;
  }
}


void conditionalSemanticChecker(struct ConditionalStmtNode *conditional_node) {
  enum terminal switch_type, case_type;
  struct CaseStmtNode *case_node;
  bool is_exists_true, is_exists_false;
  
  switch_type = leafType(conditional_node->ptr1);
  
  switch (switch_type) {
    case REAL:
      printf("Type Error: Conditional switch over type REAL not allowed\n");
      is_input_code_error_free = false;
      case_node = conditional_node->ptr2;
      while (case_node != NULL) {
        statementListSemanticChecker(case_node->ptr2);
        case_node = case_node->ptr3;
      }
      statementListSemanticChecker(conditional_node->ptr3);
      break;
    
    case INTEGER:
      case_node = conditional_node->ptr2;
      /* check: cases should only contain integer numbers */
      while (case_node != NULL) {
        case_type = leafType(case_node->ptr1);
        if (switch_type != case_type) {
          printf("Type Error: Conditional switch over type INTEGER can take only INTEGER case values, type %s found",
            terminalStringRepresentations[case_type]);
          is_input_code_error_free = false;
        }
        statementListSemanticChecker(case_node->ptr2);
        case_node = case_node->ptr3;
      }
      /* check: conditional statements over integer variables should have a default case */
      if (conditional_node->ptr3 == NULL) {
        printf("Semantic Error: Conditional switch over type INTEGER should have a default case\n");
      }
      break;
    
    case BOOLEAN_:
      /* check: conditional statements over bool variables should have case statements for
      both true and false bool values*/
      is_exists_true = is_exists_false = false;
      case_node = conditional_node->ptr2;
      while (case_node != NULL) {
        switch (case_node->ptr1->type) {
          case TRUE_:
            if (is_exists_true)
              printf("Semantic Error: Multiple declarations of Case TRUE\n");
            else
              is_exists_true = true;
            break;
          case FALSE_:
            if (is_exists_false)
              printf("Semantic Error: Multiple declarations of Case FALSE\n");
            else
              is_exists_false = true;
            break;
          default:
            case_type = leafType(case_node->ptr1);
            printf("Type Error: Conditional switch over type BOOLEAN can take only BOOLEAN case values, type %s found\n",
              terminalStringRepresentations[case_type]);
            break;
        }
        statementListSemanticChecker(case_node->ptr2);
        case_node = case_node->ptr3;
      }
      if (!is_exists_true)
        printf("Semantic Error: Conditional switch over type BOOLEAN must have a case for TRUE\n");
      if (!is_exists_false)
        printf("Semantic Error: Conditional switch over type BOOLEAN must have a case for FALSE\n");
      /* check: conditional statements over bool variables should not have a default case */
      if (conditional_node->ptr3 != NULL) {
        printf("Semantic Error: Conditional switch over type BOOLEAN should not have a default case\n");
      }
      break;  
    
    default:
      printf("Invalid type for switch variable: %d\n", switch_type);
      break;
  }
}


void forIterationSemanticChecker(struct ForIterativeStmtNode *for_iter_node) {
  enum terminal loop_var_type;

  loop_var_type = leafType(for_iter_node->ptr1);
  if (loop_var_type != INTEGER)
    printf("Type Error: Expected type INTEGER for loop variable of for-loop, found %s\n",
      terminalStringRepresentations[loop_var_type]);
  
  statementListSemanticChecker(for_iter_node->ptr3);
}


void whileIterationSemanticChecker(struct WhileIterativeStmtNode *while_iter_node) {
  enum terminal loop_expression_type;

  loop_expression_type = expressionType(while_iter_node->ptr1);
  if (loop_expression_type != BOOLEAN_ && loop_expression_type != EPSILON) {
    printf("Type Error: Excepted type BOOLEAN for loop expression of while loop, found %s\n",
      terminalStringRepresentations[loop_expression_type]);
  }

  // check to see if one of the variables in above expression is in LHS of one of the statements
  // in the while block
  // struct StatementNode *statement_node = while_iter_node->ptr2;
  // while (statement_node != NULL) {
  //   if (statement_node->ptr1->type == ASSIGN_STMT_NODE) {
  //     statement_node->ptr1->node.agn_stm->ptr1->value.entry;
  //   }
  //   statement_node = statement_node->ptr2;
  // }

  statementListSemanticChecker(while_iter_node->ptr2);
}


void statementSemanticChecker(struct StatementNode *statement_node) {
  if (statement_node == NULL)
    return;
  
  // printStatement(statement_node);
  switch (statement_node->ptr1->type) {
    case ASSIGN_STMT_NODE:
      assignmentTypeChecker(statement_node->ptr1->node.agn_stm);
      printf("\n");
      break;
    case MODULE_REUSE_STMT_NODE:
      moduleReuseTypeChecker(statement_node->ptr1->node.mod_reu_stm);
      printf("\n");
      break;
    case CONDITIONAL_STMT_NODE:
      conditionalSemanticChecker(statement_node->ptr1->node.con_stm);
      break;
    case FOR_ITERATIVE_STMT_NODE:
      forIterationSemanticChecker(statement_node->ptr1->node.for_ite_stm);
      break;
    case WHILE_ITERATIVE_STMT_NODE:
      whileIterationSemanticChecker(statement_node->ptr1->node.whi_ite_stm);
      break;
    case INPUT_NODE:
    case PRINT_NODE:
    case DECLARE_STMT_NODE:
      break;
    default:
      printf("Invalid Type for Statement Node %d\n", statement_node->ptr1->type);
      break;
  }
}


void statementListSemanticChecker(struct StatementNode *statement_node) {
  while (statement_node != NULL) {
    statementSemanticChecker(statement_node);
    statement_node = statement_node->ptr2;
  }
}


void moduleDefinitionSemanticChecker(struct StatementNode *statement_node) {
  statementListSemanticChecker(statement_node);
}


void modulesSemanticChecker(struct OtherModuleNode *other_module_node) {
  while(other_module_node != NULL) {
    moduleDefinitionSemanticChecker(other_module_node->ptr1->ptr4);
    other_module_node = other_module_node->ptr2;
  }
}


void semanticChecker(struct ProgramNode *AST) {
  modulesSemanticChecker(AST->ptr2);
  moduleDefinitionSemanticChecker(AST->ptr3);
  modulesSemanticChecker(AST->ptr4);
}
