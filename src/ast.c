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


void case_27(struct treeNode *curr_node){
  /* <statements> := EPSILON */

  /* <statements>.syn = NULL */
  curr_node->syn.type = STATEMENT_NODE;
  curr_node->syn.node.stm = NULL;
}


void case_28(struct treeNode *curr_node){
  /* <statement> := <ioStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <ioStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_29(struct treeNode *curr_node){
  /* <statement> := <simpleStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <simpleStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_30(struct treeNode *curr_node){
  /* <statement> := <declareStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <declareStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_31(struct treeNode *curr_node){
  /* <statement> := <conditionalStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <conditionalStmt>.syn */
  curr_node->syn = child_node->syn;
}


void case_32(struct treeNode *curr_node){
  /* <statement> := <iterativeStmt> */
  struct treeNode *child_node = curr_node->child;
  traverseChildren(child_node);

  /* <statement>.syn = <iterativeStmt>.syn */
  curr_node->syn = child_node->syn;
}


/* BEGIN : Utility Functions */
void traverseChildren(struct treeNode *curr_node) {
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
