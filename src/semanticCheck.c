#include "ast.h"
#include "st.h"
#include "semanticCheck.h"


extern char terminalStringRepresentations[NUM_TERMINALS][16];
extern char terminalLiteralRepresentations[NUM_TERMINALS][16];
extern struct ProgramNode AST;
extern struct SymbolTable *global_symbol_table;

/* error messages: array */
char *array_incorrect_index_type_error_message = "Line %d: (Type Error) Array index should be of type INTEGER, \
found %s\n";
char *array_index_out_of_bounds_error_message = "Line %d: (Semantic Error) Array index is out of bounds, should \
be in the range %d to %d\n";

/* error messages: expressions */
char *operator_incorrect_type_error_message = "Line %d: (Type Error) Operator '%s' expects type %s, found %s\n";
char *operator_incompatible_types_error_message = "Line %d: (Type Error) Operator '%s' received incompatible \
operand types, found %s and %s\n";
char *operator_incompatible_array_types_error_message = "Line %d: (Type Error) Operator '%s' received incompatible \
operand types, found array[%d...%d] of %s and array[%d...%d] of %s\n";

/* error messages: module reuse */
char *module_incorrect_input_type = "Line %d: (Type Error) Module reuse expects type %s in input list, found %s\n";
char *module_incorrect_input_array_type = "Line %d: (Type Error) Module reuse expects array type array[%d...%d] of %s \
in input list, found array[%d...%d] of %s\n";
char *module_input_list_missing_args = "Line %d: (Semantic Error) Insufficient number of arguments were provided \
in input list for module reuse";
char *module_input_list_too_many_args = "Line %d: (Semantic Error) Too many arguments were provided in input list \
for module reuse";
char *module_incorrect_output_type = "Line %d: (Type Error) Module reuse expects type %s in output list, found %s\n";
char *module_output_list_missing_args = "Line %d: (Semantic Error) Insufficient number of arguments were provided \
in output list for module reuse";
char *module_output_list_too_many_args = "Line %d: (Semantic Error) Too many arguments were provided in output list \
for module reuse";

/* error messages: conditionals */
char *conditional_incorrect_switch_type_error_message = "Line %d: (Type Error) Construct 'switch' expects type \
INTEGER or BOOLEAN, found %s\n";
char *conditional_incorrect_case_type_error_message = "Line %d: (Type Error) Construct 'switch' is over type %s. \
So, 'case' expects type %s for values, found %s";
char *integer_conditional_missing_default_error_message = "Line %d: (Semantic Error) Construct 'switch' is over \
type INTEGER. So, it MUST have a default case\n";
char *boolean_conditional_missing_case_error_message = "Line %d: (Semantic Error) Construct 'switch' is over type \
BOOLEAN. So, it MUST have a case for value %s\n";
char *boolean_conditional_contains_default_error_message = "Line %d: (Semantic Error) Construct 'switch' is over \
type BOOLEAN. So, it can NOT have a default case\n";

/* error messages: for iteration */
char *for_incorrect_variable_type_error_message = "Line %d: (Type Error) Construct 'for' expects type INTEGER for \
loop variable, found %s\n";
char *for_incorrect_bounds_error_message = "Line %d: (Semantic Error) Construct 'for' expects lower bound to be less \
than / equal to the upper bound\n";

/* error messages: while iteration */
char *while_incorrect_expression_type_error_message = "Line %d: (Type Error) Construct 'while' excepts type BOOLEAN \
for loop expression, found %s\n";

/* utility strings */
char *integer_str = "INTEGER";
char *real_str = "REAL";
char *integer_or_real_str = "INTEGER or REAL";
char *boolean_str = "BOOLEAN";
char *array_str = "ARRAY";
char *true_str = "TRUE";
char *false_str = "FALSE";

/* global variables */
bool st_debug_mode = false;
int semantic_error_count = 0;


enum terminal leafType(struct LeafNode *leaf) {
  enum terminal data_type;
  struct SymbolTableNode *symbol_table_entry = NULL;

  switch (leaf->type) {

    case NUM:
    case INTEGER:
      return INTEGER;
      break;
    
    case RNUM:
    case REAL:
      return REAL;
      break;
    
    case IDENTIFIER:
      symbol_table_entry = symbolTableGet(leaf->scope, leaf->value.entry);
      if (symbol_table_entry == NULL || symbol_table_entry->value_type == ST_MODULE)
        return EPSILON;
      else if (symbol_table_entry->value.variable.isArray)
        return ARRAY;
      else
        return symbol_table_entry->value.variable.datatype;
      break;
    
    case TRUE_:
    case FALSE_:
    case BOOLEAN_:
      return BOOLEAN_;
      break;
    
    default:
      printf("ERROR!! Received invalid type for leaf: %d\n", leaf->type);
      return EPSILON;
      break;
  }
}


enum terminal arrayType(struct ArrayNode *array) {
  struct SymbolTableNode *symbol_table_entry;
  enum terminal array_type, index_type;
  int index, lower_bound, upper_bound;
  int line_number;

  symbol_table_entry = symbolTableGet(array->ptr1->scope, array->ptr1->value.entry);
  if (symbol_table_entry == NULL || symbol_table_entry->value_type == ST_MODULE)
    return EPSILON;
  
  /* determine array type, index type and line number of array use */
  array_type = symbol_table_entry->value.variable.datatype;
  index_type = leafType(array->ptr2);
  line_number = array->ptr1->line_number;

  /* check: data type of array index should be integer */
  if (index_type != INTEGER) {
    fprintf(stderr, array_incorrect_index_type_error_message,
            line_number, terminalStringRepresentations[index_type]);
    semantic_error_count += 1;
    return array_type;
  }

  /*
    (if array is static and has a non-identifier index)
    check: array index should be greater than / equal to lower bound &&
    should be less than / equal to upper bound
  */
  if (symbol_table_entry->value.variable.isStatic && array->ptr2->type == NUM) {
    lower_bound = symbol_table_entry->value.variable.lower_bound->value.num;
    upper_bound = symbol_table_entry->value.variable.upper_bound->value.num;
    index = array->ptr2->value.num;
    if (index < lower_bound || index > upper_bound) {
      fprintf(stderr, array_index_out_of_bounds_error_message, line_number);
      semantic_error_count += 1;
    }
  }

  return array_type;
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


void operatorIncorrectTypeError(
  int line_number, enum terminal op,
  char *expected_type, enum terminal received_type) {
  
  fprintf(stderr, operator_incorrect_type_error_message,
          line_number,
          terminalLiteralRepresentations[op],
          expected_type,
          terminalStringRepresentations[received_type]);
  
  semantic_error_count += 1;
}


void operatorIncompatibleTypesError(
  int line_number, enum terminal op,
  enum terminal left_type, enum terminal right_type) {
  
  fprintf(stderr, operator_incorrect_type_error_message,
          line_number,
          terminalLiteralRepresentations[op],
          terminalLiteralRepresentations[left_type],
          terminalStringRepresentations[right_type]);
  
  semantic_error_count += 1;
}


enum terminal logicalExpressionType(struct N7Node *logical_expression) {
  enum terminal left_operand_type, right_operand_type;
  int line_number;
  
  left_operand_type = expressionType(logical_expression->ptr1);
  right_operand_type = expressionType(logical_expression->ptr2);
  
  line_number = logical_expression->line_number;
  
  if (left_operand_type != BOOLEAN_) {
    operatorIncorrectTypeError(
      line_number, logical_expression->logicalOp,
      boolean_str, left_operand_type);
    return EPSILON;
  }
  
  if (right_operand_type != BOOLEAN_) {
    operatorIncorrectTypeError(
      line_number, logical_expression->logicalOp,
      boolean_str, right_operand_type);
    return EPSILON;
  }
  
  return BOOLEAN_;
}


enum terminal relationalExpressionType(struct N8Node *relational_expression) {
  enum terminal left_operand_type, right_operand_type;
  int line_number;
  
  left_operand_type = expressionType(relational_expression->ptr1);
  right_operand_type = expressionType(relational_expression->ptr2);

  line_number = relational_expression->line_number;
  
  if (left_operand_type != INTEGER && left_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, relational_expression->relationalOp,
      integer_or_real_str, left_operand_type);
    return EPSILON;
  }
  
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, relational_expression->relationalOp,
      integer_or_real_str, right_operand_type);
    return EPSILON;
  }
  
  return BOOLEAN_;
}


enum terminal arithmeticExpressionType(struct ArithmeticExprNode *arithmetic_expression) {
  enum terminal left_operand_type, right_operand_type;
  int line_number;

  line_number = arithmetic_expression->line_number;
  
  right_operand_type = expressionType(arithmetic_expression->ptr2);
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, arithmetic_expression->op,
      integer_or_real_str, left_operand_type);
    return EPSILON;
  }
  
  if (arithmetic_expression->is_first) {
    left_operand_type = expressionType(arithmetic_expression->ptr1);
    
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, arithmetic_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }

    if (left_operand_type != right_operand_type) {
      operatorIncompatibleTypesError(
        line_number, arithmetic_expression->op,
        left_operand_type, right_operand_type);    
      return EPSILON;
    }
  }
  
  if (arithmetic_expression->ptr3->type != NULL_NODE) {
    left_operand_type = expressionType(arithmetic_expression->ptr3);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, arithmetic_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      operatorIncompatibleTypesError(
        line_number, arithmetic_expression->op,
        left_operand_type, right_operand_type);    
      return EPSILON;
    }
  }

  arithmetic_expression->data_type = right_operand_type;
  return right_operand_type;
}


enum terminal termExpressionType(struct TermNode *term_expression) {
  enum terminal left_operand_type, right_operand_type;
  int line_number;

  line_number = term_expression->line_number;

  right_operand_type = expressionType(term_expression->ptr2);
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, term_expression->op,
      integer_or_real_str, right_operand_type);
    return EPSILON;
  }

  if (term_expression->is_first) {
    left_operand_type = expressionType(term_expression->ptr1);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, term_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      operatorIncompatibleTypesError(
        line_number, term_expression->op,
        left_operand_type, right_operand_type);    
      return EPSILON;
    }
  }
  
  if (term_expression->ptr3->type != NULL_NODE) {
    left_operand_type = expressionType(term_expression->ptr3);
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, term_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }
    if (left_operand_type != right_operand_type) {
      operatorIncompatibleTypesError(
        line_number, term_expression->op,
        left_operand_type, right_operand_type);    
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
      return arithmeticExpressionType(expression->node.ari_exp);
      break;
    case TERM_NODE:
      return termExpressionType(expression->node.ter);
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


void arrayAssignemntTypeCheck(struct LeafNode *lhs_array, struct LeafNode *rhs_array) {
  struct SymbolTableNode *symbol_table_node;
  struct VariableEntry *st_entry_lhs, *st_entry_rhs;
  enum terminal data_type_lhs, data_type_rhs;
  int lower_bound_lhs, lower_bound_rhs;
  int upper_bound_lhs, upper_bound_rhs;
  int line_number;
  
  symbol_table_node = symbolTableGet(lhs_array->scope, lhs_array->value.entry);
  if (symbol_table_node == NULL)
    return;
  st_entry_lhs = &symbol_table_node->value.variable;

  symbol_table_node = symbolTableGet(rhs_array->scope, rhs_array->value.entry);
  if (symbol_table_node == NULL)
    return;
  st_entry_rhs = &symbol_table_node->value.variable;

  /* proceed further with semantic checks only if both arrays are static */
  if (!(st_entry_lhs->isStatic) || !(st_entry_rhs->isStatic))
    return;

  /* determine lower & upper bound, and data type of LHS */
  data_type_lhs = st_entry_lhs->datatype;
  lower_bound_lhs = st_entry_lhs->lower_bound->value.num;
  upper_bound_lhs = st_entry_lhs->upper_bound->value.num;

  /* determine lower & upper bound, and data type of RHS */
  data_type_rhs = st_entry_rhs->datatype;
  lower_bound_rhs = st_entry_rhs->lower_bound->value.num;
  upper_bound_rhs = st_entry_rhs->upper_bound->value.num;

  /*
    check: lower & upper bound, and data type of LHS & RHS
    have same values respectively
  */
  if (data_type_lhs == data_type_rhs &&
      lower_bound_lhs == lower_bound_rhs &&
      upper_bound_lhs == upper_bound_rhs) {
    return;
  }

  line_number = lhs_array->line_number;
  fprintf(stderr, operator_incompatible_array_types_error_message,
          line_number, terminalLiteralRepresentations[ASSIGNOP],
          lower_bound_lhs, upper_bound_lhs, data_type_lhs,
          lower_bound_rhs, upper_bound_rhs, data_type_rhs);
  semantic_error_count += 1;
}


void assignmentTypeChecker(struct AssignStmtNode *assignment_node) {  
  enum terminal lhs_type, rhs_type;
  struct Attribute *rhs_expression;
  int line_number;

  /* determine data type of LHS */
  lhs_type = leafType(assignment_node->ptr1);
    
  /* determine data type of RHS */
  if (assignment_node->ptr2->type == LVALUE_ID_NODE) {
    rhs_expression = assignment_node->ptr2->node.lva_id->ptr1;
  }
  else {
    rhs_expression = assignment_node->ptr2->node.lva_arr->ptr2;
  }
  rhs_type = expressionType(rhs_expression);

  /*
    check: if both LHS & RHS are arrays,
    then they should have similar array types
  */
  if (lhs_type == rhs_type && rhs_type == ARRAY) {
    arrayAssignemntTypeCheck(assignment_node->ptr1,
                             assignment_node->ptr2->node.lva_id->ptr1->node.lea);
    return;
  }

  /* check: LHS & RHS have same non-null (epsilon) data type */
  if (lhs_type != EPSILON && rhs_type != EPSILON && lhs_type != rhs_type) {
    line_number = assignment_node->ptr1->line_number;
    fprintf(stderr, operator_incompatible_types_error_message,
            line_number,
            terminalLiteralRepresentations[ASSIGNOP],
            terminalStringRepresentations[lhs_type],
            terminalStringRepresentations[rhs_type]);
    semantic_error_count += 1;
  }
}


void arrayModuleReuseTypeCheck(
  struct ArrayTypeNode *formal_array,
  struct LeafNode *actual_array) {
  
  bool is_static_formal;
  struct SymbolTableNode *symbol_table_node;
  struct VariableEntry *st_entry_actual;
  enum terminal data_type_formal, data_type_actual;
  int lower_bound_formal, lower_bound_actual;
  int upper_bound_formal, upper_bound_actual;
  int line_number;
  
  if (formal_array->ptr2->ptr1->type == IDENTIFIER ||
      formal_array->ptr2->ptr1->type == IDENTIFIER) {
    is_static_formal = false;
  }
  else {
    is_static_formal = true;
  }

  symbol_table_node = symbolTableGet(actual_array->scope, actual_array->value.entry);
  if (symbol_table_node == NULL)
    return;
  st_entry_actual = &symbol_table_node->value.variable;

  /* proceed further with semantic checks only if both arrays are static */
  if (!is_static_formal || !(st_entry_actual->isStatic))
    return;

  /* determine lower & upper bound, and data type of LHS */
  data_type_formal = leafType(formal_array->ptr1);
  lower_bound_formal = formal_array->ptr2->ptr1->value.num;
  upper_bound_formal = formal_array->ptr2->ptr2->value.num;

  /* determine lower & upper bound, and data type of RHS */
  data_type_actual = st_entry_actual->datatype;
  lower_bound_actual = st_entry_actual->lower_bound->value.num;
  upper_bound_actual = st_entry_actual->upper_bound->value.num;

  /*
    check: lower & upper bound, and data type of LHS & RHS
    have same values respectively
  */
  if (data_type_formal == data_type_actual &&
      lower_bound_formal == lower_bound_actual &&
      upper_bound_formal == upper_bound_actual) {
    return;
  }

  line_number = actual_array->line_number;
  fprintf(stderr, module_incorrect_input_array_type,
          line_number,
          lower_bound_formal, upper_bound_formal, data_type_formal,
          lower_bound_actual, upper_bound_actual, data_type_actual);
  semantic_error_count += 1;
}


void inputModuleReuseTypeCheck(
  struct InputPlistNode *formal_input,
  struct IdListNode *actual_input) {
  
  enum terminal formal_input_type, actual_input_type;
  int line_number;

  while (formal_input != NULL && actual_input != NULL) {
    /*
      (if formal input is a array)
      check: formal and actual inputs should have the same array types
      (i.e. lower & upper bound and base data type)
    */
    if (formal_input->ptr2->type == ARRAY_TYPE_NODE) {
      inputModuleReuseTypeCheck(
        formal_input->ptr2->node.arr_typ,
        actual_input->ptr1);
    }

    /* check: formal and actual inputs have the same base data types */
    else {
      formal_input_type = leafType(formal_input->ptr2->node.lea);
      actual_input_type = leafType(actual_input->ptr1);
      
      if (formal_input_type != actual_input_type) {
        fprintf(stderr, module_incorrect_input_type,
          line_number,
          terminalStringRepresentations[formal_input_type],
          terminalStringRepresentations[actual_input_type]);
        semantic_error_count += 1;
      }
    }
    
    formal_input = formal_input->ptr3;
    actual_input = actual_input->ptr2;
  }
  
  /* check: total number of formal and input parameters should be equal */
  if (formal_input != NULL) {
    fprintf(stderr, module_input_list_missing_args, line_number);
    semantic_error_count += 1;
  }  
  else if (actual_input != NULL) {
    fprintf(stderr, module_input_list_too_many_args, line_number);
    semantic_error_count += 1;
  }
}


void outputModuleReuseTypeCheck(
  struct OutputPlistNode *formal_output,
  struct IdListNode *actual_output) {
  
  enum terminal formal_output_type, actual_output_type;
  int line_number;

  while (formal_output != NULL && actual_output != NULL) {
    /* determine formal and actual output parameter types */
    formal_output_type = leafType(formal_output->ptr2);
    actual_output_type = leafType(actual_output->ptr1);

    /* check: formal and actual output parameters should have same type */
    if (formal_output_type != actual_output_type) {
      line_number = actual_output->ptr1->line_number;
      fprintf(stderr, module_incorrect_output_type,
              line_number,
              terminalStringRepresentations[formal_output_type],
              terminalStringRepresentations[actual_output_type]);
      semantic_error_count += 1;
    }

    formal_output = formal_output->ptr3;
    actual_output = actual_output->ptr2;
  }

  /* check: total number of formal and output parameters should be equal */
  if (formal_output != NULL) {
    fprintf(stderr, module_output_list_missing_args, line_number);
    semantic_error_count += 1;
  }
  else if (actual_output != NULL) {
    fprintf(stderr, module_output_list_too_many_args, line_number);
    semantic_error_count += 1;
  }
}


void moduleReuseTypeChecker(struct ModuleReuseStmtNode *module_reuse_node) {
  /* determine symbol table entry for the module */
  struct SymbolTableNode *symbol_table_entry;
  symbol_table_entry = symbolTableGet(
    global_symbol_table, module_reuse_node->ptr2->value.entry);
  if (symbol_table_entry == NULL || symbol_table_entry->value_type == ST_VARIABLE)
    return;

  /* semantic checks for formal and actual input parameters */
  inputModuleReuseTypeCheck(
    symbol_table_entry->value.module.inputplist,
    module_reuse_node->ptr3);
  
  /* semantic checks for formal and actual output parameters */
  outputModuleReuseTypeCheck(
    symbol_table_entry->value.module.outputplist,
    module_reuse_node->ptr1);
}


void integerConditionalSemanticCheck(struct ConditionalStmtNode *conditional_node) {
  struct CaseStmtNode *case_node;
  enum terminal case_type;
  int line_number;

  /*
    check: integer conditional should contain only integer case values &
    other semantic checks for case body
  */
  case_node = conditional_node->ptr2;
  while (case_node != NULL) {
    case_type = leafType(case_node->ptr1);
    line_number = case_node->ptr1->line_number;
    
    /* check: case value should be integer */
    if (case_type != INTEGER) {
      fprintf(stderr, conditional_incorrect_case_type_error_message,
              line_number, integer_str, integer_str, case_type);
      semantic_error_count += 1;
    }
    
    /* semantic checks for case body */
    else {
      statementListSemanticChecker(case_node->ptr2);
    }
    
    case_node = case_node->ptr3;
  }

  /* check: integer conditional should have a default case */
  if (conditional_node->ptr3 == NULL) {
    line_number = conditional_node->ptr1->line_number;
    fprintf(stderr, integer_conditional_missing_default_error_message,
            line_number, integer_str, integer_str, case_type);
    semantic_error_count += 1;
    return;
  }
  
  /* semantic checks for default case body */
  statementListSemanticChecker(conditional_node->ptr3);
}


void booleanConditionalSemanticCheck(struct ConditionalStmtNode *conditional_node) {
  struct CaseStmtNode *case_node;
  enum terminal case_type;
  int line_number;

  bool is_exists[2]; /* 0: true & 1: false */
  is_exists[0] = false;
  is_exists[1] = false;
  
  /*
    check: boolean conditional should contain only boolean case values &
    other semantic checks for case body
  */
  case_node = conditional_node->ptr2;
  while (case_node != NULL) {
    case_type = leafType(case_node->ptr1->type);
    line_number = case_node->ptr1->line_number;

    /* check: case value should be boolean */
    if (case_type != BOOLEAN_) {
      fprintf(stderr, conditional_incorrect_case_type_error_message,
              line_number, boolean_str, boolean_str, case_type);
      semantic_error_count += 1;
    }
    
    else {
      case_type = case_node->ptr1->type;
      if (case_type == TRUE_) {
        is_exists[0] = true;
      }
      else {
        is_exists[1] = true;
      }

    /* semantic checks for case body */
    statementListSemanticChecker(case_node->ptr2);
    }
    
    
    case_node = case_node->ptr3;
  }

  /* check: boolean conditional should have cases for both True and False values */
  if (!is_exists[0]) {
    fprintf(stderr, boolean_conditional_missing_case_error_message,
            line_number, true_str);
    semantic_error_count += 1;
  }
  if (!is_exists[1]) {
    fprintf(stderr, boolean_conditional_missing_case_error_message,
            line_number, false_str);
    semantic_error_count += 1;
  }

  /* check: boolean conditional should NOT have a default case */
  if (conditional_node->ptr3 == NULL) {
    line_number = conditional_node->ptr1->line_number;
    fprintf(stderr, boolean_conditional_contains_default_error_message,
            line_number);
    semantic_error_count += 1;
    return;
  }
}


void conditionalSemanticChecker(struct ConditionalStmtNode *conditional_node) {
  enum terminal conditional_type;
  int line_number;
  
  conditional_type = leafType(conditional_node->ptr1);
  line_number = conditional_node->ptr1->line_number;
  
  switch (conditional_type) {    
    case INTEGER:
      integerConditionalSemanticCheck(conditional_node);
      break;    
    case BOOLEAN_:
      booleanConditionalSemanticCheck(conditional_node);
      break;
    case REAL:
    case ARRAY:
      fprintf(stderr, conditional_incorrect_switch_type_error_message,
              line_number, conditional_type);
      break;
    default:
      printf("ERROR!! Invalid type for switch variable: %d\n", conditional_type);
      break;
  }
}


void forIterationSemanticChecker(struct ForIterativeStmtNode *for_iter_node) {
  enum terminal loop_var_type;
  int lower_bound, upper_bound;
  int line_number;

  /* check: for loop variable should be of type integer */
  loop_var_type = leafType(for_iter_node->ptr1);
  line_number = for_iter_node->ptr1->line_number;
  if (loop_var_type != INTEGER) {
    fprintf(stderr, for_incorrect_variable_type_error_message,
            line_number, loop_var_type);
    semantic_error_count += 1;
  }

  /* check: lower bound should be less than / equal to upper bound */
  lower_bound = for_iter_node->ptr2->ptr1->value.num;
  upper_bound = for_iter_node->ptr2->ptr2->value.num;
  line_number = for_iter_node->ptr2->ptr1->line_number;
  if (lower_bound > upper_bound) {
    fprintf(stderr, for_incorrect_bounds_error_message,
            line_number);
    semantic_error_count += 1;
  }
  
  /* semantic checks for loop body */
  statementListSemanticChecker(for_iter_node->ptr3);

  /*
    check: for loop variable should NOT be
    assigned a value inside the loop
  */
}


void whileIterationSemanticChecker(struct WhileIterativeStmtNode *while_iter_node) {
  enum terminal loop_expression_type;
  int line_number;

  /* check: while loop expression should be of type boolean */
  loop_expression_type = expressionType(while_iter_node->ptr1);
  line_number = getLineNumber(while_iter_node->ptr1);
  if (loop_expression_type != BOOLEAN_ && loop_expression_type != EPSILON) {
    fprintf(stderr, while_incorrect_expression_type_error_message,
            line_number, loop_expression_type);
    semantic_error_count += 1;
  }

  /* semantic checks for loop body */
  statementListSemanticChecker(while_iter_node->ptr2);

  /*
    check: at least one variable in the loop expression
    should be assigned a value inside the loop
  */
}


void statementSemanticChecker(struct StatementNode *statement_node) {
  if (statement_node == NULL)
    return;
  
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
