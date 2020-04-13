#include "intCode.h"

// to be removed- just added here for testing code
// actual code is in codeGen.c
void cg_ADD_SUB(ICInstr *ic_instr)
{
  char instr[100] = ""; //final instruction -temporary max length as 100

  char addr1[30];
  char store_reg1[50] = "\nMOV\tEAX , ";
  char addr2[30];
  char store_reg2[50] = "\nMOV\tEBX , ";
  // for addr1
  switch ((ic_instr->addr1).type)
  {
  case NUM:
    sprintf(addr1, "%d", (ic_instr->addr1).value.num);
    break;
  case RNUM:
    sprintf(addr1, "%f", (ic_instr->addr1).value.rnum);
    break;
  case BOOLEAN_:
    if ((ic_instr->addr1).value.boolean == true)
      sprintf(addr1, "True");
    else
      sprintf(addr1, "False");
    break;
  case IDENTIFIER:
    sprintf(addr1, "mem(%s)", (char *)(ic_instr->addr1).value.symbol);
    break;
  default:
    sprintf(addr1, "NULL");
    break;
  }
  // for addr2
  switch ((ic_instr->addr2).type)
  {
  case NUM:
    sprintf(addr2, "%d", (ic_instr->addr2).value.num);
    break;
  case RNUM:
    sprintf(addr2, "%f", (ic_instr->addr2).value.rnum);
    break;
  case BOOLEAN_:
    if ((ic_instr->addr2).value.boolean == true)
      sprintf(addr2, "True");
    else
      sprintf(addr2, "False");
    break;
  case IDENTIFIER:
    sprintf(addr2, "mem(%s)", (char *)(ic_instr->addr2).value.symbol);
    break;
  default:
    sprintf(addr2, "NULL");
    break;
  }
  // store instr
  strcat(store_reg1,addr1);
  strcat(store_reg2,addr2);
  // into code
  strcat(instr, store_reg1);
  strcat(instr, store_reg2);

  switch (ic_instr->op){
  case icADD:
    strcat(instr, "\nADD\t");
    break;
  case icSUB:
    strcat(instr, "\nSUB\t");
    break;
  default:
    strcat(instr, "BLAH\t");
  }
  // operation
  strcat(instr,"EAX , EBX");
  // next is addr3- move the result in EAX to mem
  strcat(instr,"\nMOV\tEAX, ");
  strcat(instr, "mem(");
  strcat(instr, (char *)(ic_instr->addr3).value.symbol);
  strcat(instr, ")\t");

  // print out result
  printf("%s", instr);
}



ICInstr *start_global_ic_instr = NULL;
ICInstr *global_ic_instr = NULL;
int temporary_count = 0;


ICInstr * newICInstruction() {
  ICInstr *ic_instr = (ICInstr *) malloc(sizeof(ICInstr));
  ic_instr->next = NULL;
  return ic_instr;
}


ICAddr newTemporary() {
  ICAddr ic_addr;
  ic_addr.type = IDENTIFIER;
  ic_addr.value.symbol = malloc(sizeof(char) * 5);
  strcpy(ic_addr.value.symbol, "TEMP");
  return ic_addr;
}


ICAddr newLabel() {
  ICAddr ic_addr;
  ic_addr.type = IDENTIFIER;
  ic_addr.value.symbol = malloc(sizeof(char) * 6);
  strcpy(ic_addr.value.symbol, "LAB");
  return ic_addr;
}


void icLeaf(struct LeafNode *leaf) {
  ICAddr ic_addr;
  ICInstr *ic_instr = NULL;
  
  switch (leaf->type) {
    case NUM:
      ic_addr.value.num = leaf->value.num;
      ic_addr.type = NUM;
      break;
    case RNUM:
      ic_addr.value.rnum = leaf->value.rnum;
      ic_addr.type = RNUM;
      break;
    case TRUE_:
      ic_addr.value.boolean = true;
      ic_addr.type = BOOLEAN_;
      break;
    case FALSE_:
      ic_addr.value.boolean = false;
      ic_addr.type = BOOLEAN_;
      break;
    case IDENTIFIER:
      // ic_addr.value.symbol = symbolTableGet(leaf->scope, leaf->value.entry);
      ic_addr.value.symbol = leaf->value.entry;
      ic_addr.type = IDENTIFIER;
    default:
      break;
  }
  leaf->addr = ic_addr;

  if (ic_instr != NULL) {
    global_ic_instr->next = ic_instr;
    global_ic_instr = ic_instr;
  }
}


// void icArray(struct ArrayNode *array) {
//   ICInstr *ic_instr = NULL;
  
//   icLeaf(array->ptr1);
//   icLeaf(array->ptr2);

//   struct VariableEntry *array_symbol;
//   array_symbol = symbolTableGet(array->ptr1->scope, array->ptr1->value.entry);
//   if (!array_symbol->isStatic) {
//     // dynamically check array bounds
//   }

//   ic_instr = newICInstruction();
//   ic_instr->addr1 = ic_addr;
//   ic_instr->addr2 = ic_addr;
//   ic_instr->addr3 = newTemporary();
//   ic_instr->op = icLOAD;
// }


void icUnaryExpression(struct UNode *unary_expression) {
  icExpression(unary_expression->ptr1);
  
  ICInstr *ic_instr = newICInstruction();
  ic_instr->addr1 = unary_expression->ptr1->addr;
  ic_instr->addr3 = newTemporary();
  switch (unary_expression->op) {
    case PLUS:
      ic_instr->op = icPLUS;
      break;
    case MINUS:
      ic_instr->op = icMINUS;
      break;
    default:
      break;
  }
  unary_expression->addr = ic_instr->addr3;
  
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icLogicalExpression(struct N7Node *logical_expression) {
  icExpression(logical_expression->ptr1);
  icExpression(logical_expression->ptr2);

  ICInstr *ic_instr = newICInstruction();
  ic_instr->addr1 = logical_expression->ptr1->addr;
  ic_instr->addr2 = logical_expression->ptr2->addr;
  ic_instr->addr3 = newTemporary();
  switch (logical_expression->logicalOp) {
    case AND:
      ic_instr->op = icAND;
      break;
    case OR:
      ic_instr->op = icOR;
      break;
    default:
      break;
  }
  logical_expression->addr = ic_instr->addr3;

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

}


void icRelationalExpression(struct N8Node *relational_expression) {
  icExpression(relational_expression->ptr1);
  icExpression(relational_expression->ptr2);

  ICInstr *ic_instr = newICInstruction();
  ic_instr->addr1 = relational_expression->ptr1->addr;
  ic_instr->addr2 = relational_expression->ptr2->addr;
  ic_instr->addr3 = newTemporary();
  switch (relational_expression->relationalOp) {
    case EQ:
      ic_instr->op = icEQ;
      break;
    case NE:
      ic_instr->op = icNE;
      break;
    case LT:
      ic_instr->op = icLT;
      break;
    case GT:
      ic_instr->op = icGT;
      break;
    case LE:
      ic_instr->op = icLE;
      break;
    case GE:
      ic_instr->op = icGE;
      break;
    default:
      break;
  }
  relational_expression->addr = ic_instr->addr3;

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icArithmeticExpression(struct ArithmeticExprNode *arithmetic_expression) {
  if (arithmetic_expression->is_first) {
    icExpression(arithmetic_expression->ptr1);
  }
  icExpression(arithmetic_expression->ptr2);

  ICInstr *ic_instr = newICInstruction();
  ic_instr->addr1 = arithmetic_expression->ptr1->addr;
  ic_instr->addr2 = arithmetic_expression->ptr2->addr;
  ic_instr->addr3 = newTemporary();
  switch (arithmetic_expression->op) {
    case PLUS:
      ic_instr->op = icADD;
      break;
    case MINUS:
      ic_instr->op = icSUB;
      break;
    default:
      break;
  }

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  if (arithmetic_expression->ptr3->type != NULL_NODE) {
    icExpression(arithmetic_expression->ptr3);
  }
  arithmetic_expression->addr = global_ic_instr->addr3;
}


void icTermExpression(struct TermNode *term_expression) {
  if (term_expression->is_first) {
    icExpression(term_expression->ptr1);
  }
  icExpression(term_expression->ptr2);

  ICInstr *ic_instr = newICInstruction();
  ic_instr->addr1 = term_expression->ptr1->addr;
  ic_instr->addr2 = term_expression->ptr2->addr;
  ic_instr->addr3 = newTemporary();
  switch (term_expression->op) {
    case MUL:
      ic_instr->op = icMUL;
      break;
    case DIV:
      ic_instr->op = icDIV;
      break;
    default:
      printf("Default term\n");
      break;
  }

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  if (term_expression->ptr3->type != NULL_NODE) {
    icExpression(term_expression->ptr3);
  }
  term_expression->addr = global_ic_instr->addr3;
}


void icExpression(struct Attribute *expression) {
  switch (expression->type) {
    case U_NODE:
      icUnaryExpression(expression->node.u);
      expression->addr = expression->node.u->addr;
      break;
    case N7_NODE:
      icLogicalExpression(expression->node.n7);
      expression->addr = expression->node.n7->addr;
      break;
    case N8_NODE:
      icRelationalExpression(expression->node.n8);
      expression->addr = expression->node.n8->addr;
      break;
    case ARITHMETIC_EXPR_NODE:
      icArithmeticExpression(expression->node.ari_exp);
      expression->addr = expression->node.ari_exp->addr;
      break;
    case TERM_NODE:
      icTermExpression(expression->node.ter);
      expression->addr = expression->node.ter->addr;
      break;
    case ARRAY_NODE:
      break;
    case LEAF_NODE:
      icLeaf(expression->node.lea);
      expression->addr = expression->node.lea->addr;
      break;
    default:
      printf("Default expression: %d\n", expression->type);
      break;
  }
}


void icAssignmentStatement(struct AssignStmtNode *assignment) {
  switch (assignment->ptr2->type) {
    case LVALUE_ID_NODE:
      icLeaf(assignment->ptr1);
      icExpression(assignment->ptr2->node.lva_id->ptr1);
      assignment->ptr2->addr = assignment->ptr2->node.lva_id->ptr1->addr;
      break;
    case LVALUE_ARR_NODE:
      // icLeaf(assignment->ptr1);
      // icExpression(assignment->ptr2);
      break;
    default:
      break;
  }
  printf("\n");

  ICInstr *ic_instr = newICInstruction();
  ic_instr->op = icSTORE;
  ic_instr->addr1 = assignment->ptr2->addr;
  ic_instr->addr3 = assignment->ptr1->addr;

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icConditionalStatement(struct ConditionalStmtNode *conditional) {
  ICInstr *ic_instr = NULL;
  struct CaseStmtNode *case_statement = NULL;
  ICAddr label_test, label_end;
  ICAddr temporary;

  icLeaf(conditional->ptr1);
  label_end = newLabel();

  case_statement = conditional->ptr2;
  temporary = newTemporary();
  while (case_statement != NULL) {
    label_test = newLabel();

    /* test counter */
    icLeaf(case_statement->ptr1);
    ic_instr = newICInstruction();
    ic_instr->addr1 = conditional->ptr1->addr;
    ic_instr->addr2 = case_statement->ptr1->addr;
    ic_instr->addr3 = temporary;
    ic_instr->op = icSUB;
    global_ic_instr->next = ic_instr;
    global_ic_instr = ic_instr;

    /* conditional jump */
    ic_instr = newICInstruction();
    ic_instr->addr1 = temporary;
    ic_instr->addr2 = label_test;
    ic_instr->op = icJUMPZ;
    global_ic_instr->next = ic_instr;
    global_ic_instr = ic_instr;

    /* case body */
    icStatementList(case_statement->ptr2);

    /* unconditional jump */
    ic_instr = newICInstruction();
    ic_instr->addr1 = label_end;
    ic_instr->op = icJUMP;
    global_ic_instr->next = ic_instr;
    global_ic_instr = ic_instr;

    /* set label */
    ic_instr = newICInstruction();
    ic_instr->addr1 = label_test;
    ic_instr->op = icLABEL;
    global_ic_instr->next = ic_instr;
    global_ic_instr = ic_instr;

    case_statement = case_statement->ptr3;
  }

  /* default body */
  if (conditional->ptr3 != NULL) {
    icStatementList(conditional->ptr3);
  }

  /* set label */
  ic_instr = newICInstruction();
  ic_instr->addr1 = label_end;
  ic_instr->op = icLABEL;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icForIterativeStatement(struct ForIterativeStmtNode *for_iteration) {
  ICInstr *ic_instr = NULL;
  ICAddr label;
  ICAddr temporary;

  /* initialize counter */
  icLeaf(for_iteration->ptr1);
  icLeaf(for_iteration->ptr2->ptr1);
  ic_instr = newICInstruction();
  ic_instr->addr1 = for_iteration->ptr2->ptr1->addr;
  ic_instr->addr3 = for_iteration->ptr1->addr;
  ic_instr->op = icSTORE;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
  
  /* set label */
  label = newLabel();
  ic_instr = newICInstruction();
  ic_instr->addr1 = label;
  ic_instr->op = icLABEL;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  /* loop body */
  icStatementList(for_iteration->ptr3);

  /* increment counter */
  ic_instr = newICInstruction();
  ic_instr->addr1 = for_iteration->ptr1->addr;
  ic_instr->op = icINC;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  /* test counter */
  icLeaf(for_iteration->ptr2->ptr2);
  temporary = newTemporary();
  ic_instr = newICInstruction();
  ic_instr->addr1 = for_iteration->ptr1->addr;
  ic_instr->addr2 = for_iteration->ptr2->ptr2->addr;
  ic_instr->addr3 = temporary;
  ic_instr->op = icSUB;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  /* conditional jump */
  ic_instr = newICInstruction();
  ic_instr->addr1 = temporary;
  ic_instr->addr2 = label;
  ic_instr->op = icJUMPNZ;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icWhileIterativeStatement(struct WhileIterativeStmtNode *while_iteration) {
  ICInstr *ic_instr = NULL;
  ICAddr label_test = newLabel();
  ICAddr label_end = newLabel();

  /* set label test-expression */
  ic_instr = newICInstruction();
  ic_instr->addr1 = label_test;
  ic_instr->op = icLABEL;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  /* test expression */
  icExpression(while_iteration->ptr1);

  /* conditional jump */
  ic_instr = newICInstruction();
  ic_instr->addr1 = while_iteration->ptr1->addr;
  ic_instr->addr2 = label_end;
  ic_instr->op = icJUMPZ;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
  
  /* loop body */
  icStatementList(while_iteration->ptr2);

  /* unconditional jump */
  ic_instr = newICInstruction();
  ic_instr->addr1 = label_test;
  ic_instr->op = icJUMP;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  /* set label end-loop*/
  ic_instr = newICInstruction();
  ic_instr->addr1 = label_end;
  ic_instr->op = icLABEL;
  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icStatement(struct Attribute *attribute_node) {
  switch (attribute_node->type) {
    case ASSIGN_STMT_NODE:
      return icAssignmentStatement(attribute_node->node.agn_stm);
      break;
    case MODULE_REUSE_STMT_NODE:
      // return icModuleReuseStatement(attribute_node->node.mod_reu_stm);
      break;
    case INPUT_NODE:
      // return icInputStatement(attribute_node->node.inp);
      break;
    case PRINT_NODE:
      // return icPrintStatement(attribute_node->node.pri);
      break;
    case DECLARE_STMT_NODE:
      break;
    case CONDITIONAL_STMT_NODE:
      return icConditionalStatement(attribute_node->node.con_stm);
      break;
    case FOR_ITERATIVE_STMT_NODE:
      return icForIterativeStatement(attribute_node->node.for_ite_stm);
      break;
    case WHILE_ITERATIVE_STMT_NODE:
      return icWhileIterativeStatement(attribute_node->node.whi_ite_stm);
      break;
    default:
      break;
  }
}


void icStatementList(struct StatementNode *statement_node) {  
  while(statement_node != NULL) {
    icStatement(statement_node->ptr1);
    statement_node = statement_node->ptr2;
  }
}


void generateIntermediateCode(struct ProgramNode *AST) {
  global_ic_instr = newICInstruction();
  global_ic_instr->addr1 = newLabel();
  global_ic_instr->op = icLABEL;
  start_global_ic_instr = global_ic_instr;
  icStatementList(AST->ptr3);
  // icModuleList(AST->ptr2);
  // icModuleList(AST->ptr4);
}


void printICAddress(ICAddr ic_addr) {
  switch (ic_addr.type) {
    case NUM:
      printf("%d\t", ic_addr.value.num);
      break;
    case RNUM:
      printf("%f\t", ic_addr.value.rnum);
      break;
    case BOOLEAN_:
      if (ic_addr.value.boolean == true)
        printf("True\t");
      else
        printf("False\t");
      break;
    case IDENTIFIER:
      printf("%s\t", (char *)ic_addr.value.symbol);
      break;
    default:
      break;
  }
}


void printICInstruction(ICInstr *ic_instr) {
  switch (ic_instr->op) {
    case icADD:
      // printf("ADD\t");
      // printICAddress(ic_instr->addr3);
      // printICAddress(ic_instr->addr1);
      // printICAddress(ic_instr->addr2);
      cg_ADD_SUB(ic_instr);
      break;
    case icSUB:
      // printf("SUB\t");
      // printICAddress(ic_instr->addr3);
      // printICAddress(ic_instr->addr1);
      // printICAddress(ic_instr->addr2);
      cg_ADD_SUB(ic_instr);
      break;
    case icMUL:
      printf("MUL\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);

      break;
    case icDIV:
      printf("DIV\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      // cg_ADD_SUB_DIV_MUL(ic_instr);
      break;
    case icINC:
      printf("INC\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      break;
    case icDEC:
      printf("DEC\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      break;
    case icAND:
      printf("AND\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icOR:
      printf("OR\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icEQ:
      printf("EQ\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icNE:
      printf("NE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icLT:
      printf("LT\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icGT:
      printf("GT\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icLE:
      printf("LE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icGE:
      printf("GE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icPLUS:
      printf("PLUS\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      break;
    case icMINUS:
      printf("MINUS\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      break;
    // case icCOPY:
    //   printf("COPY\t");
    //   break;
    // case icLOAD:
    //   printf("LOAD\t");
    //   break;
    case icSTORE:
      printf("STORE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      // cgSTORE(ic_instr);
      break;
    case icJUMP:
      printf("JUMP\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      break;
    case icJUMPNZ:
      printf("JUMPNZ\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icJUMPZ:
      printf("JUMPZ\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    // case icCALL:
    //   printf("CALL\t\t\t");
    //   break;
    case icLABEL:
      printf("LABEL\t");
      printICAddress(ic_instr->addr1);
      break;
    default:
      break;
  }
  printf("\n");
}


void printICInstructionList(ICInstr *ic_instr) {
  while (ic_instr != NULL) {
    printICInstruction(ic_instr);
    ic_instr = ic_instr->next;
  }
}
