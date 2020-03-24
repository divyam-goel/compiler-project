#include "ast.h"

extern struct parseTree PT;
struct ProgramNode AST;

/* BEGIN : Function Declarations */
void traverseChildren(struct treeNode *curr_node);
struct treeNode *nextNonTerminalNode(struct treeNode *curr_node);
struct LeafNode *newLeafNode(enum terminal type, void *data);
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

    case 03: // <moduleDeclarations> := EPSILON
      case_3(curr_node);
      break;

    case 04: // <moduleDeclaration> := DECLARE MODULE ID SEMICOL
      case_4(curr_node);
      break;

    case 05: // <otherModules>  := <module> <otherModules>
      case_5(curr_node);
      break;

    case 06: // <otherModules>  := EPSILON
      case_6(curr_node);
      break;

    case 07: // <driverModule> := DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
      case_7(curr_node);
      break;

    case 08: // <module> := DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
      case_8(curr_node);
      break;

    case 09: // <ret> := RETURNS SQBO <output_plist> SQBC SEMICOL
      case_9(curr_node);
      break;

    case 10: // <ret> := EPSILON
      case_10(curr_node);
      break;

    case 11: // <input_plist> := ID COLON <dataType> <sub_input_plist>
      case_11(curr_node);
      break;

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



void case_1(struct treeNode *curr_node) {
  // <program> := <moduleDeclarations> <otherModules> <driverModule> <otherModules>

  struct tree_node *child_node = curr_node->child;
  traverseChildren(child_node);

  // define and populate structure ProgramNode
  struct ProgramNode *prog_node = (struct ProgramNode *) malloc(sizeof(ProgramNode));
  prog_node->ptr1 = child_node->syn.node.mod_dec;
  child_node = nextNonTerminalNode(child_node);
  prog_node->ptr2 = next_node->syn.node.oth_mod;
  child_node = nextNonTerminalNode(child_node);
  prog_node->ptr3 = child_node->syn.node.stm;
  child_node = nextNonTerminalNode(child_node);
  prog_node->ptr4 = child_node->syn.node.oth_mod;

  curr_node->syn.node.pro = prog_node;
  curr_node->syn.type = PROGRAM_NODE;
}


void case_2(struct treeNode *curr_node) {
  // <moduleDeclarations> := <moduleDeclaration> <moduleDeclarations>

  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  // define and populate structure ModuleDeclarationNode
  struct ModuleDeclarationNode *mod_dec_node = (struct ModuleDeclarationNode *) malloc(sizeof(ModuldeDeclarationNode));
  mod_dec_node->ptr1 = child_node->syn.node.lea;
  child_node = nextNonTerminalNode(child_node);
  mod_dec_node->ptr2 = next_node->syn.node.mod_dec;

  // <moduleDeclarations>.syn = new ModuleDeclarationNode(<moduleDeclaration>.syn, <moduleDeclarations>1.syn)
  curr_node->syn.node.mod_dec = mod_dec_node;
  curr_node->syn.type = MODULE_DECLARATION_NODE;
}


void case_3(struct treeNode *curr_node) {
  // <moduleDeclarations> := EPSILON

  // <moduleDeclarations>.syn = NULL
  curr_node->syn.node.mod_dec = NULL;
}


void case_4(struct treeNode *curr_node) {
  // <moduleDeclaration> := DECLARE MODULE ID SEMICOL

  struct treeNode *child_node = curr_node->child_node;
  traverseChildren();

  // define and populate structure LeafNode
  struct LeafNode *leaf_node = (struct LeafNode *) malloc(sizeof(struct LeafNode));
  leaf_node->type = ID;
  // TO DO: leaf_node->val.entry

  // <moduleDeclaration>.syn = new LeafNode(ID, ID.entry)
  curr_node->syn.lea = leaf_node;
}


void case_5(struct treeNode *curr_node) {
  // <otherModules>  := <module> <otherModules>

  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  // define and populate structure OtherModuleNode
  struct OtherModuleNode *oth_mod_node = (struct OtherModuleNode *) malloc(sizeof(struct OtherModuleNode));
  oth_mod_node->ptr1 = child_node->syn.mod;
  child_node = child_node->next;
  oth_mod_node->ptr1 = child_node->syn.oth_mod;

  // <otherModules>.syn = new OtherModuleNode(<module>.syn, <otherModules>.syn)
  curr_node->syn.oth_mod = oth_mod_node;
}


void case_6(struct treeNode *curr_node) {
  // <otherModules>  := EPSILON

  // <otherModules>.syn = NULL
  curr_node->syn.oth_mod = NULL;
}


void case_7(struct treeNode *curr_node) {
  // <driverModule> := DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>

  struct treeNode *child_node = curr_node->child_node;
  traverseChildren();

  child_node = nextNonTerminalNode(child_node);
  // <driverModule>.syn = <moduleDef>.syn
  curr_node->syn.stm = child_node->syn.stm;
}


void case_8(struct treeNode *curr_node) {
  // <module> := DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>

  struct treeNode *child_node = curr_node->child_node;
  traverseChildren();

  // define and populate structure ModuleNode
  struct ModuleNode *module_node = (struct ModuleNode *) malloc(sizeof(struct ModuleNode));
  child_node = child_node->next->next;
  module_node->ptr1 = newLeafNode(ID, child_node);
  child_node = nextNonTerminalNode(child_node);
  module_node->ptr2 = child_node->syn.inp_pli;
  child_node = nextNonTerminalNode(child_node);
  module_node->ptr3 = child_node->syn.out_pli;
  child_node = nextNonTerminalNode(child_node);
  module_node->ptr4 = child_node->syn.stm;

  // <module>.syn = new ModuleNode(new LeafNode(ID, ID.entry), <input_plist>.syn, <ret>.syn, <moduleDef>.syn)
  curr_node->syn.mod = module_node;
}


void case_9(struct treeNode *curr_node) {
  // <ret> := RETURNS SQBO <output_plist> SQBC SEMICOL

  struct treeNode *child_node = curr_node->child_node;
  traverseChildren();

  child_node = nextNonTerminalNode(child_node);
  // <ret>.syn = <output_plist>.syn
  curr_node->syn.out_pli = child_node->syn.out_pli;
}

void case_10(struct treeNode *curr_node) {}

void case_11(struct treeNode *curr_node) {}

void case_17(struct treeNode *curr_node){
  // <dataType> := INTEGER
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  //define leafNode structure by using constructor - TO BE DEFINED
  struct LeafNode *leaf_node = newLeafNode(TYPE,"INT");

  // <dataType>.syn = new LeafNode(TYPE, “INT”)
  curr_node->syn.lea = leaf_node;
}

void case_18(struct treeNode *curr_node){
  // <dataType> := REAL
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  //define leafNode structure by using constructor - TO BE DEFINED
  struct LeafNode *leaf_node = newLeafNode(TYPE,"REAL");

  // <dataType>.syn = new LeafNode(TYPE, "REAL")
  curr_node->syn.lea = leaf_node;
}

void case_19(struct treeNode *curr_node){
  // <dataType> := BOOLEAN
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  //define leafNode structure by using constructor - TO BE DEFINED
  struct LeafNode *leaf_node = newLeafNode(TYPE,"BOOLEAN");

  // <dataType>.syn = new LeafNode(TYPE, "BOOLEAN")
  curr_node->syn.lea = leaf_node;
}

void case_20(struct treeNode *curr_node){
  // <dataType> := ARRAY SQBO <dynamic_range> SQBC OF <type>
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  // define structure of type ArrayTypeNode
  struct ArrayTypeNode *array_type_node = (struct ArrayTypeNode *) malloc(sizeof(ArrayTypeNode));
  child_node = nextNonTerminalNode(child_node);
  array_type_node->ptr1 = child_node->syn.lea;
  child_node = nextNonTerminalNode(child_node);
  array_type_node->ptr2 = child_node->syn.dyn_ran;

  // <dataType>.syn = new ArrayTypeNode(<type>.syn, <dynamic_range>.syn)
  curr_node->syn.arr_typ  = array_type_node;

}

void case_21(struct treeNode *curr_node){
  // <dynamic_range> := <index> RANGEOP <index>
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  // define structure of type DynamicRangeNode
  struct DynamicRangeNode *dynamic_type_node = (struct DynamicRangeNode *) malloc(sizeof(DynamicRangeNode));
  dynamic_type_node->ptr1 = child_node->syn.lea;
  child_node = nextNonTerminalNode(child_node);
  dynamic_type_node->ptr2 = child_node->syn.lea;

  // <dynamic_range>.syn = new DynamicRangeNode(<index>1.syn, <index>2.syn)
  curr_node->syn.dyn_ran  = dynamic_type_node;

}

void case_22(struct treeNode *curr_node){
  // <type> := INTEGER
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  //define leafNode structure by using constructor - TO BE DEFINED
  struct LeafNode *leaf_node = newLeafNode(TYPE,"INT");

  // <type>.syn = new LeafNode(TYPE, “INT”)
  curr_node->syn.lea = leaf_node;

}

void case_23(struct treeNode *curr_node){
  // <type> := REAL
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  //define leafNode structure by using constructor - TO BE DEFINED
  struct LeafNode *leaf_node = newLeafNode(TYPE,"REAL");

  // <type>.syn = new LeafNode(TYPE, "REAL")
  curr_node->syn.lea = leaf_node;
}

void case_24(struct treeNode *curr_node){
  // <type> := BOOLEAN
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  //define leafNode structure by using constructor - TO BE DEFINED
  struct LeafNode *leaf_node = newLeafNode(TYPE,"BOOLEAN");

  // <type>.syn = new LeafNode(TYPE, "BOOLEAN")
  curr_node->syn.lea = leaf_node;
}

void case_25(struct treeNode *curr_node){
  // <moduleDef> := START <statements> END
  struct treeNode *child_node = curr_node->child_node;
  traverseChildren(child_node);

  child_node = child_node->next;

  // <moduleDef>.syn = <statements>.syn
  curr_node.syn.
}


void case_103(struct treeNode *curr_node) {
    // <iterativeStmt> := WHILE BO <expression> BC START <statements> END

    struct WhileIterativeStmtNode *new_node = (struct WhileIterativeStmtNode *) malloc(sizeof(WhileIterativeStmtNode));
    struct treeNode *child_node = curr_node->child_node;
    nextNonTerminalNode(child_node);
    new_node->ptr1 =
}


void case_104(struct treeNode *curr_node) {
    // <range> := NUM1 RANGEOP NUM2

    // define and populate structure RangeNode
    struct RangeNode *new_node = (struct RangeNode *) malloc(sizeof(RangeNode));
    struct treeNode child_node = curr_node->child_node;
    new_node->ptr1 = newLeafNode(INT, child_node->symbol.terminal.lexeme.num);
    child_node = child_node->next->next;
    new_node->ptr2 = newLeafNode(INT, child_node->symbol.terminal.lexeme.num);

    // <range>.syn = new RangeNode(new LeafNode(NUM1, NUM1.val), new LeafNode(NUM2, NUM2.val))
    curr_node->syn = new_node;
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

struct LeafNode *newLeafNode(enum terminal type, void *data) {
    int datalen = 0;
    struct LeafNode *new_node = (struct LeafNode *) malloc(sizeof(struct LeafNode));
    new_node->type = type;
    switch (node_type) {
        case (BOOL):
            new_node->value = malloc(sizeof(bool));
            memcpy(new_node, data, sizeof(bool));
            break;
        case (NUM):
            new_node->value = malloc(sizeof(int));
            memcpy(new_node, data, sizeof(int));
            break;
        case (RNUM):
            new_node->value = malloc(sizeof(float));
            memcpy(new_node, data, sizeof(float));
            break;
        case (ID):
            datalen = strlen(data);
            new_node->value = malloc(datalen + 1);  /* +1 for the '\0' */
            strcpy(new_node->value, (char *)data);
            break;
        default:
            free(new_node);
            die("Invalid node type.\n");
    }
    return new_node;
}
/* END : Utility Functions */

void createAST() {
  traverseParseTree(PT.head)
}
