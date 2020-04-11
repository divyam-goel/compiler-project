#include "intCode.h"


ICInstr *global_ic_instr = NULL;


void generateIntermediateCode(struct ProgramNode *AST) {
  icStatementList(AST->ptr3);
  icModuleList(AST->ptr2);
  icModuleList(AST->ptr4);
}


ICAddr * newICAddress() {
  ICAddr *ic_addr = (ICAddr *) malloc(sizeof(ICAddr));
  return ic_addr;
}


ICInstr * newICInstruction() {
  ICInstr *ic_instr = (ICInstr *) malloc(sizeof(ICInstr));
  return ic_instr;
}


void icLeafNode(struct LeafNode *leaf) {
  ICAddr *ic_addr = newICAddress();
  ICInstr *ic_instr = NULL;
  
  switch (leaf->type) {
    case NUM:
      ic_addr->value.num = leaf->value.num;
      ic_addr->type = NUM;
      break;
    case RNUM:
      ic_addr->value.rnum = leaf->value.rnum;
      ic_addr->type = RNUM;
      break;
    case TRUE_:
      ic_addr->value.boolean = true;
      ic_addr->type = BOOLEAN_;
      break;
    case FALSE_:
      ic_addr->value.boolean = false;
      ic_addr->type = BOOLEAN_;
      break;
    case IDENTIFIER:
      ic_instr->addr3.value.symbol = symbolTableGet(leaf->scope, leaf->value.entry);
      
      ic_instr->addr3.type = IDENTIFIER;
  }

  if (ic_instr != NULL) {
    global_ic_instr->next = ic_instr;
    global_ic_instr = ic_instr;
  }
}


ICInstr * icUnaryExpr(struct UNode *unary_expression) {
  icExpression(unary_expression->ptr1);
  
  ICInstr *ic_instr = newICInstruction();
  ic_instr->addr1 = unary_expression->ptr1->addr;
  ic_instr->addr3 = newTemporary(scope);
  switch (unary_expression->op) {
    case PLUS:
      ic_instr->op = icPLUS;
      break;
    case MINUS:
      ic_instr->op = icMINUS;
      break;
  }

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
  }

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
  }

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icArithmeticlExpression(struct ArithmeticExprNode *arithmetic_expression) {
  if (arithmetic_expression->ptr1->type == LEAF_NODE) {
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
  }

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  if (arithmetic_expression->ptr3->type != NULL_NODE) {
    icExpression(arithmetic_expression->ptr3);
  }
}


void icTermExpression(struct TermNode *term_expression) {
  if (term_expression->ptr1->type == LEAF_NODE) {
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
  }

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;

  if (term_expression->ptr3->type != NULL_NODE) {
    icExpression(term_expression->ptr3);
  }
}


void icExpression(struct Attribute *expression) {
  switch (expression->type) {
    case U_NODE:
      icUnaryExpression(expression->node.u);
      break;
    case N7_NODE:
      icLogicalExpression(expression->node.n7);
      break;
    case N8_NODE:
      icRelationalExpression(expression->node.n8);
      break;
    case ARITHMETIC_EXPR_NODE:
      icArithmeticExpression(expression->node.ari_exp);
      break;
    case TERM_NODE:
      icTermExpression(expression->node.ter);
      break;
    case ARRAY_NODE:
      break;
    case LEAF_NODE:
      break;
    default:
      printf("Invalid Expression Type %d!\n", expression->type);
      break;
  }
}


void icAssignmentStatement(struct AssignStmtNode *assignment) {
  icLeaf(assignment->ptr1);
  icExpression(assignment->ptr2);

  ICInstr *ic_instr = newICInstruction();
  ic_instr->op = STORE;
  ic_instr->addr1 = assignment->ptr2->addr;
  ic_instr->addr3 = assignment->ptr1->addr;
  ic_instr->next = assignment->ptr1->code;

  global_ic_instr->next = ic_instr;
  global_ic_instr = ic_instr;
}


void icStatement(struct Attribute *attribute_node) {
  switch (attribute_node->type) {
    case ASSIGN_STMT_NODE:
      return icAssignmentStatement(attribute_node->node.agn_stm);
      break;
    case MODULE_REUSE_STMT_NODE:
      return icModuleReuseStatement(attribute_node->node.mod_reu_stm);
      break;
    case INPUT_NODE:
      return icInputStatement(attribute_node->node.mod_reu_stm);
      break;
    case PRINT_NODE:
      return icPrintStatement(attribute_node->node.mod_reu_stm);
      break;
    case DECLARE_STMT_NODE:
      return icDeclareStatement(attribute_node->node.mod_reu_stm);
      break;
    case CONDITIONAL_STMT_NODE:
      return icConditionalStatement(attribute_node->node.mod_reu_stm);
      break;
    case FOR_ITERATIVE_STMT_NODE:
      return icForIterativeStatement(attribute_node->node.mod_reu_stm);
      break;
    case WHILE_ITERATIVE_STMT_NODE:
      return icWhileIterativeStatement(attribute_node->node.mod_reu_stm);
      break;
  }
}


void icStatementList(struct StatementNode *statement_node) {  
  while(statement_node != NULL) {
    icStatement(statement_node->ptr1);
    statement_node = statement_node->ptr2;
  }
}