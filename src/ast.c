#include "ast.h"

extern struct parseTree PT;

void traverseChildren(struct treeNode *curr_node) {
  while(curr_node != NULL) {
    traverseParseTree(curr_node);
    curr_node = curr_node->next;
  }
}


void traverseParseTree(struct treeNode *curr_node) {
  struct treeNode *child_node, *next_node;

  // rule number which the current non-terminal derives
  int rule_num = curr_node->rule_number;

  switch(rule_num) {

    case 1: // <program> := <moduleDeclarations> <otherModules> <driverModule> <otherModules>
      traverseChildren(curr_node->child_node);
      // define and populate a ProgramNode
      struct ProgramNode *prog_node = (struct ProgramNode) malloc(sizeof(ProgramNode))
      child_node = curr_node->child;
      prog_node->ptr1 = child_node->syn;
      child_node = child_node->next;
      prog_node->ptr2 = next_node->syn;
      child_node = child_node->next;
      prog_node->ptr3 = child_node->syn;
      child_node = child_node->next;
      prog_node->ptr4 = child_node->syn;
      break;

    case 2: // <moduleDeclarations> := <moduleDeclaration> <moduleDeclarations>
      traverseChildren(curr_node->child_node);
      /// define and populate ModuleDeclarationNode
      struct ModuleDeclarationNode *mod_dec_node = (struct ModuleDeclarationNode) malloc(sizeof(ModuldeDeclarationNode));
      child_node = curr_node->child_node;
      mod_dec_node->ptr1 = child_node->syn;
      child_node = child_node->next;
      mod_dec_node->ptr2 = next_node->syn;
      break;

    case 03: // <moduleDeclarations> := EPSILON
      traverseChildren();
      break;

    case 04: // <moduleDeclaration> := DECLARE MODULE ID SEMICOL
    case 05: // <otherModules>  := <module> <otherModules>
    case 06: // <otherModules>  := EPSILON
    case 07: // <driverModule> := DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
    case 08: // <module> := DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
    case 09: // <ret> := RETURNS SQBO <output_plist> SQBC SEMICOL
    case 10: // <ret> := EPSILON
    case 11: // <input_plist> := ID COLON <dataType> <sub_input_plist>
    case 12: // <sub_input_plist> := COMMA ID COLON <dataType> <sub_input_plist>
    case 13: // <sub_input_plist> := EPSILON
    case 14: // <output_plist> := ID COLON <type> <sub_output_plist>
    case 15: // <sub_output_plist> := COMMA ID COLON <type> <sub_output_plist>
    case 16: // <sub_output_plist> := EPSILON
    case 17: // <dataType> := INTEGER
    case 18: // <dataType> := REAL
    case 19: // <dataType> := BOOLEAN
    case 20: // <dataType> := ARRAY SQBO <dynamic_range> SQBC OF <type>
    case 21: // <dynamic_range> := <index> RANGEOP <index>
    case 22: // <type> := INTEGER
    case 23: // <type> := REAL
    case 24: // <type> := BOOLEAN
    case 25: // <moduleDef> := START <statements> END
    case 26: // <statements> := <statement> <statements>
    case 27: // <statements> := EPSILON
    case 28: // <statement> := <ioStmt>
    case 29: // <statement> := <simpleStmt>
    case 30: // <statement> := <declareStmt>
    case 31: // <statement> := <conditionalStmt>
    case 32: // <statement> := <iterativeStmt>
    case 33: // <ioStmt> := GET_VALUE BO ID BC SEMICOL
    case 34: // <ioStmt> := PRINT BO <extended_var> BC SEMICOL
    case 35: // <boolConstt> := TRUE
    case 36: // <boolConstt> := FALSE
    case 37: // <extended_var>  := <var>
    case 38: // <extended_var> := <boolConstt>
    case 39: // <var> := ID <whichId>
    case 40: // <var> := NUM
    case 41: // <var> := RNUM
    case 42: // <whichId> := SQBO <index> SQBC
    case 43: // <whichId> := EPSILON
    case 44: // <simpleStmt> := <assignmentStmt>
    case 45: // <simpleStmt> := <moduleReuseStmt>
    case 46: // <assignmentStmt> := ID <whichStmt>
    case 47: // <whichStmt> := <lvalueIDStmt>
    case 48: // <whichStmt> := <lvalueARRStmt>
    case 49: // <lvalueIDStmt> := ASSIGNOP <new_expression> SEMICOL
    case 50: // <lvalueARRStmt> := SQBO <index> SQBC ASSIGNOP <new_expression> SEMICOL
    case 51: // <index> := NUM
    case 52: // <index> := ID
    case 53: // <moduleReuseStmt> := <optional> USE MODULE ID WITH PARAMETERS <idList> SEMICOL
    case 54: // <optional> := SQBO <idList> SQBC ASSIGNOP
    case 55: // <optional> := EPSILON
    case 56: // <idList> := ID <sub_idList>
    case 57: // <sub_idList> := COMMA ID <sub_idList>1
    case 58: // <sub_idList> := EPSILON
    case 59: // <new_expression> := <u>
    case 60: // <new_expression> := <expression>
    case 61: // <u> := PLUS <sub_u>
    case 62: // <u> := MINUS <sub_u>
    case 63: // <sub_u> := BO <arithmeticExpr> BC
    case 64: // <sub_u> := <var>
    case 65: // <expression> := <AnyTerm> <N7>
    case 66: // <N7> := <logicalOp> <AnyTerm> <N7>
    case 67: // <N7> := EPSILON
    case 68: // <AnyTerm> := <arithmeticExpr> <N8>
    case 69: // <AnyTerm> := <boolConstt>
    case 70: // <N8> := <relationalOp> <arithmeticExpr>
    case 71: // <N8> := EPSILON
    case 72: // <arithmeticExpr> := <term> <sub_arithmeticExpr>
    case 73: // <sub_arithmeticExpr> := <op1> <term> <sub_arithmeticExpr>
    case 74: // <sub_arithmeticExpr> := EPSILON
    case 75: // <term> :=  <factor> <sub_term>
    case 76: // <sub_term> := <op2> <factor> <sub_term>
    case 77: // <sub_term> := EPSILON
    case 78: // <factor> := BO <expression> BC
    case 79: // <factor> := <var>
    case 80: // <op1> := PLUS
    case 81: // <op1> := MINUS
    case 82: // <op2> := MUL
    case 83: // <op2> := DIV
    case 84: // <logicalOp> := AND
    case 85: // <logicalOp> := OR
    case 86: // <relationalOp> := LT
    case 87: // <relationalOp> := LE
    case 88: // <relationalOp> := GT
    case 89: // <relationalOp> := GE
    case 90: // <relationalOp> := EQ
    case 91: // <relationalOp> := NE
    case 92: // <declareStmt> := DECLARE <idList> COLON <dataType> SEMICOL
    case 93: // <conditionalStmt> := SWITCH BO ID BC START <caseStmt> <default> END
    case 94: // <caseStmt> := CASE <value> COLON <statements> BREAK SEMICOL <nullableCaseStmt>
    case 95: // <nullableCaseStmt> := CASE <value> COLON <statements> BREAK SEMICOL <nullableCaseStmt>1
    case 96: // <nullableCaseStmt> := EPSILON
    case 97: // <value> := NUM
    case 98: // <value> := TRUE
    case 99: // <value> := FALSE
    case 100: // <default> := DEFAULT COLON <statements> BREAK SEMICOL
    case 101: // <default> :=  EPSILON
    case 102: // <iterativeStmt> := FOR BO ID IN <range> BC START <statements> END
    case 103: // <iterativeStmt> := WHILE BO <expression> BC START <statements> END
    case 104: // <range> := NUM RANGEOP NUM

  }
}

void createAST() {
  traverseParseTree(PT.head)
}
