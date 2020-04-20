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
in input list for module reuse\n";
char *module_input_list_too_many_args = "Line %d: (Semantic Error) Too many arguments were provided in input list \
for module reuse\n";
char *module_incorrect_output_type = "Line %d: (Type Error) Module reuse expects type %s in output list, found %s\n";
char *module_output_list_missing_args = "Line %d: (Semantic Error) Insufficient number of arguments were provided \
in output list for module reuse\n";
char *module_output_list_too_many_args = "Line %d: (Semantic Error) Too many arguments were provided in output list \
for module reuse\n";

/* error messages: conditionals */
char *conditional_incorrect_switch_type_error_message = "Line %d: (Type Error) Construct 'switch' expects type \
INTEGER or BOOLEAN, found %s\n";
char *conditional_incorrect_case_type_error_message = "Line %d: (Type Error) Construct 'switch' is over type %s. \
So, 'case' expects type %s for values, found %s\n";
char *integer_conditional_missing_default_error_message = "Line %d: (Semantic Error) Construct 'switch' is over \
type INTEGER. So, it MUST have a default case\n";
char *boolean_conditional_missing_case_error_message = "Line %d: (Semantic Error) Construct 'switch' is over type \
BOOLEAN. So, it MUST have a case for value %s\n";
char *boolean_conditional_contains_default_error_message = "Line %d: (Semantic Error) Construct 'switch' is over \
type BOOLEAN. So, it must NOT have a default case\n";

/* error messages: for iteration */
char *for_incorrect_variable_type_error_message = "Line %d: (Type Error) Construct 'for' expects type INTEGER for \
loop variable, found %s\n";
char *for_incorrect_bounds_error_message = "Line %d: (Semantic Error) Construct 'for' should get a lower bound less \
than / equal to the upper bound\n";
char *for_loop_var_unassigned_error_message = "Line %d: (Semantic Error) Construct 'for' loop variable should NOT \
be assigned a value inside the loop body\n";

/* error messages: while iteration */
char *while_incorrect_expression_type_error_message = "Line %d: (Type Error) Construct 'while' excepts type BOOLEAN \
for loop expression, found %s\n";
char *while_loop_variables_not_updated = "Line %d: (Semantic Error) While loop variable not being updated: Infinite Loop Possible\n";

/* error message: module definition */
char *module_output_variable_unassigned = "Line %d: (Semantic Error) Parameter in output list is never assigned a \
value in the module definition\n";

char *semantic_errors_count_message = "\nDetected %d semantic error(s) while checking the input code.\n";

/* utility strings */
char *integer_str = "INTEGER";
char *real_str = "REAL";
char *integer_or_real_str = "INTEGER or REAL";
char *boolean_str = "BOOLEAN";
char *array_str = "ARRAY";
char *true_str = "TRUE";
char *false_str = "FALSE";

/* global variables */
extern int semantic_error_count;


enum terminal leafType(struct LeafNode *leaf) {
  struct VariableEntry *symbol_table_entry;

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
      symbol_table_entry = resolveVariable((char *) leaf->value.entry, leaf->scope);
      if (symbol_table_entry == NULL) {
        return EPSILON;
      }
      else if (symbol_table_entry->isArray)
        return ARRAY;
      else
        return symbol_table_entry->datatype;
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
  struct VariableEntry *symbol_table_entry;
  enum terminal array_type, index_type;
  int index, lower_bound, upper_bound;
  int line_number;

  /* determine array primitive type */
  symbol_table_entry = resolveVariable(array->ptr1->value.entry, array->ptr1->scope);
  if (symbol_table_entry == NULL)
    return EPSILON;
  array_type = symbol_table_entry->datatype;
  
  /* determine index type */
  index_type = leafType(array->ptr2);
  if (index_type == EPSILON)
    return EPSILON;

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
  if (symbol_table_entry->isStatic && array->ptr2->type == NUM) {
    lower_bound = symbol_table_entry->lower_bound->value.num;
    upper_bound = symbol_table_entry->upper_bound->value.num;
    index = array->ptr2->value.num;
    if (index < lower_bound || index > upper_bound) {
      fprintf(stderr, array_index_out_of_bounds_error_message,
      line_number, lower_bound, upper_bound);
      semantic_error_count += 1;
    }
  }

  return array_type;
}


/*
  utility function to improve reusability for printing
  the "operator_incorrect_type_error_message", which is
  needed many times while calculating expression type
*/
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


/*
  utility function to improve reusability for printing
  the "operator_incorrect_type_error_message", which is
  needed many times while calculating expression type
*/
void operatorIncompatibleTypesError(
  int line_number, enum terminal op,
  enum terminal left_type, enum terminal right_type) {
  
  fprintf(stderr, operator_incompatible_types_error_message,
          line_number,
          terminalLiteralRepresentations[op],
          terminalStringRepresentations[left_type],
          terminalStringRepresentations[right_type]);
  
  semantic_error_count += 1;
}


enum terminal logicalExpressionType(struct N7Node *logical_expression) {
  enum terminal left_operand_type, right_operand_type;
  int line_number;
  
  /* determine left operand type */
  left_operand_type = expressionType(logical_expression->ptr1);
  if (left_operand_type == EPSILON)
    return EPSILON;

  /* determine right operand type */
  right_operand_type = expressionType(logical_expression->ptr2);
  if (right_operand_type == EPSILON)
    return EPSILON;
  
  line_number = logical_expression->line_number;
  
  /* check: left operand should be of type BOOLEAN */
  if (left_operand_type != BOOLEAN_) {
    operatorIncorrectTypeError(
      line_number, logical_expression->logicalOp,
      boolean_str, left_operand_type);
    return EPSILON;
  }
  
  /* check: right operand should be of type BOOLEAN */
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
  
  /* determine left operand type */
  left_operand_type = expressionType(relational_expression->ptr1);
  if (left_operand_type == EPSILON)
    return EPSILON;

  /* determine right operand type */
  right_operand_type = expressionType(relational_expression->ptr2);
  if (right_operand_type == EPSILON)
    return EPSILON;

  line_number = relational_expression->line_number;
  
  /* check: left operand should be of type INTEGER or REAL */
  if (left_operand_type != INTEGER && left_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, relational_expression->relationalOp,
      integer_or_real_str, left_operand_type);
    return EPSILON;
  }
  
  /* check: right operand should be of type INTEGER or REAL */
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
  
  /* determine the right operand type */
  right_operand_type = expressionType(arithmetic_expression->ptr2);
  if (right_operand_type == EPSILON)
    return EPSILON;
  
  /* check: right operand should be of type INTEGER or REAL */
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, arithmetic_expression->op,
      integer_or_real_str, right_operand_type);
    return EPSILON;
  }
  
  /*
    if ptr1 of arithmetic expression node points to an orignal 
    expression, and NOT the previous arithmetic expression node
    (this is determined by is_first attribute) then perform semantic checks
  */
  if (arithmetic_expression->is_first) {
    /* determine the left operand type */
    left_operand_type = expressionType(arithmetic_expression->ptr1);
    if (left_operand_type == EPSILON)
      return EPSILON;
    
    /* check: left operand should be a INTEGER or REAL */
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, arithmetic_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }

    /* check: left and right operand should have the same types */
    if (left_operand_type != right_operand_type) {
      operatorIncompatibleTypesError(
        line_number, arithmetic_expression->op,
        left_operand_type, right_operand_type);    
      return EPSILON;
    }
  }
  
  /*
    this is the check that will be performed for ALL arithmetic expression
    nodes unless it's last in the list of arithmetic expression nodes.
    Here: variables have been interchanged, left operand type should
    have been right operand type and vice versa
  */
  if (arithmetic_expression->ptr3->type != NULL_NODE) {
    /* determine the left operand type */
    left_operand_type = expressionType(arithmetic_expression->ptr3);
    if (left_operand_type == EPSILON)
      return EPSILON;

    /* check: left operand should be a INTEGER or REAL */
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, arithmetic_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }
    
    /* check: left and right operand should have the same types */
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

  /* determine right operand type */
  right_operand_type = expressionType(term_expression->ptr2);
  if (right_operand_type == EPSILON)
    return EPSILON;
  
  /* check: right operand should be an INTEGER or REAL */
  if (right_operand_type != INTEGER && right_operand_type != REAL) {
    operatorIncorrectTypeError(
      line_number, term_expression->op,
      integer_or_real_str, right_operand_type);
    return EPSILON;
  }

  /*
    if ptr1 of term expression node points to an orignal 
    expression, and NOT the previous term expression node
    (this is determined by is_first attribute) then perform semantic checks
  */
if (term_expression->is_first) {
    /* determine the left operand type */
    left_operand_type = expressionType(term_expression->ptr1);
    if (left_operand_type == EPSILON)
      return EPSILON;

    /* check: left operand should be a INTEGER or REAL */
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, term_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }
    
    /* check: left and right operand should have the same types */
    if (left_operand_type != right_operand_type) {
      operatorIncompatibleTypesError(
        line_number, term_expression->op,
        left_operand_type, right_operand_type);    
      return EPSILON;
    }
  }
  
  /*
    this is the check that will be performed for ALL term expression
    nodes unless it's last in the list of term expression nodes.
    Here: variables have been interchanged, left operand type should
    have been right operand type and vice versa
  */
  if (term_expression->ptr3->type != NULL_NODE) {
    /* determine the left operand type */
    left_operand_type = expressionType(term_expression->ptr3);
    if (left_operand_type == EPSILON)
      return EPSILON;

    /* check: left operand should be a INTEGER or REAL */
    if (left_operand_type != INTEGER && left_operand_type != REAL) {
      operatorIncorrectTypeError(
        line_number, term_expression->op,
        integer_or_real_str, left_operand_type);
      return EPSILON;
    }
    
    /* check: left and right operand should have the same types */
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
  struct VariableEntry *st_entry_lhs, *st_entry_rhs;
  enum terminal data_type_lhs, data_type_rhs;
  int lower_bound_lhs, lower_bound_rhs;
  int upper_bound_lhs, upper_bound_rhs;
  int line_number;
  
  st_entry_lhs = resolveVariable(lhs_array->value.entry, lhs_array->scope);
  if (st_entry_lhs == NULL)
    return;

  st_entry_rhs = resolveVariable(rhs_array->value.entry, rhs_array->scope);
  if (st_entry_rhs == NULL)
    return;

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
          lower_bound_lhs, upper_bound_lhs,
          terminalStringRepresentations[data_type_lhs],
          lower_bound_rhs, upper_bound_rhs,
          terminalStringRepresentations[data_type_rhs]);
  semantic_error_count += 1;
}


void assignmentTypeChecker(struct AssignStmtNode *assignment_node) {  
  enum terminal lhs_type, rhs_type;
  struct Attribute *rhs_expression;
  struct ArrayNode array_node;
  int line_number;
    
  /* determine data type of LHS & RHS */
  if (assignment_node->ptr2->type == LVALUE_ID_NODE) {
    lhs_type = leafType(assignment_node->ptr1);
    rhs_expression = assignment_node->ptr2->node.lva_id->ptr1;
  }
  else {
    /*
      create a new array node to determine the type of
      array element in LHS
    */
    array_node.ptr1 = assignment_node->ptr1;
    array_node.ptr2 = assignment_node->ptr2->node.lva_arr->ptr1;
    lhs_type = arrayType(&array_node);
    rhs_expression = assignment_node->ptr2->node.lva_arr->ptr2;
  }
  rhs_type = expressionType(rhs_expression);

  if (lhs_type == EPSILON || rhs_type == EPSILON)
    return;

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
  struct VariableEntry *st_entry_actual;
  enum terminal data_type_formal, data_type_actual;
  int lower_bound_formal, lower_bound_actual;
  int upper_bound_formal, upper_bound_actual;
  int line_number;
  
  /* determine whether the formal parameter array is static or not */
  if (formal_array->ptr2->ptr1->type == IDENTIFIER ||
      formal_array->ptr2->ptr1->type == IDENTIFIER) {
    is_static_formal = false;
  }
  else {
    is_static_formal = true;
  }

  st_entry_actual = resolveVariable(actual_array->value.entry, actual_array->scope);
  if (st_entry_actual == NULL)
    return;
  
  /* check: actual parameter should be an array */
  if (!st_entry_actual->isArray) {
    data_type_formal = ARRAY;
    data_type_actual = st_entry_actual->datatype;
    line_number = actual_array->line_number;
    fprintf(stderr, module_incorrect_input_type,
            line_number,
            terminalStringRepresentations[data_type_formal],
            terminalStringRepresentations[data_type_actual]);
    semantic_error_count += 1;
    return;
  }
  
  /* proceed further with semantic checks only if both arrays are static */
  if (!is_static_formal || !(st_entry_actual->isStatic))
    return;

  /* determine lower & upper bound, and data type of LHS */
  data_type_formal = leafType(formal_array->ptr1);
  if (data_type_formal == EPSILON)
    return;
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
          lower_bound_formal, upper_bound_formal,
          terminalStringRepresentations[data_type_formal],
          lower_bound_actual, upper_bound_actual,
          terminalStringRepresentations[data_type_actual]);
  semantic_error_count += 1;
}


void inputModuleReuseTypeCheck(
  struct InputPlistNode *formal_input,
  struct IdListNode *actual_input,
  int line_number) {
  
  enum terminal formal_input_type, actual_input_type;
  
  line_number = actual_input->ptr1->line_number;

  while (formal_input != NULL && actual_input != NULL) {
    /*
      perform semantic checks based on whether the expected
      input parameter is an array or not
    */
    if (formal_input->ptr2->type == ARRAY_TYPE_NODE) {
      /* check: formal and actual inputs should have the same array types */
      arrayModuleReuseTypeCheck(
        formal_input->ptr2->node.arr_typ,
        actual_input->ptr1);
    }
    else {
      /* determine formal & actual input variable types */
      formal_input_type = leafType(formal_input->ptr2->node.lea);
      actual_input_type = leafType(actual_input->ptr1);
      if (formal_input_type == EPSILON || actual_input_type == EPSILON)
        continue;
      
      /* check: formal and actual inputs have the same base data types */
      if (formal_input_type != actual_input_type) {
        line_number = actual_input->ptr1->line_number;
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
  struct IdListNode *actual_output,
  int line_number) {
  
  enum terminal formal_output_type, actual_output_type;

  while (formal_output != NULL && actual_output != NULL) {
    /* determine formal and actual output parameter types */
    formal_output_type = leafType(formal_output->ptr2->node.lea);
    actual_output_type = leafType(actual_output->ptr1);

    if (formal_output_type == EPSILON || actual_output_type == EPSILON)
      return;

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
  struct ModuleEntry *symbol_table_entry;
  int line_number;
  
  /* determine symbol table entry for the module */
  symbol_table_entry = resolveModule(module_reuse_node->ptr2->value.entry);
  if (symbol_table_entry == NULL)
    return;

  line_number = module_reuse_node->ptr2->line_number;

  /* semantic checks for formal and actual input parameters */
  inputModuleReuseTypeCheck(
    symbol_table_entry->inputplist,
    module_reuse_node->ptr3, line_number);
  
  /* semantic checks for formal and actual output parameters */
  outputModuleReuseTypeCheck(
    symbol_table_entry->outputplist,
    module_reuse_node->ptr1, line_number);
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

    /* determine the case value type */
    case_type = leafType(case_node->ptr1);
    if (case_type == EPSILON) {
      case_node = case_node->ptr3;
      continue;
    }

    line_number = case_node->ptr1->line_number;
    
    /* check: case value should be integer */
    if (case_type != INTEGER) {
      fprintf(stderr, conditional_incorrect_case_type_error_message,
              line_number, integer_str, integer_str,
              terminalStringRepresentations[case_type]);
      semantic_error_count += 1;
    }  
    else {
      /* semantic checks for case body */
      statementListSemanticChecker(case_node->ptr2);
    }
    
    case_node = case_node->ptr3;
  }

  /* check: integer conditional should have a default case */
  if (conditional_node->ptr3 == NULL) {
    line_number = conditional_node->ending_line_number;
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
    
    /* determine the case value type */
    case_type = leafType(case_node->ptr1);
    if (case_type == EPSILON) {
      case_node = case_node->ptr3;
      continue;
    }
    
    line_number = case_node->ptr1->line_number;

    /* check: case value should be boolean */
    if (case_type != BOOLEAN_) {
      fprintf(stderr, conditional_incorrect_case_type_error_message,
              line_number, boolean_str, boolean_str,
              terminalStringRepresentations[case_type]);
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
  if (conditional_node->ptr3 != NULL) {
    line_number = conditional_node->ending_line_number;
    fprintf(stderr, boolean_conditional_contains_default_error_message,
            line_number);
    semantic_error_count += 1;
    return;
  }
}


void conditionalSemanticChecker(struct ConditionalStmtNode *conditional_node) {
  enum terminal conditional_type;
  int line_number;
  
  /* determine conditional statement type from switch variable type*/
  conditional_type = leafType(conditional_node->ptr1);
  if (conditional_type == EPSILON)
    return;
  
  line_number = conditional_node->ptr1->line_number;
  
  /* perform semantic checks based on conditional type */
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
              line_number, terminalStringRepresentations[conditional_type]);
      break;
    default:
      printf("ERROR!! Invalid type for switch variable: %d\n", conditional_type);
      break;
  }
}


bool cmpIdentifier(struct LeafNode *leaf1, struct LeafNode *leaf2) {
  struct VariableEntry *variable_entry1, *variable_entry2;
  int is_same;

  if (leaf1->type != IDENTIFIER || leaf2->type != IDENTIFIER) {
    printf("ERROR!! cmpIdentifier got a non-identifier\n");
    return false;
  }
  
  /* check: leaf 1 & 2 have the same identifier */
  is_same = strcmp (leaf1->value.entry, leaf2->value.entry);

  /* determine symbol table variable entries for leaf 1 & 2 */
  variable_entry1 = resolveVariable(leaf1->value.entry, leaf1->scope);
  variable_entry2 = resolveVariable(leaf2->value.entry, leaf2->scope);
  if (variable_entry1 == NULL || variable_entry2 == NULL)
    return false;

  /* check: leaf 1 & 2 belong to the same scope */
  if (is_same == 0 && variable_entry1 != variable_entry2) {
    is_same = 1;
  }

  if (is_same == 0)
    return true;
  return false;
}


void forIterationVariableUpdateCheck(
  struct LeafNode *loop_var,
  struct StatementNode *statement_node) {

  struct IdListNode *id_list_node;
  struct CaseStmtNode *case_node;
  bool is_same;
  int line_number;

  /*
    if statement assigns value to identifier loop_var,
    then set is variable updated to true and return
  */
  while (statement_node != NULL) {
    is_same = false;

    switch (statement_node->ptr1->type) {
      case ASSIGN_STMT_NODE:
        /* check: compare loop var with lhs */
        is_same = cmpIdentifier(
                    loop_var,
                    statement_node->ptr1->node.agn_stm->ptr1);
        if (is_same) {
          line_number = statement_node->ptr1->node.agn_stm->ptr1->line_number;
          fprintf(stderr, for_loop_var_unassigned_error_message, line_number);
          semantic_error_count += 1;
        }
        break;
      
      case MODULE_REUSE_STMT_NODE:
        /* iterate through list of actual output parameters */
        id_list_node = statement_node->ptr1->node.mod_reu_stm->ptr3;
        while (id_list_node != NULL) {
          /* check: compare loop var with output parameter list */
          is_same = cmpIdentifier(loop_var, id_list_node->ptr1);
          if (is_same) {
            line_number = id_list_node->ptr1->line_number;
            fprintf(stderr, for_loop_var_unassigned_error_message, line_number);
            semantic_error_count += 1;
          }
          id_list_node = id_list_node->ptr2;
          is_same = false;
        }
        break;
      
      case CONDITIONAL_STMT_NODE:
        /*
          check: recursively find if loop var is assigned a value
          in any case body of the conditional statement
        */
        case_node = statement_node->ptr1->node.con_stm->ptr2;
        while (case_node != NULL) {
          forIterationVariableUpdateCheck(loop_var, case_node->ptr2);
          case_node = case_node->ptr3;
        }
        break;
      
      case FOR_ITERATIVE_STMT_NODE:
        /*
          check: recursively find if loop var is assigned a value
          in the for loop body
        */
        forIterationVariableUpdateCheck(
          loop_var,
          statement_node->ptr1->node.for_ite_stm->ptr3);
        break;
      
      case WHILE_ITERATIVE_STMT_NODE:
        /*
          check: recursively find if loop var is assigned a value
          in the while loop body
        */
        forIterationVariableUpdateCheck(
          loop_var,
          statement_node->ptr1->node.whi_ite_stm->ptr2);
        break;
      
      case INPUT_NODE:
        /* check: compare loop var with input variable */
        is_same = cmpIdentifier(loop_var, statement_node->ptr1->node.inp->ptr1);
        if (is_same) {
          line_number = statement_node->ptr1->node.inp->ptr1->line_number;
          fprintf(stderr, for_loop_var_unassigned_error_message, line_number);
          semantic_error_count += 1;
        }
        break;
      
      /* following statement types don't assign any variable a value */
      case PRINT_NODE:
      case DECLARE_STMT_NODE:
        break;
      
      default:
        printf("ERROR!! Invalid type for statement node %d\n", statement_node->ptr1->type);
        break;
    }

    statement_node = statement_node->ptr2;
  }
}


void forIterationSemanticChecker(struct ForIterativeStmtNode *for_iter_node) {
  enum terminal loop_var_type;
  int lower_bound, upper_bound;
  int line_number;

  /* determine loop variable type */
  loop_var_type = leafType(for_iter_node->ptr1);
  if (loop_var_type == EPSILON)
    return;
  
  line_number = for_iter_node->ptr1->line_number;
  
  /* check: for loop variable should be of type integer */
  if (loop_var_type != INTEGER) {
    fprintf(stderr, for_incorrect_variable_type_error_message,
            line_number,
            terminalStringRepresentations[loop_var_type]);
    semantic_error_count += 1;
  }

  /* check: lower bound should be less than / equal to upper bound */
  lower_bound = for_iter_node->ptr2->ptr1->value.num;
  upper_bound = for_iter_node->ptr2->ptr2->value.num;
  if (lower_bound > upper_bound) {
    line_number = for_iter_node->ptr2->ptr1->line_number;
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
 forIterationVariableUpdateCheck(for_iter_node->ptr1, for_iter_node->ptr3);
}


void whileIterationSemanticChecker(struct WhileIterativeStmtNode *while_iter_node) {
  enum terminal loop_expression_type;
  int line_number;

  /* determine loop expression type */
  loop_expression_type = expressionType(while_iter_node->ptr1);
  if (loop_expression_type == EPSILON)
    return;

  /* check: while loop expression should be of type boolean */
  if (loop_expression_type != BOOLEAN_) {
    line_number = while_iter_node->starting_line_number;
    fprintf(stderr, while_incorrect_expression_type_error_message,
            line_number,
            terminalStringRepresentations[loop_expression_type]);
    semantic_error_count += 1;
  }

  /* semantic checks for loop body */
  statementListSemanticChecker(while_iter_node->ptr2);

  /*
    check: at least one variable in the loop expression should be assigned a value inside the loop
  */
  int found_var_change = 0;
  struct StatementNode *loop_body = while_iter_node->ptr2;
  struct LeafNode *leaf = NULL;
  while(loop_body != NULL){
    if(loop_body->ptr1->type == ASSIGN_STMT_NODE){
      leaf = loop_body->ptr1->node.agn_stm->ptr1;
      if (checkInExpression(leaf->value.entry, while_iter_node->ptr1)){ // checks if identifier in assignment statement is in loop expr
        found_var_change = 1;
        break;
      }
    }
    loop_body = loop_body->ptr2;
  }
  if(found_var_change == 0){
    fprintf(stderr, while_loop_variables_not_updated, while_iter_node->starting_line_number);
    semantic_error_count += 1;
  }  


}

//function to check if given expression attribute contains the given leaf node
bool checkInExpression(char *entry, struct Attribute *expr){
  
  bool res1 = false;
  bool res2 = false; 
  bool res3 = false;
  // printf("IN function:");
  switch (expr->type){
    case U_NODE:
      return checkInExpression(entry, expr->node.u->ptr1);
      break;
    case N7_NODE:
      res1 = checkInExpression(entry,expr->node.n7->ptr1);
      res2 = checkInExpression(entry,expr->node.n7->ptr2);
      return (res1 || res2);
      break;
    case N8_NODE:
      res1 = checkInExpression(entry, expr->node.n8->ptr1);
      res2 = checkInExpression(entry, expr->node.n8->ptr2);
      return (res1 || res2);
      break;
    case ARITHMETIC_EXPR_NODE:
      if (expr->node.ari_exp->is_first)
        res1 = checkInExpression(entry, expr->node.ari_exp->ptr1);
      res2 = checkInExpression(entry, expr->node.ari_exp->ptr2);
      res3 = checkInExpression(entry, expr->node.ari_exp->ptr3);
      return (res1 || res2 ||res3);
      break;
    case TERM_NODE:
      if (expr->node.ter->is_first)
        res1 = checkInExpression(entry, expr->node.ter->ptr1);
      res2 = checkInExpression(entry, expr->node.ter->ptr2);
      res3 = checkInExpression(entry, expr->node.ter->ptr3);
      return (res1 || res2 || res3);
      break;
    case ARRAY_NODE:
      /* code */
      break;
    case LEAF_NODE:
      if (expr->node.lea->type != IDENTIFIER)
        return false;
      if(strcmp(expr->node.lea->value.entry,entry) == 0)
        return true;
      break;

    default:
      return false;
      break;
  }
  return false;
}

void statementSemanticChecker(struct StatementNode *statement_node) {
  if (statement_node == NULL)
    return;
  
  switch (statement_node->ptr1->type) {
    case ASSIGN_STMT_NODE:
      assignmentTypeChecker(statement_node->ptr1->node.agn_stm);
      break;
    case MODULE_REUSE_STMT_NODE:
      moduleReuseTypeChecker(statement_node->ptr1->node.mod_reu_stm);
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


void variableListUpdateUtility(
  struct LeafNode *leaf,
  struct OutputPlistNode *output_list_node,
  bool *is_updated) {

  bool is_same;
  int list_index;
  
  is_same = false;
  list_index = 0;

  while (output_list_node != NULL) {
    is_same = cmpIdentifier(output_list_node->ptr1, leaf);
    if (is_same) {
      is_updated[list_index] = true;
      return;
    }
    list_index += 1;
    output_list_node = output_list_node->ptr3;
  }

}


void variableListUpdateSemanticCheck(
  struct OutputPlistNode *output_variable_list,
  bool *is_updated,
  struct StatementNode *statement_node) {

  /*
    utility variables for aid in semantic checks with
    different types of statements
  */
  struct IdListNode *id_list_node;
  struct CaseStmtNode *case_node;
  
  /*
    for every statement type that updates the value of a variable
    iterate through the variable list to determine if the statement
    assigns a value to an identifier in this list, and if it does,
    then set the correspoding element in is_updated_array to true 
  */
  while (statement_node != NULL) {

    switch (statement_node->ptr1->type) {

      case ASSIGN_STMT_NODE:
        /* check: compare with lhs */
        variableListUpdateUtility(
          statement_node->ptr1->node.agn_stm->ptr1,
          output_variable_list, is_updated);
        break;
      
      case MODULE_REUSE_STMT_NODE:
        /* iterate through list of actual output parameters */
        id_list_node = statement_node->ptr1->node.mod_reu_stm->ptr3;
        while (id_list_node != NULL) {
          /* check: compare with output parameter list */
          variableListUpdateUtility(id_list_node->ptr1, output_variable_list, is_updated);
          id_list_node = id_list_node->ptr2;
        }
        break;
      
      case CONDITIONAL_STMT_NODE:
        /*
          check: recursively find if a variable in the variable list is assigned
          a value in the scope of one of the cases
        */
        case_node = statement_node->ptr1->node.con_stm->ptr2;
        while (case_node != NULL) {
          variableListUpdateSemanticCheck(output_variable_list, is_updated, case_node->ptr2);
          case_node = case_node->ptr3;
        }
        break;
      
      case FOR_ITERATIVE_STMT_NODE:
        /*
          check: recursively find if a variable in the variable list 
          is assigned a value in the for loop body
        */
        variableListUpdateSemanticCheck(
          output_variable_list, is_updated,
          statement_node->ptr1->node.for_ite_stm->ptr3);
        break;
      
      case WHILE_ITERATIVE_STMT_NODE:
        /*
          check: recursively find if a variable in the variable list 
          is assigned a value in the while loop body
        */
        variableListUpdateSemanticCheck(
          output_variable_list, is_updated,
          statement_node->ptr1->node.whi_ite_stm->ptr2);
        break;
      
      case INPUT_NODE:
        /* check: compare with input variable */
        variableListUpdateUtility(
          statement_node->ptr1->node.inp->ptr1,
          output_variable_list, is_updated);
        break;
      
      /* following statement types don't assign any variable a value */
      case PRINT_NODE:
      case DECLARE_STMT_NODE:
        break;
      
      default:
        printf("ERROR!! Invalid type for statement node %d\n", statement_node->ptr1->type);
        break;
    }

    statement_node = statement_node->ptr2;
  }
}


void moduleSemanticChecker(struct ModuleNode *module_node) {
  struct OutputPlistNode *output_list_node;
  bool *is_updated, semantic_check;
  int count_param, line_number;
  
  /* semantic checks for statements in the module scope */
  statementListSemanticChecker(module_node->ptr4);

  /*
    determine the number of output parameters and allocate
    a boolean array with number of elements equal to former
  */
  count_param = 0;
  output_list_node = module_node->ptr3;
  while (output_list_node != NULL) {
    count_param += 1;
    output_list_node = output_list_node->ptr3;
  }
  is_updated = (bool *) malloc(sizeof(bool) * count_param);

  /*
    check: ALL output parameters are assigned a value inside
    the module scope
  */
  output_list_node = module_node->ptr3;
  variableListUpdateSemanticCheck(
    output_list_node,
    is_updated,
    module_node->ptr4);
  
  semantic_check = true;
  for (int idx = 0; idx < count_param; idx++) {
    semantic_check = semantic_check && is_updated[idx];
  }

  if (!semantic_check) {
    line_number = module_node->ending_line_number;
    fprintf(stderr, module_output_variable_unassigned, line_number);
    semantic_error_count += 1;
  }
}


void moduleListSemanticChecker(struct OtherModuleNode *other_module_node) {
  while(other_module_node != NULL) {
    moduleSemanticChecker(other_module_node->ptr1);
    other_module_node = other_module_node->ptr2;
  }
}


void semanticChecker(struct ProgramNode *AST) {
  /* semantic checks for modules defined before the driver */
  moduleListSemanticChecker(AST->ptr2);
  
  /* semantic checks for statements in the driver scope*/
  statementListSemanticChecker(AST->ptr3);
  
  /* semantic checks for modules defined after the driver */
  moduleListSemanticChecker(AST->ptr4);

  /* print error statistics */
  if (semantic_error_count > 0) {
    fprintf(stdout, semantic_errors_count_message, semantic_error_count);
  }
}
