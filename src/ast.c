#include "ast.h"
#include <assert.h>

extern struct parseTree PT;
struct ProgramNode AST;
extern char terminalStringRepresentations[NUM_TERMINALS][16];

/* BEGIN : Function Declarations */
void traverseParseTree(struct treeNode *curr_node);
void traverseChildren(struct treeNode *curr_node);
struct treeNode *nextNonTerminalNode(struct treeNode *curr_node);
struct LeafNode *newLeafNode(int type, void *data, int line_num);
struct Attribute *newAttribute(struct Attribute attr);
/* END : Function Declarations */

void createAST() {
  traverseParseTree(PT.head);
  AST.ptr1 = PT.head->syn.node.pro->ptr1;
  AST.ptr2 = PT.head->syn.node.pro->ptr2;
  AST.ptr3 = PT.head->syn.node.pro->ptr3;
  AST.ptr4 = PT.head->syn.node.pro->ptr4;
}

void traverseParseTree(struct treeNode *curr_node) {
  if (curr_node->flag == TERMINAL) {
    return;
  }

  int rule_num = curr_node->rule_number + 1;
  // printf("CASE: %d\n", rule_num);
  // fflush(stdout);
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
      case_60(curr_node);
      break;

    case 61: // <u> := PLUS <sub_u>
      case_61(curr_node);
      break;

    case 62: // <u> := MINUS <sub_u>
      case_62(curr_node);
      break;

    case 63: // <sub_u> := BO <arithmeticExpr> BC
      case_63(curr_node);
      break;

    case 64: // <sub_u> := <var>
      case_64(curr_node);
      break;

    case 65: // <expression> := <AnyTerm> <N7>
      case_65(curr_node);
      break;

    case 66: // <N7> := <logicalOp> <AnyTerm> <N7>
      case_66(curr_node);
      break;

    case 67: // <N7> := EPSILON
      case_67(curr_node);
      break;

    case 68: // <AnyTerm> := <arithmeticExpr> <N8>
      case_68(curr_node);
      break;

    case 69: // <AnyTerm> := <boolConstt>
      case_69(curr_node);
      break;

    case 70: // <N8> := <relationalOp> <arithmeticExpr>
      case_70(curr_node);
      break;

    case 71: // <N8> := EPSILON
      case_71(curr_node);
      break;

    case 72: // <arithmeticExpr> := <term> <sub_arithmeticExpr>
      case_72(curr_node);
      break;

    case 73: // <sub_arithmeticExpr> := <op1> <term> <sub_arithmeticExpr>
      case_73(curr_node);
      break;

    case 74: // <sub_arithmeticExpr> := EPSILON
      case_74(curr_node);
      break;

    case 75: // <term> :=  <factor> <sub_term>
      case_75(curr_node);
      break;

    case 76: // <sub_term> := <op2> <factor> <sub_term>
      case_76(curr_node);
      break;

    case 77: // <sub_term> := EPSILON
      case_77(curr_node);
      break;

    case 78: // <factor> := BO <expression> BC
      case_78(curr_node);
      break;

    case 79: // <factor> := <var>
      case_79(curr_node);
      break;

    case 80: // <op1> := PLUS
      case_80(curr_node);
      break;

    case 81: // <op1> := MINUS
      case_81(curr_node);
      break;

    case 82: // <op2> := MUL
      case_82(curr_node);
      break;

    case 83: // <op2> := DIV
      case_83(curr_node);
      break;

    case 84: // <logicalOp> := AND
      case_84(curr_node);
      break;

    case 85: // <logicalOp> := OR
      case_85(curr_node);
      break;

    case 86: // <relationalOp> := LT
      case_86(curr_node);
      break;

    case 87: // <relationalOp> := LE
      case_87(curr_node);
      break;

    case 88: // <relationalOp> := GT
      case_88(curr_node);
      break;

    case 89: // <relationalOp> := GE
      case_89(curr_node);
      break;

    case 90: // <relationalOp> := EQ
      case_90(curr_node);
      break;

    case 91: // <relationalOp> := NE
      case_91(curr_node);
      break;

    case 92: // <declareStmt> := DECLARE <idList> COLON <dataType> SEMICOL
      case_92(curr_node);
      break;

    case 93: // <conditionalStmt> := SWITCH BO ID BC START <caseStmt> <default> END
      case_93(curr_node);
      break;

    case 94: // <caseStmt> := CASE <value> COLON <statements> BREAK SEMICOL <nullableCaseStmt>
      case_94_95(curr_node);
      break;

    case 95: // <nullableCaseStmt> := CASE <value> COLON <statements> BREAK SEMICOL <nullableCaseStmt>1
      case_94_95(curr_node);
      break;

    case 96: // <nullableCaseStmt> := EPSILON
      case_96(curr_node);
      break;

    case 97: // <value> := NUM
      case_97(curr_node);
      break;

    case 98: // <value> := TRUE
      case_98(curr_node);
      break;

    case 99: // <value> := FALSE
      case_99(curr_node);
      break;

    case 100: // <default> := DEFAULT COLON <statements> BREAK SEMICOL
      case_100(curr_node);
      break;

    case 101: // <default> :=  EPSILON
      case_101(curr_node);
      break;

    case 102: // <iterativeStmt> := FOR BO ID IN <range> BC START <statements> END
      case_102(curr_node);
      break;

    case 103: // <iterativeStmt> := WHILE BO <expression> BC START <statements> END
      case_103(curr_node);
      break;

    case 104: // <range> := NUM RANGEOP NUM
      case_104(curr_node);
      break;

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
  curr_node->syn.type = NULL_NODE;
}


void case_4(struct treeNode *curr_node) {
  /* <moduleDeclaration> := DECLARE MODULE ID SEMICOL */
  struct treeNode *id = curr_node->child->next->next;

  /* <moduleDeclaration>.syn = new LeafNode(ID, ID.entry) */
  curr_node->syn.node.lea = newLeafNode(IDENTIFIER, id->symbol.terminal.lexeme.str, id->symbol.terminal.line_no);
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
  curr_node->syn.type = NULL_NODE;
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
  module_node->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
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
  curr_node->syn.type = NULL_NODE;
}


void case_11(struct treeNode *curr_node) {
  /* <input_plist> := ID COLON <dataType> <sub_input_plist> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct InputPlistNode *inp_pli = (struct InputPlistNode *) malloc(sizeof(struct InputPlistNode));
  inp_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  child_node = nextNonTerminalNode(child_node);
  inp_pli->ptr2 = newAttribute(child_node->syn);
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
  child_node = child_node->next;
  inp_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  child_node = nextNonTerminalNode(child_node);
  inp_pli->ptr2 = newAttribute(child_node->syn);
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
  curr_node->syn.type = NULL_NODE;
}


void case_14(struct treeNode *curr_node) {
  /* <output_plist> := ID COLON <type> <sub_output_plist> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct OutputPlistNode *out_pli = (struct OutputPlistNode *) malloc(sizeof(struct OutputPlistNode));
  out_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  child_node = nextNonTerminalNode(child_node);
  out_pli->ptr2 = newAttribute(child_node->syn);
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
  child_node = child_node->next;
  out_pli->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  child_node = nextNonTerminalNode(child_node);
  out_pli->ptr2 = newAttribute(child_node->syn);
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
  curr_node->syn.type = NULL_NODE;
}


void case_17(struct treeNode *curr_node) {
  /* <dataType> := INTEGER */

  /* <dataType>.syn = new LeafNode(TYPE, “INT”) */
  curr_node->syn.node.lea = newLeafNode(INTEGER, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_18(struct treeNode *curr_node) {
  /* <dataType> := REAL */

  /* <dataType>.syn = new LeafNode(TYPE, “REAL”) */
  curr_node->syn.node.lea = newLeafNode(REAL, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_19(struct treeNode *curr_node) {
  /* <dataType> := BOOLEAN */

  /* <dataType>.syn = new LeafNode(TYPE, “BOOL”) */
  curr_node->syn.node.lea = newLeafNode(BOOLEAN_, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
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
  traverseChildren(child_node);

  struct DynamicRangeNode *dyn_ran = (struct DynamicRangeNode *) malloc(sizeof(struct DynamicRangeNode));
  dyn_ran->ptr1 = child_node->syn.node.lea;

  child_node = nextNonTerminalNode(child_node);
  dyn_ran->ptr2 = child_node->syn.node.lea;

  /* <dynamic_range>.syn = new DynamicRangeNode(<index>1.syn, <index>2.syn) */
  curr_node->syn.type = DYNAMIC_RANGE_NODE;
  curr_node->syn.node.dyn_ran = dyn_ran;
}


void case_22(struct treeNode *curr_node) {
  /* <type> := INTEGER */

  /* <type>.syn = new LeafNode(TYPE, “INT”) */
  curr_node->syn.node.lea = newLeafNode(INTEGER, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_23(struct treeNode *curr_node) {
  /* <type> := REAL */

  /* <type>.syn = new LeafNode(TYPE, “REAL”) */
  curr_node->syn.node.lea = newLeafNode(REAL, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_24(struct treeNode *curr_node) {
  /* <type> := BOOLEAN */

  /* <type>.syn = new LeafNode(TYPE, “BOOL”) */
  curr_node->syn.node.lea = newLeafNode(BOOLEAN_, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
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
  stm->ptr1 = newAttribute(child_node->syn);
  child_node = nextNonTerminalNode(child_node);
  stm->ptr2 = child_node->syn.node.stm;

  /* <statements>.syn = new StatementNode(<statement>.syn, <statements>.syn) */
  curr_node->syn.type = STATEMENT_NODE;
  curr_node->syn.node.stm = stm;
}


void case_27(struct treeNode *curr_node) {
  /* <statements> := EPSILON */

  /* <statements>.syn = NULL */
  curr_node->syn.type = NULL_NODE;
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
  input_node->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);

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
  print_node->ptr1 = (struct Attribute *) malloc(sizeof(struct Attribute));
  // print_node->ptr1->type = LEAF_NODE;
  // print_node->ptr1->node.lea = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  print_node->ptr1 = newAttribute(child_node->syn);
  /* <ioStmt>.syn = new PrintNode(<extended_var>.syn) */
  curr_node->syn.node.pri = print_node;
  curr_node->syn.type = PRINT_NODE;
}


void case_35(struct treeNode *curr_node) {
  /* <boolConstt> := TRUE */

  /* <boolConstt>.syn = new LeafNode(BOOL, “TRUE”) */
  curr_node->syn.node.lea = newLeafNode(TRUE_, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_36(struct treeNode *curr_node) {
  /* <boolConstt> := FALSE */

  /* <boolConstt>.syn = new LeafNode(BOOL, “FALSE”) */
  curr_node->syn.node.lea = newLeafNode(FALSE_, NULL, curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
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
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct treeNode *next_node = nextNonTerminalNode(child_node);
  /* IF (<whichId>.syn == NULL)
   *   <var>.syn = new LeafNode(ID, ID.entry)
   * ELSE
   *   <var>.syn = new ArrayNode(new LeafNode(ID, ID.entry), <whichId>.syn) */
  if (next_node->syn.type == NULL_NODE) {
    curr_node->syn.type = LEAF_NODE;
    curr_node->syn.node.lea = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  }
  else {
    struct ArrayNode *arr = (struct ArrayNode *) malloc(sizeof(struct ArrayNode));
    arr->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
    arr->ptr2 = next_node->syn.node.lea;
    curr_node->syn.type = ARRAY_NODE;
    curr_node->syn.node.arr = arr;
  }
}


void case_40(struct treeNode *curr_node) {
  /* <var> := NUM */
  struct treeNode *child_node = curr_node->child;

  /* <var>.syn = new LeafNode(NUM, NUM.val) */
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num), child_node->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_41(struct treeNode *curr_node) {
  /* <var> := RNUM */
  struct treeNode *child_node = curr_node->child;

  /* <var>.syn = new LeafNode(RNUM, RNUM.val) */
  curr_node->syn.node.lea = newLeafNode(RNUM, &(child_node->symbol.terminal.lexeme.rnum), child_node->symbol.terminal.line_no);
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
  agn_stm->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  child_node = child_node->next;
  agn_stm->ptr2 = newAttribute(child_node->syn);

  curr_node->syn.node.agn_stm = agn_stm;
  curr_node->syn.type = ASSIGN_STMT_NODE;
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
  child_node = nextNonTerminalNode(child_node);
  lva_id->ptr1 = newAttribute(child_node->syn);

  curr_node->syn.node.lva_id = lva_id;
  curr_node->syn.type = LVALUE_ID_NODE;
}


void case_50(struct treeNode *curr_node) {
  /* <lvalueARRStmt> := SQBO <index> SQBC ASSIGNOP <new_expression> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <lvalueARRStmt>.syn = new LvalueARRNode(<index>.syn, <new_expression>.syn) */
  struct LvalueARRNode *lva_arr = (struct LvalueARRNode *) malloc(sizeof(struct LvalueARRNode));
  child_node = nextNonTerminalNode(child_node);
  lva_arr->ptr1 = child_node->syn.node.lea;
  child_node = nextNonTerminalNode(child_node);
  lva_arr->ptr2 = newAttribute(child_node->syn);

  curr_node->syn.type = LVALUE_ARR_NODE,
  curr_node->syn.node.lva_arr = lva_arr;
}


void case_51(struct treeNode *curr_node) {
  /* <index> := NUM  */
  struct treeNode *child_node = curr_node->child;

  /* <index>.syn = new LeafNode(NUM, NUM.val) */
  curr_node->syn.type = LEAF_NODE;
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num), child_node->symbol.terminal.line_no);
}


void case_52(struct treeNode *curr_node) {
  /* <index> := ID */
  struct treeNode *child_node = curr_node->child;

  /* <index>.syn = new LeafNode(ID, ID.entry) */
  curr_node->syn.type = LEAF_NODE;
  curr_node->syn.node.lea = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str), child_node->symbol.terminal.line_no);
}


void case_53(struct treeNode *curr_node) {
  /* <moduleReuseStmt> := <optional> USE MODULE ID WITH PARAMETERS <idList> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <moduleReuseStmt>.syn = new ModuleReuseStmtNode(<optional>.syn, new LeafNode(ID, ID.entry), <idList>.syn) */
  struct ModuleReuseStmtNode *mod_reu_stm = (struct ModuleReuseStmtNode *) malloc(sizeof(struct ModuleReuseStmtNode));
  mod_reu_stm->ptr1 = child_node->syn.node.id_lis;
  child_node = child_node->next->next->next;
  mod_reu_stm->ptr2 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str), child_node->symbol.terminal.line_no);
  child_node = nextNonTerminalNode(child_node);
  mod_reu_stm->ptr3 = child_node->syn.node.id_lis;

  curr_node->syn.type = MODULE_REUSE_STMT_NODE;
  curr_node->syn.node.mod_reu_stm = mod_reu_stm;
}


void case_54(struct treeNode *curr_node) {
  /* <optional> := SQBO <idList> SQBC ASSIGNOP */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  child_node = nextNonTerminalNode(child_node);

  /* <optional>.syn = <idList>.syn */
  curr_node->syn = child_node->syn;
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
  id_lis->ptr1 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str), child_node->symbol.terminal.line_no);
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
  id_lis->ptr1 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str), child_node->symbol.terminal.line_no);
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
  u_node->ptr1 = newAttribute(child_node->syn);
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
  u_node->ptr1 = newAttribute(child_node->syn);
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

  // child_node = nextNonTerminalNode(child_node);

  /* <sub_u>.syn = <var>.syn */
  curr_node->syn = child_node->syn;
}


void case_65(struct treeNode *curr_node) {
  /* <expression> := <AnyTerm> <N7>       --------> Assuming we have inh attribute- type Attribute*/
  struct treeNode *child_node = curr_node->child;
  traverseParseTree(child_node);
  /* 1. <N7>.inh = <AnyTerm>.syn */
  struct treeNode *next_node = nextNonTerminalNode(child_node);
  next_node->inh = child_node->syn;
  traverseParseTree(next_node);

  
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
  traverseParseTree(child_node);
  enum terminal val = child_node->val;
  child_node = nextNonTerminalNode(child_node);
  traverseParseTree(child_node);

  /* 1. <N7>1.inh = <AnyTerm>.syn */
  struct treeNode *next_node = nextNonTerminalNode(child_node);
  next_node->inh = child_node->syn;
  
  traverseParseTree(next_node);
  
  /* 2. */
  /* IF (<N7>1.syn != NULL) <N7>.syn = new N7Node(<N7>.inh, <logicalOp>.val, <N7>1.syn) */
  /* ELSE <N7>.syn = new N7Node(<N7>.inh, <logicalOp>.val, <AnyTerm>.syn) */
  struct N7Node *n7_node = (struct N7Node *)malloc(sizeof(struct N7Node));
  n7_node->ptr1 = newAttribute(curr_node->inh);
  n7_node->logicalOp = val;
  
  if(next_node->syn.type == NULL_NODE) {
    n7_node->ptr2 = newAttribute(child_node->syn);
  }
  else {
    n7_node->ptr2 = newAttribute(next_node->syn);
  }

  curr_node->syn.node.n7 = n7_node;
  curr_node->syn.type = N7_NODE;
}


void case_67(struct treeNode *curr_node) {
  /* <N7> := EPSILON */

  /* <N7>.syn = NULL */
  curr_node->syn.node.n7 = NULL;
  curr_node->syn.type = NULL_NODE;
}


void case_68(struct treeNode *curr_node) {
  /* <AnyTerm> := <arithmeticExpr> <N8> */
  struct treeNode *child_node = curr_node->child;
  traverseParseTree(child_node);

  /* 1. <N8>.inh = <arithmeticExpr>.syn */
  struct treeNode *next_node = nextNonTerminalNode(child_node);
  next_node->inh = child_node->syn;
  traverseParseTree(next_node);

  /* 2. */
  /* IF (<N8>.syn == NULL) <AnyTerm>.syn = <arithmeticExpr>.syn */
  /* ELSE <AnyTerm>.syn = <N8>.syn */
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

  /* <N8>.syn = new N8Node(<N8>.inh, <relationalOp>.val, <arithmeticExpr>.syn) */
  struct N8Node *n8_node = (struct N8Node *)malloc(sizeof(struct N8Node));
  n8_node->ptr1 = newAttribute(curr_node->inh);
  n8_node->relationalOp = child_node->val;
  child_node = nextNonTerminalNode(child_node);
  n8_node->ptr2 = newAttribute(child_node->syn);

  curr_node->syn.node.n8 = n8_node;
  curr_node->syn.type = N8_NODE;
}


void case_71(struct treeNode *curr_node) {
  /* <N8> := EPSILON */

  /* <N8>.syn = NULL */
  curr_node->syn.node.n8 = NULL;
  curr_node->syn.type = NULL_NODE;
}


void case_72(struct treeNode *curr_node){
  /* <arithmeticExpr> := <term> <sub_arithmeticExpr> */
  struct treeNode *child_node = curr_node->child;
  traverseParseTree(child_node);

  /* 1. <sub_arithmeticExpr>.inh = <term>.syn */
  struct treeNode *next_node = nextNonTerminalNode(child_node);
  next_node->inh = child_node->syn;
  traverseParseTree(next_node);

  /* 2.
    IF (<sub_arithmeticExpr>.syn == NULL) <arithmeticExpr>.syn = <term>.syn
    ELSE <arithmeticExpr>.syn = <sub_arithmeticExpr>.syn */
  if (next_node->syn.type == NULL_NODE) {
    curr_node->syn = child_node->syn;
  }
  else {
    next_node->syn.node.ari_exp->is_first = true;
    curr_node->syn = next_node->syn;
  }
}


void case_73(struct treeNode *curr_node){
  /* <sub_arithmeticExpr> := <op1> <term> <sub_arithmeticExpr>1 */
  struct treeNode *child_node = curr_node->child;

  /* 1. <sub_arithmeticExpr>.syn = new ArithmeticExprNode(<sub_arithmeticExpr>.inh, <op1>.val, <term>.syn) */
  struct ArithmeticExprNode *arithmetic_expr_node = (struct ArithmeticExprNode *)malloc(sizeof(struct ArithmeticExprNode));
  arithmetic_expr_node->ptr1 = newAttribute(curr_node->inh);
  arithmetic_expr_node->is_first = false;
  traverseParseTree(child_node);
  arithmetic_expr_node->op = child_node->val;
  child_node = nextNonTerminalNode(child_node);
  traverseParseTree(child_node);
  arithmetic_expr_node->ptr2 = newAttribute(child_node->syn);

  curr_node->syn.node.ari_exp = arithmetic_expr_node;
  curr_node->syn.type = ARITHMETIC_EXPR_NODE;

  /* 2. <sub_arithmeticExpr>1.inh = <sub_arithmeticExpr>.syn */
  child_node = nextNonTerminalNode(child_node);
  child_node->inh = curr_node->syn;
  traverseParseTree(child_node);

  arithmetic_expr_node->ptr3 = newAttribute(child_node->syn);
}


void case_74(struct treeNode *curr_node){
  /* <sub_arithmeticExpr> := EPSILON */

  curr_node->syn.node.ari_exp = NULL;
  curr_node->syn.type = NULL_NODE;
  /*1. <sub_arithmeticExpr>.syn = NULL */
}


void case_75(struct treeNode *curr_node){
  /* <term> :=  <factor> <sub_term> */
  struct treeNode *child_node = curr_node->child;
  traverseParseTree(child_node);

  /* 1. <sub_term>.inh = <factor>.syn */
  struct treeNode *next_node = nextNonTerminalNode(child_node);
  next_node->inh = child_node->syn;
  traverseParseTree(next_node);

  /* 2.
    IF (<sub_term>.syn == NULL) <term>.syn = <factor>.syn
    ELSE <term>.syn = <sub_term>.syn */
  if (next_node->syn.type == NULL_NODE) {
    curr_node->syn = child_node->syn;
  }
  else {
    next_node->syn.node.ter->is_first = true;
    curr_node->syn = next_node->syn;
  }
}


void case_76(struct treeNode *curr_node){
  /* <sub_term> := <op2> <factor> <sub_term>1 */
  struct treeNode *child_node = curr_node->child;
  traverseParseTree(child_node);

  /* 1. <sub_term>.syn = new TermNode(<sub_term>.inh, <op2>.val, <factor>.syn) */
  struct TermNode *term_node = (struct TermNode *)malloc(sizeof(struct TermNode));
  term_node->ptr1 = newAttribute(curr_node->inh);
  term_node->is_first = false;
  term_node->op = child_node->val;
  child_node = nextNonTerminalNode(child_node);
  traverseParseTree(child_node);
  term_node->ptr2 = newAttribute(child_node->syn);

  curr_node->syn.node.ter = term_node;
  curr_node->syn.type = TERM_NODE;

  /* 2. <sub_term>1.inh = <sub_term>.syn */
  child_node = nextNonTerminalNode(child_node);
  child_node->inh = curr_node->syn;
  traverseParseTree(child_node);

  term_node->ptr3 = newAttribute(child_node->syn);
}


void case_77(struct treeNode *curr_node){
  /* <sub_term> := EPSILON */

  curr_node->syn.node.ter = NULL;
  curr_node->syn.type = NULL_NODE;
  /* <sub_term>.syn = NULL */
}


void case_78(struct treeNode *curr_node){
  /* <factor> := BO <expression> BC */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <factor>.syn  = <expression>.syn */
  child_node = nextNonTerminalNode(child_node);
  curr_node->syn = child_node->syn;
}


void case_79(struct treeNode *curr_node){
  /* <factor> := <var> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <factor>.syn  = <var>.syn */
  curr_node->syn = child_node->syn;
}


void case_80(struct treeNode *curr_node) {
  /* <op1> := PLUS */
  /* <op1>.val = “PLUS” */
  curr_node->val = PLUS;
}


void case_81(struct treeNode *curr_node) {
  /* <op1> := MINUS */
  /* <op1>.val = “MINUS” */
  curr_node->val = MINUS;
}


void case_82(struct treeNode *curr_node) {
  /* <op1> := MUL */
  /* <op1>.val = “MUL” */
  curr_node->val = MUL;
}


void case_83(struct treeNode *curr_node) {
  /* <op1> := DIV */
  /* <op1>.val = “DIV” */
  curr_node->val = DIV;
}


void case_84(struct treeNode *curr_node) {
  /* <logicalOp> := AND */
  /* <logicalOp>.val = “AND” */
  curr_node->val = AND;
}


void case_85(struct treeNode *curr_node) {
  /* <logicalOp> := OR */
  /* <logicalOp>.val = “OR” */
  curr_node->val = OR;
}


void case_86(struct treeNode *curr_node) {
  /* <relationalOp> := LT */
  /* <relationalOp>.val = “LT” */
  curr_node->val = LT;
}


void case_87(struct treeNode *curr_node) {
  /* <relationalOp> := LE */
  /* <relationalOp>.val = “LE” */
  curr_node->val = LE;
}


void case_88(struct treeNode *curr_node) {
  /* <relationalOp> := GT */
  /* <relationalOp>.val = “GT” */
  curr_node->val = GT;
}


void case_89(struct treeNode *curr_node) {
  /* <relatio/nalOp> := GE */
  /* <relationalOp>.val = “GE” */
  curr_node->val = GE;
}


void case_90(struct treeNode *curr_node) {
  /* <relationalOp> := EQ */
  /* <relationalOp>.val = “EQ” */
  curr_node->val = EQ;
}


void case_91(struct treeNode *curr_node) {
  /* <relationalOp> := NE */
  /* <relationalOp>.val = “NE” */
  curr_node->val = NE;
}


void case_92(struct treeNode *curr_node) {
  /* <declareStmt> := DECLARE <idList> COLON <dataType> SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct DeclareStmtNode *dec_stmt_node = (struct DeclareStmtNode *) malloc(sizeof(struct DeclareStmtNode));
  child_node = nextNonTerminalNode(child_node);
  dec_stmt_node->ptr1 = child_node->syn.node.id_lis;
  child_node = nextNonTerminalNode(child_node);
  dec_stmt_node->ptr2 = newAttribute(child_node->syn);

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
  cond_stmt_node->ptr1 = newLeafNode(IDENTIFIER, &(child_node->symbol.terminal.lexeme.str), child_node->symbol.terminal.line_no);
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
  curr_node->syn.type = NULL_NODE;
}


void case_97(struct treeNode *curr_node) {
  /* <value> := NUM */
  struct treeNode *child_node = curr_node->child;

  /* <value> = new LeafNode(NUM, NUM.val) */
  curr_node->syn.node.lea = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num), child_node->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_98(struct treeNode *curr_node) {
  /* <value> := TRUE */

  /* <value> = new LeafNode(BOOL, "TRUE") */
  bool val = true;
  curr_node->syn.node.lea = newLeafNode(BOOLEAN_, &(val), curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_99(struct treeNode *curr_node) {
  /* <value> := FALSE */

  /* <value> = new LeafNode(BOOL, "FALSE") */
  bool val = false;
  curr_node->syn.node.lea = newLeafNode(BOOLEAN_, &(val), curr_node->child->symbol.terminal.line_no);
  curr_node->syn.type = LEAF_NODE;
}


void case_100(struct treeNode *curr_node) {
  /* <default> := DEFAULT COLON <statements> BREAK SEMICOL */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);
  child_node = nextNonTerminalNode(child_node);
  /* <default>.syn = <statements>.syn */
  curr_node->syn.node.stm = child_node->syn.node.stm;
  curr_node->syn.type = STATEMENT_NODE;
}


void case_101(struct treeNode *curr_node) {
  /* <default> :=  EPSILON */

  /* <default>.syn = NULL */
  curr_node->syn.node.stm = NULL;
  curr_node->syn.type = NULL_NODE;
}


void case_102(struct treeNode *curr_node) {
  /* <iterativeStmt> := FOR BO ID IN <range> BC START <statements> END */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct ForIterativeStmtNode *for_iter_node = (struct ForIterativeStmtNode *) malloc(sizeof(struct ForIterativeStmtNode));
  child_node = child_node->next->next;
  for_iter_node->ptr1 = newLeafNode(IDENTIFIER, child_node->symbol.terminal.lexeme.str, child_node->symbol.terminal.line_no);
  child_node = nextNonTerminalNode(child_node);
  for_iter_node->ptr2 = child_node->syn.node.ran;
  child_node = nextNonTerminalNode(child_node);
  for_iter_node->ptr3 = child_node->syn.node.stm;

  /* <iterativeStmt>.syn = new ForIterativeStmtNode(new LeafNode(ID, ID.entry), <range>.syn, <statements>.syn) */
  curr_node->syn.node.for_ite_stm = for_iter_node;
  curr_node->syn.type = FOR_ITERATIVE_STMT_NODE;
}


void case_103(struct treeNode *curr_node) {
  /* <iterativeStmt> := WHILE BO <expression> BC START <statements> END */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  struct WhileIterativeStmtNode * while_iter_node = (struct WhileIterativeStmtNode *) malloc(sizeof(struct WhileIterativeStmtNode));
  child_node = nextNonTerminalNode(child_node);
  while_iter_node->ptr1 = newAttribute(child_node->syn);
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
  range_node->ptr1 = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num), child_node->symbol.terminal.line_no);
  child_node = child_node->next->next;
  range_node->ptr2 = newLeafNode(NUM, &(child_node->symbol.terminal.lexeme.num), child_node->symbol.terminal.line_no);
  range_node->ptr2->value.num += 1;

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

struct LeafNode *newLeafNode(int type, void *data, int line_num) {
  int datalen = 0;
  struct LeafNode *new_node = (struct LeafNode *) malloc(sizeof(struct LeafNode));
  new_node->type = type;
  new_node->line_number = line_num;
  if (data != NULL) {
    switch (new_node->type) {
      case (NUM):
        memcpy(&(new_node->value.num), data, sizeof(int));
        break;
      case (RNUM):
        memcpy(&(new_node->value.rnum), data, sizeof(float));
        break;
      case (IDENTIFIER):
        datalen = strlen((char *) data);
        assert(datalen <= 20);
        strcpy(new_node->value.entry, (char *)data);
        break;
      case (INTEGER):
      case (REAL):
      case (BOOLEAN_):
      case (TRUE_):
      case (FALSE_):
        break;
      default:
        free(new_node);
        die("Invalid node type.\n");
    }
  }
  else
    new_node->nullData = true;
  new_node->scope = NULL;  /* Will be filled when populating the symbol table. */
  return new_node;
}

struct Attribute *newAttribute(struct Attribute attr) {
  struct Attribute *new_attr = (struct Attribute *) malloc(sizeof(struct Attribute));
  new_attr->node = attr.node;
  new_attr->type = attr.type;
  return new_attr;
}
/* END : Utility Functions */


void printAttribute(struct Attribute *attr) {
  if (attr->type == INPUT_NODE) printf("INPUT STATEMENT\n");
  else if (attr->type == PRINT_NODE) printf("PRINT STATEMENT\n");
  else if (attr->type == ASSIGN_STMT_NODE) printf("ASSIGNMENT STATEMENT\n");
  else if (attr->type == MODULE_REUSE_STMT_NODE) printf("MODULE REUSE STATEMENT\n");
  else if (attr->type == DECLARE_STMT_NODE) printf("DECLARE STATEMENT\n");
  else if (attr->type == CONDITIONAL_STMT_NODE) printf("CONDITIONAL STATEMENT\n");
  else if (attr->type == FOR_ITERATIVE_STMT_NODE) printf("FOR ITERATIVE STATEMENT\n");
  else if (attr->type == WHILE_ITERATIVE_STMT_NODE) printf("WHILE ITERATIVE STATEMENT\n");
  else if (attr->type == LVALUE_ID_NODE) printf("lvalue id\n");
  else if (attr->type == LVALUE_ARR_NODE) printf("lvalue arr\n");
}


void printLeaf(struct LeafNode *leaf) {
  if (leaf->type == NUM) printf("%d", leaf->value.num);
  else if (leaf->type == RNUM) printf("%f", leaf->value.rnum);
  else if (leaf->type == IDENTIFIER) printf("%s", (char *)leaf->value.entry);
  else if (leaf->type == INTEGER) printf("INTEGER");
  else if (leaf->type == REAL) printf("REAL");
  else if (leaf->type == BOOLEAN_) printf("BOOLEAN");
  else if (leaf->type == TRUE_) printf("TRUE");
  else if (leaf->type == FALSE_) printf("FALSE");
}


void printExpression(struct Attribute *expr) {
  switch(expr->type) {
    case U_NODE:
      printf("%s ", terminalStringRepresentations[expr->node.u->op]);
      printExpression(expr->node.u->ptr1);
      break;

    case N7_NODE:
      printExpression(expr->node.n7->ptr1);
      printf("%s ", terminalStringRepresentations[expr->node.n7->logicalOp]);
      printExpression(expr->node.n7->ptr2);
      break;

    case N8_NODE:
      printExpression(expr->node.n8->ptr1);
      printf("%s ", terminalStringRepresentations[expr->node.n8->relationalOp]);
      printExpression(expr->node.n8->ptr2);
      break;

    case ARITHMETIC_EXPR_NODE:
      if (expr->node.ari_exp->is_first)
        printExpression(expr->node.ari_exp->ptr1);
      printf("%s ", terminalStringRepresentations[expr->node.ari_exp->op]);
      printExpression(expr->node.ari_exp->ptr2);
      printExpression(expr->node.ari_exp->ptr3);
      break;

    case TERM_NODE:
      if (expr->node.ter->is_first)
        printExpression(expr->node.ter->ptr1);
      printf("%s ", terminalStringRepresentations[expr->node.ter->op]);
      printExpression(expr->node.ter->ptr2);
      printExpression(expr->node.ter->ptr3);
      break;

    case ARRAY_NODE:
      printLeaf(expr->node.arr->ptr1);
      printf(" ");
      printLeaf(expr->node.arr->ptr2);
      printf(" ");
      break;

    case LEAF_NODE:
      printLeaf(expr->node.lea);
      printf(" ");
      break;
    
    case NULL_NODE:
      break;

    default:
      printf("Invalid Expression Type %d! ", expr->type);
      break;
  }
}


void printAssignStmt(struct AssignStmtNode *assign_stmt) {
  printf("LHS: %s", (char *)assign_stmt->ptr1->value.entry);
  if (assign_stmt->ptr2->type == LVALUE_ID_NODE) {
    printf(" | RHS: ");
    printExpression(assign_stmt->ptr2->node.lva_id->ptr1);
  }
  else {
    printf(" %d", assign_stmt->ptr2->node.lva_arr->ptr1->value.num);
    printf(" | RHS: ");
    printExpression(assign_stmt->ptr2->node.lva_arr->ptr2);
  }
}


void printIdList(struct IdListNode *id_list_node) {
  while(id_list_node != NULL) {
    printLeaf(id_list_node->ptr1);
    printf(" ");
    id_list_node = id_list_node->ptr2;
  }
}


void printModuleReuseStmt(struct ModuleReuseStmtNode *module_reuse_stmt_node) {
  printf("Output: ");
  printIdList(module_reuse_stmt_node->ptr1);
  if (module_reuse_stmt_node->ptr1 == NULL)
    printf("NONE ");
  printf("| Module: ");
  printLeaf(module_reuse_stmt_node->ptr2);
  printf(" | Input: ");
  printIdList(module_reuse_stmt_node->ptr3);
}


void printDeclareStmt(struct DeclareStmtNode *decl_stmt_node) {
  /* print format: list of identifiers --> data type
    example: a, b, c --> INTEGER  */
  printf("Id: ");
  printIdList(decl_stmt_node->ptr1);
  printf("| Type: ");
  struct Attribute *data_type = decl_stmt_node->ptr2;
  switch(data_type->type) {
    case ARRAY_TYPE_NODE:
      printLeaf(data_type->node.arr_typ->ptr1);
      printf(" ARRAY | Start Index: ");
      printLeaf(data_type->node.arr_typ->ptr2->ptr1);
      printf("| End Index: ");
      printLeaf(data_type->node.arr_typ->ptr2->ptr2);
      break;
    case LEAF_NODE:
      printLeaf(data_type->node.lea);
      break;
    default:
      printf("Invalid Declare Statement Node with Type %d", data_type->type);
      break;
  }
}


void printConditionalStmt(struct ConditionalStmtNode *cond_stmt_node) {
  printf("Switch Variable: ");
  printLeaf(cond_stmt_node->ptr1);
  printf("\n\n");
  struct CaseStmtNode *case_stmt_node = cond_stmt_node->ptr2;
  while(case_stmt_node != NULL) {
    printf("Case Value: ");
    printLeaf(case_stmt_node->ptr1);
    printf("\n\n-- Start Case Statements --\n\n");
    printStatementList(case_stmt_node->ptr2);
    printf("-- End Case Statements --");
    case_stmt_node = case_stmt_node->ptr3;
  }
  if (cond_stmt_node->ptr3 != NULL) {
    printf("\n\nDefault: ");
    printStatementList(cond_stmt_node->ptr3);
  }
}


void printForIterativeStmt(struct ForIterativeStmtNode *for_stmt_node) {
 printf("Loop Variable: ");
 printLeaf(for_stmt_node->ptr1);
 printf(" | Start Index: ");
 printLeaf(for_stmt_node->ptr2->ptr1);
 printf(" | End Index: ");
 printLeaf(for_stmt_node->ptr2->ptr2);
 printf("\n\n-- Start For Statements --\n\n");
 printStatementList(for_stmt_node->ptr3);
 printf("-- End For Statements --");
}


void printWhileIterativeStmt(struct WhileIterativeStmtNode *while_stmt_node) {
 printf("Loop Expression: ");
 printExpression(while_stmt_node->ptr1);
 printf("\n\n-- Start While Statements --\n\n");
 printStatementList(while_stmt_node->ptr2);
 printf("-- End While Statements --");
}


void printStatement(struct StatementNode *stmt_node) {
  printAttribute(stmt_node->ptr1);
  switch (stmt_node->ptr1->type) {
    case ASSIGN_STMT_NODE:
      printAssignStmt(stmt_node->ptr1->node.agn_stm);
      break;
    case MODULE_REUSE_STMT_NODE:
      printModuleReuseStmt(stmt_node->ptr1->node.mod_reu_stm);
      break;
    case DECLARE_STMT_NODE:
      printDeclareStmt(stmt_node->ptr1->node.dec_stm);
      break;
    case PRINT_NODE:
      if (stmt_node->ptr1->node.pri->ptr1->type == LEAF_NODE) {
        printf("Output Variable: ");
        printLeaf(stmt_node->ptr1->node.pri->ptr1->node.lea);
      }
      else {
        printf("Output Array Id: ");
        printLeaf(stmt_node->ptr1->node.pri->ptr1->node.arr->ptr1);
        printf(" | Element Index: ");
        printLeaf(stmt_node->ptr1->node.pri->ptr1->node.arr->ptr2);
      }
      break;
    case INPUT_NODE:
      printf("Input Variable: ");
      printLeaf(stmt_node->ptr1->node.inp->ptr1);
      break;
    case CONDITIONAL_STMT_NODE:
      printConditionalStmt(stmt_node->ptr1->node.con_stm);
      break;
    case FOR_ITERATIVE_STMT_NODE:
      printForIterativeStmt(stmt_node->ptr1->node.for_ite_stm);
      break;
    case WHILE_ITERATIVE_STMT_NODE:
      printWhileIterativeStmt(stmt_node->ptr1->node.whi_ite_stm);
      break;
    default:
      printf("Not Implemented Statement Type %d ...", stmt_node->ptr1->type);
      break;
  }
  printf("\n\n");
  stmt_node = stmt_node->ptr2;
}


void printStatementList(struct StatementNode *statement_node) {
  while (statement_node != NULL) {
    printStatement(statement_node);
    statement_node = statement_node->ptr2;
  }
}


void printAST() {
  printf("\n");

  struct ProgramNode *pro = &AST;
  if (pro->ptr1 == NULL) printf("No Module Declarations\n");
  if (pro->ptr2 == NULL) printf("No Modules Definitions Before Driver\n");
  if (pro->ptr3 == NULL) printf("No Statments in Driver Module\n");
  if (pro->ptr4 == NULL) printf("No Module Declarations After Driver\n");
  printf("\n");

  // Print Driver module statements
  if (pro->ptr2 != NULL) printStatementList(pro->ptr2->ptr1->ptr4);
  if (pro->ptr3 != NULL) printStatementList(pro->ptr3);
  if (pro->ptr4 != NULL) printStatementList(pro->ptr4->ptr1->ptr4);
}
