#include "ast.h"

extern struct parseTree PT;
struct ProgramNode AST;

/* BEGIN : Function Declarations */
void traverseChildren(struct treeNode *curr_node);
struct treeNode *nextNonTerminalNode(struct treeNode *curr_node);
struct LeafNode *newLeafNode(int type, void *data);
/* END : Function Declarations */


void traverseParseTree(struct treeNode *curr_node) {
  if (curr_node->flag == TERMINAL) {
    return;
  }

  int rule_num = curr_node->rule_number;
  switch(rule_num) {

    case 1: // <program> := <moduleDeclarations> <otherModules> <driverModule> <otherModules>
      case_1(curr_node);
      break;

    case 2: // <moduleDeclarations> := <moduleDeclaration> <moduleDeclarations>
      case_2(curr_node);
      break;

    case 3: // <moduleDeclarations> := EPSILON
      case_3(curr_node);
      break;

    case 4: // <moduleDeclaration> := DECLARE MODULE ID SEMICOL
      case_4(curr_node);
      break;

    case 5: // <otherModules>  := <module> <otherModules>
      case_5(curr_node);
      break;

    case 6: // <otherModules>  := EPSILON
      case_6(curr_node);
      break;

    case 7: // <driverModule> := DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
      case_7(curr_node);
      break;

    case 8: // <module> := DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
      case_8(curr_node);
      break;

    case 9: // <ret> := RETURNS SQBO <output_plist> SQBC SEMICOL
      case_9(curr_node);
      break;

    case 10: // <ret> := EPSILON
      case_10(curr_node);
      break;

    case 11: // <input_plist> := ID COLON <dataType> <sub_input_plist>
      case_11(curr_node);
      break;

    case 12: // <sub_input_plist> := COMMA ID COLON <dataType> <sub_input_plist>
      case_12(curr_node);
      break;

    case 13: // <sub_input_plist> := EPSILON
      case_13(curr_node);
      break;

    case 14: // <output_plist> := ID COLON <type> <sub_output_plist>
      case_14(curr_node);
      break;

    case 15: // <sub_output_plist> := COMMA ID COLON <type> <sub_output_plist>
      case_15(curr_node);
      break;

    case 16: // <sub_output_plist> := EPSILON
      case_16(curr_node);
      break;

    case 17: // <dataType> := INTEGER
      case_17(curr_node);
      break;

    case 18: // <dataType> := REAL
      case_18(curr_node);
      break;

    case 19: // <dataType> := BOOLEAN
      case_19(curr_node);
      break;

    case 20: // <dataType> := ARRAY SQBO <dynamic_range> SQBC OF <type>
      case_20(curr_node);
      break;

    case 21: // <dynamic_range> := <index> RANGEOP <index>
      case_21(curr_node);
      break;

    case 22: // <type> := INTEGER
      case_22(curr_node);
      break;

    case 23: // <type> := REAL
      case_23(curr_node);
      break;

    case 24: // <type> := BOOLEAN
      case_24(curr_node);
      break;

    case 25: // <moduleDef> := START <statements> END
      case_25(curr_node);
      break;

    case 26: // <statements> := <statement> <statements>
      case_26(curr_node);
      break;

    case 27: // <statements> := EPSILON
      case_27(curr_node);
      break;

    case 28: // <statement> := <ioStmt>
      case_28(curr_node);
      break;

    case 29: // <statement> := <simpleStmt>
      case_29(curr_node);
      break;

    case 30: // <statement> := <declareStmt>
      case_30(curr_node);
      break;

    case 31: // <statement> := <conditionalStmt>
      case_31(curr_node);
      break;

    case 32: // <statement> := <iterativeStmt>
      case_32(curr_node);
      break;

    case 33: // <ioStmt> := GET_VALUE BO ID BC SEMICOL
      case_33(curr_node);
      break;

    case 34: // <ioStmt> := PRINT BO <extended_var> BC SEMICOL
      case_34(curr_node);
      break;

    case 35: // <boolConstt> := TRUE
      case_35(curr_node);
      break;

    case 36: // <boolConstt> := FALSE
      case_36(curr_node);
      break;

    case 37: // <extended_var>  := <var>
      case_37(curr_node);
      break;

    case 38: // <extended_var> := <boolConstt>
      case_38(curr_node);
      break;

    case 39: // <var> := ID <whichId>
      case_39(curr_node);
      break;

    case 40: // <var> := NUM
      case_40(curr_node);
      break;

    case 41: // <var> := RNUM
      case_41(curr_node);
      break;

    case 42: // <whichId> := SQBO <index> SQBC
      case_42(curr_node);
      break;

    case 43: // <whichId> := EPSILON
      case_43(curr_node);
      break;

    case 44: // <simpleStmt> := <assignmentStmt>
      case_44(curr_node);
      break;

    case 45: // <simpleStmt> := <moduleReuseStmt>
      case_45(curr_node);
      break;

    case 46: // <assignmentStmt> := ID <whichStmt>
      case_46(curr_node);
      break;

    case 47: // <whichStmt> := <lvalueIDStmt>
      case_47(curr_node);
      break;

    case 48: // <whichStmt> := <lvalueARRStmt>
      case_48(curr_node);
      break;

    case 49: // <lvalueIDStmt> := ASSIGNOP <new_expression> SEMICOL
      case_49(curr_node);
      break;

    case 50: // <lvalueARRStmt> := SQBO <index> SQBC ASSIGNOP <new_expression> SEMICOL
      case_50(curr_node);
      break;

    case 51: // <index> := NUM
      case_51(curr_node);
      break;

    case 52: // <index> := ID
      case_52(curr_node);
      break;

    case 53: // <moduleReuseStmt> := <optional> USE MODULE ID WITH PARAMETERS <idList> SEMICOL
      case_53(curr_node);
      break;

    case 54: // <optional> := SQBO <idList> SQBC ASSIGNOP
      case_54(curr_node);
      break;

    case 55: // <optional> := EPSILON
      case_55(curr_node);
      break;

    case 56: // <idList> := ID <sub_idList>
      case_56(curr_node);
      break;

    case 57: // <sub_idList> := COMMA ID <sub_idList>1
      case_57(curr_node);
      break;

    case 58: // <sub_idList> := EPSILON
      case_58(curr_node);
      break;

    case 59: // <new_expression> := <u>
      case_59(curr_node);
      break;

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
    default:
      break;  // TEMP
  }
}


void case_1(struct treeNode *curr_node) {
  /* <program> := <moduleDeclarations> <otherModules> <driverModule> <otherModules> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct ProgramNode *pro = (struct ProgramNode *) malloc(sizeof(struct ProgramNode));
  pro->ptr1 = child_node->syn.node.mod_dec;
  child_node = nextNonTerminalNode(child_node);
  pro->ptr2 = child_node->syn.node.oth_mod;
  child_node = nextNonTerminalNode(child_node);
  pro->ptr3 = child_node->syn.node.stm;
  child_node = nextNonTerminalNode(child_node);
  pro->ptr4 = child_node->syn.node.oth_mod;

  /* <program>.syn = new ProgramNode(<moduleDeclarations>.syn, <otherModules>.syn, <driverModule>.syn, <otherModules>1.syn) */
  curr_node->syn.node.pro = pro;
  curr_node->syn.type = PROGRAM_NODE;
}


void case_2(struct treeNode *curr_node) {
  /* <moduleDeclarations> := <moduleDeclaration> <moduleDeclarations> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct ModuleDeclarationNode *mod_dec_node = (struct ModuleDeclarationNode *) malloc(sizeof(struct ModuleDeclarationNode));
  mod_dec_node->ptr1 = child_node->syn.node.lea;
  child_node = nextNonTerminalNode(child_node);
  mod_dec_node->ptr2 = child_node->syn.node.mod_dec;

  /* <moduleDeclarations>.syn = new ModuleDeclarationNode(<moduleDeclaration>.syn, <moduleDeclarations>1.syn) */
  curr_node->syn.node.mod_dec = mod_dec_node;
  curr_node->syn.type = MODULE_DECLARATION_NODE;
}


void case_3(struct treeNode *curr_node) {
  /* <moduleDeclarations> := EPSILON */

  /* <moduleDeclarations>.syn = NULL */
  curr_node->syn.node.mod_dec = NULL;
  curr_node->syn.type = MODULE_DECLARATION_NODE;
}


void case_4(struct treeNode *curr_node) {
  /* <moduleDeclaration> := DECLARE MODULE ID SEMICOL */
  struct treeNode *id = curr_node->child->next->next;

  /* <moduleDeclaration>.syn = new LeafNode(ID, ID.entry) */
  curr_node->syn.node.lea = newLeafNode(IDENTIFIER, id->symbol.terminal.lexeme.str);
  curr_node->syn.type = LEAF_NODE;
}


void case_5(struct treeNode *curr_node) {
  /* <otherModules>  := <module> <otherModules> */

  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct OtherModuleNode *oth_mod_node = (struct OtherModuleNode *) malloc(sizeof(struct OtherModuleNode));
  oth_mod_node->ptr1 = child_node->syn.node.mod;
  child_node = nextNonTerminalNode(child_node);
  oth_mod_node->ptr2 = child_node->syn.node.oth_mod;

  /* <otherModules>.syn = new OtherModuleNode(<module>.syn, <otherModules>.syn) */
  curr_node->syn.node.oth_mod = oth_mod_node;
  curr_node->syn.type = OTHER_MODULE_NODE;
}


void case_6(struct treeNode *curr_node) {
  /* <otherModules>  := EPSILON */

  /* <otherModules>.syn = NULL */
  curr_node->syn.node.oth_mod = NULL;
  curr_node->syn.type = OTHER_MODULE_NODE;
}


void case_7(struct treeNode *curr_node) {
  /* <driverModule> := DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef> */

  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  /* <driverModule>.syn = <moduleDef>.syn */
  curr_node->syn = child_node->syn;
}


void case_8(struct treeNode *curr_node) {
  /* <module> := DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef> */

  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* define and populate structure ModuleNode */
  struct ModuleNode *module_node = (struct ModuleNode *) malloc(sizeof(struct ModuleNode));
  child_node = child_node->next->next;
  module_node->ptr1 = newLeafNode(IDENTIFIER, child_node);
  child_node = nextNonTerminalNode(child_node);
  module_node->ptr2 = child_node->syn.node.inp_pli;
  child_node = nextNonTerminalNode(child_node);
  module_node->ptr3 = child_node->syn.node.out_pli;
  child_node = nextNonTerminalNode(child_node);
  module_node->ptr4 = child_node->syn.node.stm;

  /* <module>.syn = new ModuleNode(new LeafNode(ID, ID.entry), <input_plist>.syn, <ret>.syn, <moduleDef>.syn) */
  curr_node->syn.node.mod = module_node;
  curr_node->syn.type = MODULE_NODE;
}


void case_9(struct treeNode *curr_node) {
  /* <ret> := RETURNS SQBO <output_plist> SQBC SEMICOL */

  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  /* <ret>.syn = <output_plist>.syn */
  curr_node->syn = child_node->syn;
}


void case_10(struct treeNode *curr_node) {
  /* <ret> := EPSILON */

  /* <ret>.syn = NULL */
  curr_node->syn.node.out_pli = NULL;
  curr_node->syn.type = OUTPUT_PLIST_NODE;
}


void case_11(struct treeNode *curr_node) {
  /* <input_plist> := ID COLON <dataType> <sub_input_plist> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct InputPlistNode *inp_pli = (struct InputPlistNode *) malloc(sizeof(struct InputPlistNode));
  inp_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);
  child_node = nextNonTerminalNode(child_node);
  inp_pli->ptr2 = &(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  inp_pli->ptr3 = child_node->syn.node.inp_pli;

  /* <input_plist>.syn = new InputPlistNode(new LeafNode(ID, ID.entry), <dataType>.syn, <sub_input_plist>.syn) */
  curr_node->syn.node.inp_pli = inp_pli;
  curr_node->syn.type = INPUT_PLIST_NODE;
}


void case_12(struct treeNode *curr_node) {
  /* <sub_input_plist> := COMMA ID COLON <dataType> <sub_input_plist> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  struct InputPlistNode *inp_pli = (struct InputPlistNode *) malloc(sizeof(struct InputPlistNode));
  inp_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);
  child_node = nextNonTerminalNode(child_node);
  inp_pli->ptr2 = &(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  inp_pli->ptr3 = child_node->syn.node.inp_pli;

  /* <sub_input_plist>.syn = new InputPlistNode(new LeafNode(ID, ID.entry), <dataType>.syn, <sub_input_plist>1.syn) */
  curr_node->syn.node.inp_pli = inp_pli;
  curr_node->syn.type = INPUT_PLIST_NODE;
}


void case_13(struct treeNode *curr_node) {
  /* <sub_input_plist> := EPSILON */

  /* <sub_input_plist>.syn = NULL */
  curr_node->syn.node.inp_pli = NULL;
  curr_node->syn.type = INPUT_PLIST_NODE;
}


void case_14(struct treeNode *curr_node) {
  /* <output_plist> := ID COLON <type> <sub_output_plist> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct OutputPlistNode *out_pli = (struct OutputPlistNode *) malloc(sizeof(struct OutputPlistNode));
  out_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);
  child_node = nextNonTerminalNode(child_node);
  out_pli->ptr2 = &(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  out_pli->ptr3 = child_node->syn.node.out_pli;
   
  /* <output_plist>.syn = new OutputPlistNode(new LeafNode(ID, ID.entry), <type>.syn, <sub_output_plist>.syn) */
  curr_node->syn.node.out_pli = out_pli;
  curr_node->syn.type = OUTPUT_PLIST_NODE;
}


void case_15(struct treeNode *curr_node) {
  /* <sub_output_plist> := COMMA ID COLON <dataType> <sub_output_plist> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  struct OutputPlistNode *out_pli = (struct OutputPlistNode *) malloc(sizeof(struct OutputPlistNode));
  out_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);
  child_node = nextNonTerminalNode(child_node);
  out_pli->ptr2 = &(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  out_pli->ptr3 = child_node->syn.node.out_pli;

  /* <sub_output_plist>.syn = new OutputPlistNode(new LeafNode(ID, ID.entry), <dataType>.syn, <sub_output_plist>1.syn) */
  curr_node->syn.node.out_pli = out_pli;
  curr_node->syn.type = OUTPUT_PLIST_NODE;
}


void case_16(struct treeNode *curr_node) {
  /* <sub_out_plist> := EPSILON */

  /* <sub_output_plist>.syn = NULL */
  curr_node->syn.node.out_pli = NULL;
  curr_node->syn.type = OUTPUT_PLIST_NODE;
}

// TODO:
void case_17(struct treeNode *curr_node) {
  /* <dataType> := INTEGER */

  /* <dataType>.syn = new LeafNode(TYPE, “INT”) */
}


// TODO:
void case_18(struct treeNode *curr_node) {
  /* <dataType> := REAL */

  /* <dataType>.syn = new LeafNode(TYPE, “REAL”) */
}


// TODO:
void case_19(struct treeNode *curr_node) {
  /* <dataType> := BOOLEAN */

  /* <dataType>.syn = new LeafNode(TYPE, “BOOL”) */
}


void case_20(struct treeNode *curr_node) {
  /* <dataType> := ARRAY SQBO <dynamic_range> SQBC OF <type> */ 
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct ArrayTypeNode *arr_typ = (struct ArrayTypeNode *) malloc(sizeof(struct ArrayTypeNode));
  child_node = nextNonTerminalNode(child_node);
  arr_typ->ptr2 = child_node->syn.node.dyn_ran;
  child_node = nextNonTerminalNode(child_node);
  arr_typ->ptr1 = child_node->syn.node.lea;

  /* <dataType>.syn = new ArrayTypeNode(<type>.syn, <dynamic_range>.syn) */
  curr_node->syn.type = ARRAY_TYPE_NODE;
  curr_node->syn.node.arr_typ = arr_typ; 
}


void case_21(struct treeNode *curr_node) {
  /* <dynamic_range> := <index> RANGEOP <index> */
  struct treeNode *child_node = curr_node->child;

  struct DynamicRangeNode *dyn_ran = (struct DynamicRangeNode *) malloc(sizeof(struct DynamicRangeNode));
  dyn_ran->ptr1 = child_node->syn.node.lea;
  child_node = nextNonTerminalNode(child_node);
  dyn_ran->ptr2 = child_node->syn.node.lea;

  /* <dynamic_range>.syn = new DynamicRangeNode(<index>1.syn, <index>2.syn) */
  curr_node->syn.type = DYNAMIC_RANGE_NODE;
  curr_node->syn.node.dyn_ran = dyn_ran;
}


// TODO:
void case_22(struct treeNode *curr_node) {
  /* <type> := INTEGER */
  
  /* <type>.syn = new LeafNode(TYPE, “INT”) */
}


// TODO:
void case_23(struct treeNode *curr_node) {
  /* <type> := REAL */

  /* <type>.syn = new LeafNode(TYPE, “REAL”) */
}


// TODO:
void case_24(struct treeNode *curr_node) {
  /* <type> := BOOLEAN */

  /* <type>.syn = new LeafNode(TYPE, “BOOL”) */
}

void case_25(struct treeNode *curr_node) {
  /* <moduleDef> := START <statements> END */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  /* <moduleDef>.syn = <statements>.syn */
  curr_node->syn = child_node->syn;
}


void case_26(struct treeNode *curr_node) {
  /* <statements> := <statement> <statements> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct StatementNode *stm = (struct StatementNode *) malloc(sizeof(struct StatementNode));
  stm->ptr1 = &(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  stm->ptr2 = child_node->syn.node.stm;

  /* <statements>.syn = new StatementNode(<statement>.syn, <statements>.syn) */
  curr_node->syn.type = STATEMENT_NODE;
  curr_node->syn.node.stm = stm;
}


void case_27(struct treeNode *curr_node) {
  /* <statements> := EPSILON */

  /* <statements>.syn = NULL */
  curr_node->syn.type = STATEMENT_NODE;
  curr_node->syn.node.stm = NULL;
}


void case_28(struct treeNode *curr_node) {
  /* <statement> := <ioStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <ioStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_29(struct treeNode *curr_node) {
  /* <statement> := <simpleStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <simpleStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_30(struct treeNode *curr_node) {
  /* <statement> := <declareStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <declareStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_31(struct treeNode *curr_node) {
  /* <statement> := <conditionalStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <conditionalStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_32(struct treeNode *curr_node) {
  /* <statement> := <iterativeStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <iterativeStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_33(struct treeNode *curr_node) {
  /* <ioStmt> := GET_VALUE BO ID BC SEMICOL */
  struct treeNode *child_node = curr_node->child;

  struct InputNode *input_node = (struct InputNode *) malloc(sizeof(struct InputNode));
  child_node = child_node->next->next;
  input_node->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);

  /* <ioStmt>.syn = new InputNode(new LeafNode(ID, ID.entry)) */
  curr_node->syn.node.inp = input_node;
  curr_node->syn.type = INPUT_NODE;
}


void case_34(struct treeNode *curr_node) {
  /* <ioStmt> := PRINT BO <extended_var> BC SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct PrintNode *print_node = (struct PrintNode *) malloc(sizeof(struct PrintNode));
  child_node = child_node->next->next;
  print_node->ptr1->type = LEAF_NODE;
  print_node->ptr1->node.lea = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);

  /* <ioStmt>.syn = new PrintNode(<extended_var>.syn) */
  curr_node->syn.node.pri = print_node;
  curr_node->syn.type = PRINT_NODE;
}


// TODO: This suffers from a similar issue as with dataType and type.
void case_35(struct treeNode *curr_node) {
  /* <boolConstt> := TRUE */

  /* <boolConstt>.syn = new LeafNode(BOOL, “TRUE”) */
}


// TODO: This suffers from a similar issue as with dataType and type.
void case_36(struct treeNode *curr_node) {
  /* <boolConstt> := FALSE */

  /* <boolConstt>.syn = new LeafNode(BOOL, “FALSE”) */
}


void case_37(struct treeNode *curr_node) {
  /*  <extended_var>  := <var> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <extended_var>.syn = <var>.syn */
  curr_node->syn = child_node->syn;
}


void case_38(struct treeNode *curr_node) {
  /* <extended_var> := <boolConstt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <extended_var>.syn = <boolConstt>.syn */
  curr_node->syn = child_node->syn;
}


void case_39(struct treeNode *curr_node) {
  /* <var> := ID <whichId> */
  struct treeNode *id_node = curr_node->child;
  struct treeNode *whichid_node = id_node->next;
  traverseChildren(id_node);

  /* IF (<whichId>.syn == NULL)
   *   <var>.syn = new LeafNode(ID, ID.entry)
   * ELSE
   *   <var>.syn = new ArrayNode(new LeafNode(ID, ID.entry), <whichId>.syn) */
  if (whichid_node == NULL) {
    curr_node->syn.type = LEAF_NODE;
    curr_node->syn.node.lea = newLeafNode(IDENTIFIER, id_node->symbol.terminal.lexeme.str);
  }
  else {
    struct ArrayNode *arr = (struct ArrayNode *) malloc(sizeof(struct ArrayNode));
    arr->ptr1 = newLeafNode(IDENTIFIER, id_node->symbol.terminal.lexeme.str);
    arr->ptr2 = whichid_node->syn.node.lea;
    curr_node->syn.type = ARRAY_NODE;
    curr_node->syn.node.arr = arr;
  }
}


void case_40(struct treeNode *curr_node) {
  /* <var> := NUM */
  struct treeNode *child_node = curr_node->child;

  /* <var>.syn = new LeafNode(NUM, NUM.val) */
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num));
  curr_node->syn.type = LEAF_NODE;
}


void case_41(struct treeNode *curr_node) {
  /* <var> := RNUM */
  struct treeNode *child_node = curr_node->child;

  /* <var>.syn = new LeafNode(RNUM, RNUM.val) */
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.rnum));
  curr_node->syn.type = LEAF_NODE;
}


void case_42(struct treeNode *curr_node) {
  /* <whichId> := SQBO <index> SQBC */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  child_node = nextNonTerminalNode(child_node);

  /* <whichId>.syn = <index>.syn */
  curr_node->syn.node.lea = child_node->syn.node.lea;
  curr_node->syn.type = LEAF_NODE;
}



void case_43(struct treeNode *curr_node) {
   /* <whichId> := EPSILON */

  /* <whichId>.syn = NULL */
  curr_node->syn.node.lea = NULL;  /* This line *does* seem a little unnecessary. */
  curr_node->syn.type = NULL_NODE;
}


void case_44(struct treeNode *curr_node) {
  /* <simpleStmt> := <assignmentStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  /* <simpleStmt>.syn = <assignmentStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_45(struct treeNode *curr_node) {
  /* <simpleStmt> := <moduleReuseStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  /* <simpleStmt>.syn = <moduleReuseStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_46(struct treeNode *curr_node) {
  /* <assignmentStmt> := ID <whichStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  /* <assignmentStmt>.syn = new AssignStmtNode(new LeafNode(ID, ID.entry), <whichStmt>.syn) */
  struct AssignStmtNode *agn_stm = (struct AssignStmtNode *) malloc(sizeof(struct AssignStmtNode));
  agn_stm->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);
  child_node = child_node->next;
  agn_stm->ptr2 = &(child_node->syn);
  
  curr_node->syn.type = ASSIGN_STMT_NODE;
  curr_node->syn.node.agn_stm = agn_stm;
}


void case_47(struct treeNode *curr_node) {
  /* <whichStmt> := <lvalueIDStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  /* <whichStmt>.syn = <lvalueIDStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_48(struct treeNode *curr_node) {
  /* <whichStmt> := <lvalueARRStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  
  /* <whichStmt>.syn = <lvalueARRStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_49(struct treeNode *curr_node) {
  /* <lvalueIDStmt> := ASSIGNOP <new_expression> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <lvalueIDStmt>.syn = new LvalueIDNode(<new_expression>.syn) */
  struct LvalueIDNode *lva_id = (struct LvalueIDNode *) malloc(sizeof(struct LvalueIDNode));
  lva_id->ptr1 = &(child_node->syn);

  curr_node->syn.node.lva_id = lva_id;
  curr_node->syn.type = LVALUE_ID_NODE;
}


void case_50(struct treeNode *curr_node) {
  /* <lvalueARRStmt> := SQBO <index> SQBC ASSIGNOP <new_expression> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  child_node = nextNonTerminalNode(child_node);

  
  /* <lvalueARRStmt>.syn = new LvalueARRNode(<index>.syn, <new_expression>.syn) */
  struct LvalueARRNode *lva_arr = (struct LvalueARRNode *) malloc(sizeof(struct LvalueARRNode));
  lva_arr->ptr1 = child_node->syn.node.lea;
  child_node = nextNonTerminalNode(child_node);
  lva_arr->ptr2 = &(child_node->syn);

  curr_node->syn.type = LVALUE_ARR_NODE,
  curr_node->syn.node.lva_arr = lva_arr;
}


void case_51(struct treeNode *curr_node) {
  /* <index> := NUM  */
  struct treeNode *child_node = curr_node->child;

  /* <index>.syn = new LeafNode(NUM, NUM.val) */
  curr_node->syn.type = LEAF_NODE;
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num));
}


void case_52(struct treeNode *curr_node) {
  /* <index> := ID */
  struct treeNode *child_node = curr_node->child;
  
  /* <index>.syn = new LeafNode(ID, ID.entry) */
  curr_node->syn.type = LEAF_NODE;
  curr_node->syn.node.lea = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str));
}


void case_53(struct treeNode *curr_node) {
  /* <moduleReuseStmt> := <optional> USE MODULE ID WITH PARAMETERS <idList> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <moduleReuseStmt>.syn = new ModuleReuseStmtNode(<optional>.syn, <idList>.syn) */
  struct ModuleReuseStmtNode *mod_reu_stm = (struct ModuleReuseStmtNode *) malloc(sizeof(struct ModuleReuseStmtNode));
  mod_reu_stm->ptr1 = child_node->syn.node.id_lis;
  child_node = nextNonTerminalNode(child_node);
  mod_reu_stm->ptr2 = child_node->syn.node.id_lis;

  curr_node->syn.type = MODULE_REUSE_STMT_NODE;
  curr_node->syn.node.mod_reu_stm = mod_reu_stm;
}


void case_54(struct treeNode *curr_node) {
  /* <optional> := SQBO <idList> SQBC ASSIGNOP */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <optional>.syn = <idList>.syn */
  curr_node->syn.type = child_node->syn.type;
  curr_node->syn.node = child_node->syn.node;
}


void case_55(struct treeNode *curr_node) {
  /* <optional> := EPSILON */

  curr_node->syn.type = NULL_NODE;
  memset(&(curr_node->syn.node), 0, sizeof(union ASTNodesUnion));
}


void case_56(struct treeNode *curr_node){
  /* <idList> := ID <sub_idList> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <idList>.syn = new IdListNode(new LeafNode(ID, ID.entry), <sub_idList>.syn) */
  struct IdListNode *id_lis = (struct IdListNode *) malloc(sizeof(struct IdListNode));
  id_lis->ptr1 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str));
  child_node = child_node->next;
  id_lis->ptr2 = child_node->syn.node.id_lis;

  curr_node->syn.type = ID_LIST_NODE;
  curr_node->syn.node.id_lis = id_lis;
}


void case_57(struct treeNode *curr_node) {
  /* <sub_idList> := COMMA ID <sub_idList>1 */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  child_node = child_node->next;

  /* <sub_idList>.syn = new IdListNode(new LeafNode(ID, ID.entry), <sub_idList>1.syn) */
  struct IdListNode *id_lis = (struct IdListNode *) malloc(sizeof(struct IdListNode));
  id_lis->ptr1 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str));
  child_node = child_node->next;
  id_lis->ptr2 = child_node->syn.node.id_lis;

  curr_node->syn.type = ID_LIST_NODE;
  curr_node->syn.node.id_lis = id_lis;
}


void case_58(struct treeNode *curr_node) {
  /* <sub_idList> := EPSILON */

  curr_node->syn.type = NULL_NODE;
  memset(&(curr_node->syn.node), 0, sizeof(union ASTNodesUnion));
}


void case_59(struct treeNode *curr_node) {
  /* <new_expression> : = <u> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* < new_expression >.syn = <u>.syn */
  curr_node->syn = child_node->syn;
}


void case_60(struct treeNode *curr_node) {
  /* <new_expression> := <expression> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <new_expression>.syn = <expression>.syn */
  curr_node->syn = child_node->syn;
}


void case_61(struct treeNode *curr_node) {
  /* <u> : = PLUS <sub_u> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  struct UNode *u_node = (struct UNode *)malloc(sizeof(struct UNode));
  u_node->op = PLUS;
  u_node->ptr1 = &(child_node->syn);
  /* < u >.syn = new UNode(“PLUS”, <sub_u>.syn) */
  curr_node->syn.type = U_NODE;
  curr_node->syn.node.u = u_node; 
}


void case_62(struct treeNode *curr_node) {
  /* <u> : = MINUS <sub_u> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  struct UNode *u_node = (struct UNode *)malloc(sizeof(struct UNode));
  u_node->op = MINUS;
  u_node->ptr1 = &(child_node->syn);
  /* < u >.syn = new UNode(“MINUS”, <sub_u>.syn) */
  curr_node->syn.type = U_NODE;
  curr_node->syn.node.u = u_node;
}


void case_63(struct treeNode *curr_node) {
  /* <sub_u> : = BO <arithmeticExpr> BC */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  /* < sub_u >.syn = <arithmeticExpr>.syn */
  curr_node->syn = child_node->syn;
}


void case_64(struct treeNode *curr_node) {
  /* <sub_u> := <var> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  child_node = nextNonTerminalNode(child_node);
  /* <sub_u>.syn = <var>.syn */
  curr_node->syn = child_node->syn;
}


void case_65(struct treeNode *curr_node) {
  /* <expression> := <AnyTerm> <N7>       --------> Assuming we have inh attribute- type Attribute*/
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  /* 1. <N7>.inh = <AnyTerm>.syn */
  next_node->inh = child_node->syn;        
  /* 2. IF (<N7>.syn == NULL) <expression>.syn = <AnyTerm>.syn ELSE <expression>.syn = <N7>.syn */
  if(next_node->syn.type == NULL_NODE) {  // checks for NULL using type attr.
    curr_node->syn = child_node->syn;
  }
  else {
    curr_node->syn = next_node->syn;
  }
}


void case_66(struct treeNode *curr_node) {
  /* <N7> := <logicalOp> <AnyTerm> <N7>1 */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  struct treeNode *next_next_node = nextNonTerminalNode(next_node);

  /* 1. <N7>1.inh = <AnyTerm>.syn */
  next_next_node->inh = next_node->syn;
  struct N7Node *n7_node = (struct N7Node *)malloc(sizeof(struct N7Node));
  n7_node->ptr1 = &(curr_node->inh);
  n7_node->logicalOp = child_node->val;

  /* 2. IF (<N7>1.syn != NULL) <N7>.syn = new N7Node(<N7>.inh, <logicalOp>.val, <N7>1.syn)
        ELSE <N7>.syn = new N7Node(<N7>.inh, <logicalOp>.val, <AnyTerm>.syn) */
  if(next_next_node->syn.type == NULL_NODE) {
    n7_node->ptr2 = &(next_next_node->syn);
  }
  else {
    n7_node->ptr2 = &(next_node->syn);
  }
  curr_node->syn.node.n7 = n7_node;
  curr_node->syn.type = N7_NODE;
}


void case_67(struct treeNode *curr_node) {
  /* <N7> := EPSILON */
  curr_node->syn.type = NULL_NODE;
  curr_node->syn.node.n7 = NULL;
  /* <N7>.syn = NULL */
}


void case_68(struct treeNode *curr_node) {
  /* <AnyTerm> := <arithmeticExpr> <N8> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  /* 1. <N8>.inh = <arithmeticExpr>.syn */
  next_node->inh = child_node->syn;

  /* 2. IF (<N8>.syn == NULL) <AnyTerm>.syn = <arithmeticExpr>.syn ELSE <AnyTerm>.syn = <N8>.syn */
  if(next_node->syn.type == NULL_NODE) {
    curr_node->syn = child_node->syn;
  }
  else {
    curr_node->syn = next_node->syn;
  }
}


void case_69(struct treeNode *curr_node) {
  /* <AnyTerm> := <boolConstt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  /* <AnyTerm>.syn = <boolConstt>.syn */
  curr_node->syn = child_node->syn;
}


void case_70(struct treeNode *curr_node) {
  /* <N8> := <relationalOp> <arithmeticExpr> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  struct N8Node *n8_node = (struct N8Node *)malloc(sizeof(struct N8Node));
  n8_node->ptr1 = &(curr_node->inh);
  n8_node->relationalOp = child_node->val;
  n8_node->ptr2 = &(next_node->syn);
  /* <N8>.syn = new N8Node(<N8>.inh, <relationalOp>.val, <arithmeticExpr>.syn) */
  curr_node->syn.type = N8_NODE;
  curr_node->syn.node.n8 = n8_node;
}


void case_71(struct treeNode *curr_node) {
  /* <N8> := EPSILON */
  curr_node->syn.type = NULL_NODE;
  curr_node->syn.node.n8 = NULL;
  /* <N8>.syn = NULL */
}


void case_72(struct treeNode *curr_node){
  /* <arithmeticExpr> := <term> <sub_arithmeticExpr> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  /* 1. IF (<sub_arithmeticExpr>.syn == NULL) <arithmeticExpr>.syn = <term>.syn 
        ELSE <arithmeticExpr>.syn = <sub_arithmeticExpr>.syn */
  if (next_node->syn.type == NULL_NODE)
  {
    curr_node->syn = child_node->syn;
  }
  else
  {
    curr_node->syn = next_node->syn;
  }
  /* 2. <sub_arithmeticExpr>.inh = <term>.syn */
  next_node->inh = child_node->syn;
}

void case_73(struct treeNode *curr_node){
  /* <sub_arithmeticExpr> := <op1> <term> <sub_arithmeticExpr>1 */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  struct treeNode *next_next_node = nextNonTerminalNode(next_node);
  struct ArithmeticExprNode *arithmetic_expr_node = (struct ArithmeticExprNode *)malloc(sizeof(struct ArithmeticExprNode));
  arithmetic_expr_node->ptr1 = &(curr_node->inh);
  arithmetic_expr_node->op = child_node->val;
  arithmetic_expr_node->ptr2 = &(next_node->syn);
  /* 1. <sub_arithmeticExpr>.syn = new ArithmeticExprNode(<sub_arithmeticExpr>.inh, <op1>.val, <term>.syn) */
  curr_node->syn.type = ARITHMETIC_EXPR_NODE;
  curr_node->syn.node.ari_exp = arithmetic_expr_node;
  /* 2. <sub_arithmeticExpr>1.inh = <sub_arithmeticExpr>.syn */
  next_next_node->inh = curr_node->syn;
}


void case_74(struct treeNode *curr_node){
  /* <sub_arithmeticExpr> := EPSILON */
  curr_node->syn.type = NULL_NODE;
  curr_node->syn.node.ari_exp = NULL;
  /*1. <sub_arithmeticExpr>.syn = NULL */
}


void case_75(struct treeNode *curr_node){
  /* <term> :=  <factor> <sub_term> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  /* 1. IF (<sub_term>.syn == NULL) <term>.syn = <factor>.syn ELSE <term>.syn = <sub_term>.syn */
  if (next_node->syn.type == NULL_NODE)
  {
    curr_node->syn = child_node->syn;
  }
  else
  {
    curr_node->syn = next_node->syn;
  }
  /* 2. <sub_term>.inh = <factor>.syn */
  next_node->inh = child_node->syn;
}


void case_76(struct treeNode *curr_node){
  /* <sub_term> := <op2> <factor> <sub_term>1 */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  struct treeNode *next_next_node = nextNonTerminalNode(next_node);
  struct TermNode *term_node = (struct TermNode *)malloc(sizeof(struct TermNode));
  term_node->ptr1 = &(curr_node->inh);
  term_node->op = child_node->val;
  term_node->ptr2 = &(next_node->syn);
  /* 1. <sub_term>.syn = new TermNode(<sub_term>.inh, <op2>.val, <factor>.syn) */
  curr_node->syn.type = TERM_NODE;
  curr_node->syn.node.ter = term_node;
  /* 2. <sub_term>1.inh = <sub_term>.syn */
  next_next_node->inh = curr_node->syn;
}


void case_77(struct treeNode *curr_node){
  /* <sub_term> := EPSILON */
  curr_node->syn.type = NULL_NODE;
  curr_node->syn.node.ter = NULL;
  /* <sub_term>.syn = NULL */
}


void case_78(struct treeNode *curr_node){
  /* <factor> := BO <expression> BC */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  /* <factor>.syn  = <expression>.syn */
  curr_node->syn.node = child_node->syn.node;
  curr_node->syn.type = child_node->syn.type;
}


void case_79(struct treeNode *curr_node){
  /* <factor> := <var> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  /* <factor>.syn  = <var>.syn */
  curr_node->syn = child_node->syn;
}


void case_92(struct treeNode *curr_node) {
  /* <declareStmt> := DECLARE <idList> COLON <dataType> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct DeclareStmtNode *dec_stmt_node = (struct DeclareStmtNode *) malloc(sizeof(struct DeclareStmtNode));
  child_node = nextNonTerminalNode(child_node);
  dec_stmt_node->ptr1 = child_node->syn.node.id_lis;
  child_node = nextNonTerminalNode(child_node);
  if(child_node->syn.type == LEAF_NODE) {
    dec_stmt_node->ptr2->node.lea = child_node->syn.node.lea;
    dec_stmt_node->ptr2->type = LEAF_NODE;
  }
  else {
    dec_stmt_node->ptr2->node.arr_typ = child_node->syn.node.arr_typ;
    dec_stmt_node->ptr2->type = ARRAY_TYPE_NODE;
  }

  /* <declareStmt>.syn = new DeclareStmtNode(<dataType>.syn, <idList>.syn) */
  curr_node->syn.node.dec_stm = dec_stmt_node;
  curr_node->syn.type = DECLARE_STMT_NODE;
}


void case_93(struct treeNode *curr_node) {
  /* <conditionalStmt> := SWITCH BO ID BC START <caseStmt> <default> END */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct ConditionalStmtNode *cond_stmt_node = (struct ConditionalStmtNode *) malloc(sizeof(struct ConditionalStmtNode));
  child_node = child_node->next->next;
  cond_stmt_node->ptr1 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str));
  child_node = nextNonTerminalNode(child_node);
  cond_stmt_node->ptr2 = child_node->syn.node.cas_stm;
  child_node = nextNonTerminalNode(child_node);
  cond_stmt_node->ptr3 = child_node->syn.node.stm;
  
  /* <conditionalStmt>.syn = new CoditionalStmtNode(new LeafNode(ID, ID.entry), <caseStmt>.syn, <default>.syn) */
  curr_node->syn.node.con_stm = cond_stmt_node;
  curr_node->syn.type = CONDITIONAL_STMT_NODE;
}

void case_94_95(struct treeNode *curr_node) {
  /* 94 -  <caseStmt> := CASE <value> COLON <statements> BREAK SEMICOL <nullableCaseStmt> */
  /* 95 - <nullableCaseStmt> := CASE <value> COLON <statements> BREAK SEMICOL <nullableCaseStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct CaseStmtNode *cas_stm_node = (struct CaseStmtNode *) malloc(sizeof(struct CaseStmtNode));
  child_node = nextNonTerminalNode(child_node);
  cas_stm_node->ptr1 = child_node->syn.node.lea;
  child_node = nextNonTerminalNode(child_node);
  cas_stm_node->ptr2 = child_node->syn.node.stm;
  child_node = nextNonTerminalNode(child_node);
  cas_stm_node->ptr3 = child_node->syn.node.cas_stm;

  /* 94 - <caseStmt>.syn = new CaseStmtNode(<value>.syn, <statements>.syn, <nullableCaseStmt>.syn) */
  /* 95 - <nullableCaseStmt>.syn = new CaseStmtNode(<value>.syn, <statements>.syn, <nullableCaseStmt>.syn) */
  curr_node->syn.node.cas_stm = cas_stm_node;
  curr_node->syn.type = CASE_STMT_NODE;
}


void case_96(struct treeNode *curr_node) {
  /* <nullableCaseStmt> := EPSILON */
  
  /* <nullableCaseStmt>.syn = NULL */
  curr_node->syn.node.cas_stm = NULL;
  curr_node->syn.type = CASE_STMT_NODE;
}


void case_97(struct treeNode *curr_node) {
  /* <value> := NUM */
  struct treeNode *child_node = curr_node->child;

  /* <value> = new LeafNode(NUM, NUM.val) */
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num));
  curr_node->syn.type = LEAF_NODE;
}


void case_98(struct treeNode *curr_node) {
  /* <value> := NUM */

  /* <value> = new LeafNode(BOOL, "TRUE") */
  bool val = true;
  curr_node->syn.node.lea = newLeafNode(BOOLEAN_, &(val));
  curr_node->syn.type = LEAF_NODE;
}


void case_99(struct treeNode *curr_node) {
  /* <value> := NUM */

  /* <value> = new LeafNode(BOOL, "FALSE") */
  bool val = false;
  curr_node->syn.node.lea = newLeafNode(BOOLEAN_, &(val));
  curr_node->syn.type = LEAF_NODE;
}


void case_100(struct treeNode *curr_node) {
  /* <default> := DEFAULT COLON <statements> BREAK SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <default>.syn = <statements>.syn */
  curr_node->syn.node.stm = child_node->syn.node.stm;
  curr_node->syn.type = STATEMENT_NODE;
}


void case_101(struct treeNode *curr_node) {
  /* <default> :=  EPSILON */

  /* <default>.syn = NULL */
  curr_node->syn.node.stm = NULL;
  curr_node->syn.type = STATEMENT_NODE;
}


void case_102(struct treeNode *curr_node) {
  /* <iterativeStmt> := FOR BO ID IN <range> BC START <statements> END */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct ForIterativeStmtNode *for_iter_node = (struct ForIterativeStmtNode *) malloc(sizeof(struct ForIterativeStmtNode));
  child_node = child_node->next->next;
  for_iter_node->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str);
  child_node = nextNonTerminalNode(child_node);
  for_iter_node->ptr2 = child_node->syn.node.ran;
  child_node = nextNonTerminalNode(child_node);
  for_iter_node->ptr3 = child_node->syn.node.stm;

  /* <iterativeStmt>.syn = new ForIterativeStmtNode(new LeafNode(ID, ID.entry), <range>.syn, <statements>.syn) */
  curr_node->syn.node.for_ite_stm = for_iter_node;
  curr_node->syn.type = FOR_ITERATIVE_STMT_NODE;
}


// TO DO:
void case_103(struct treeNode *curr_node) {
  /* <iterativeStmt> := WHILE BO <expression> BC START <statements> END */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct WhileIterativeStmtNode * while_iter_node = (struct WhileIterativeStmtNode *) malloc(sizeof(struct WhileIterativeStmtNode));
  child_node = nextNonTerminalNode(child_node);
  while_iter_node->ptr1 = &(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  while_iter_node->ptr2 = child_node->syn.node.stm;

  /* <iterativeStmt>.syn = new WhileIterativeStmtNode(<expression>.syn, <statements>.syn) */
  curr_node->syn.node.whi_ite_stm = while_iter_node;
  curr_node->syn.type = WHILE_ITERATIVE_STMT_NODE;
}


void case_104(struct treeNode *curr_node) {
  /* <range> := NUM RANGEOP NUM */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct RangeNode *range_node = (struct RangeNode *) malloc(sizeof(struct RangeNode));
  range_node->ptr1 = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num));
  child_node = child_node->next->next;
  range_node->ptr2 = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num));
  
  /* <range>.syn = new RangeNode(new LeafNode(NUM1, NUM1.val), new LeafNode(NUM2, NUM2.val)) */
  curr_node->syn.node.ran = range_node;
  curr_node->syn.type = RANGE_NODE;
}


/* BEGIN : Utility Functions */
void traverseChildren(struct treeNode *curr_node)
{
  while(curr_node != NULL) {
    traverseParseTree(curr_node);
    curr_node = curr_node->next;
  }
}

struct treeNode *nextNonTerminalNode(struct treeNode *curr_node) {
  if (curr_node == NULL)
    return NULL;

  curr_node = curr_node->next;
  while(curr_node != NULL) {
    if (curr_node->flag == NON_TERMINAL) {
      return curr_node;
    }
    curr_node = curr_node->next;
  }

  return NULL;
}

struct LeafNode *newLeafNode(int type, void *data) {
    int datalen = 0;
    struct LeafNode *new_node = (struct LeafNode *) malloc(sizeof(struct LeafNode));
    new_node->type = type;
    switch (new_node->type) {
        case (BOOLEAN_):
            memcpy(&(new_node->value.boolean), data, sizeof(bool));
            break;
        case (NUM):
            memcpy(&(new_node->value.num), data, sizeof(int));
            break;
        case (RNUM):
            memcpy(&(new_node->value.rnum), data, sizeof(float));
            break;
        case (IDENTIFIER):
            datalen = strlen(data);
            new_node->value.entry = malloc(datalen + 1);  /* +1 for the '\0' */
            strcpy(new_node->value.entry, (char *)data);
            break;
        default:
            free(new_node);
            die("Invalid node type.\n");
    }
    return new_node;
}
/* END : Utility Functions */

void createAST() {
  traverseParseTree(PT.head);
}
