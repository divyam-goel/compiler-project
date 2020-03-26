#ifndef ASTDEF_H

#define ASTDEF_H

#include "grammar.h"
#include "lexerDef.h"

/* ----- BEGIN UNIONS ----- */
union DataTypeUnion {
  struct LeafNode *leaf_node;
  struct ArrayTypeNode *array_type_node;
};

union ExtendedVarUnion {
  struct LeafNode *leaf_node;
  struct ArrayNode *array_node;
};

union WhichStmtUnion {
  struct LvalueIDNode *lvalue_id_node;
  struct LvalueARRNode *lvalue_arr_node;
};

union ExpressionUnion { // ---> use when <expression>.syn is involved!
  struct N7Node *n7_node;
  struct N8Node *n8_node;
  struct ArithmeticExprNode *aritimethic_expr_node;
  struct TermNode *term_node;
  struct LeafNode *leaf_node;
  struct ArrayNode *array_node;
};

union VarUnion {
    struct LeafNode *ptr_node;
    struct ArrayNode *arr_node;
};

union NewExpressionUnion {
  union ExpressionUnion expression;
  struct UNode *u_node;
};

union FactorUnion {
  union ExpressionUnion expr;
  union VarUnion var;
};

union ASTNodesUnion {
  struct ProgramNode *pro;
  struct ModuleDeclarationNode *mod_dec;
  struct OtherModuleNode *oth_mod;
  struct ModuleNode *mod;
  struct InputPlistNode *inp_pli;
  struct OutputPlistNode *out_pli;
  struct LeafNode *lea;
  struct ArrayTypeNode *arr_typ;
  struct DynamicRangeNode *dyn_ran;
  struct StatementNode *stm;
  struct InputNode *inp;
  struct PrintNode *pri;
  struct ArrayNode *arr;
  struct AssignStmtNode *ass_stm;
  struct LvalueIdNode *lva_id;
  struct LvalueARRNode *lva_arr;
  struct ModuleReuseStmtNode *mod_reu_stm;
  struct IdListNode *id_lis;
  struct UNode *u;
  struct N7Node *n7;
  struct N8Node *n8;
  struct ArithmeticExprNode *ari_exp;
  struct TermNode *ter;
  struct DeclareStmtNode *dec_stm;
  struct ConditionalStmtNode *con_stm;
  struct CaseStmtNode *cas_stm;
  struct ForIterativeStmtNode *for_ite_stm;
  struct WhileIterativeStmtNode *whi_ite_stm;
  struct RangeNode *ran;
};

union LeafNodeUnion {
    int num;
    float rnum;
    bool boolean;
    void *entry;  // TEMP: Just until we define SymbolTableNode.
};
/* ----- END UNIONS ----- */

/* ----- BEGIN ENUMS ----- */
enum ASTNodesEnum {
  /* 00 */ PROGRAM_NODE,
  /* 01 */ MODULE_DECLARATION_NODE,
  /* 02 */ OTHER_MODULE_NODE,
  /* 03 */ MODULE_NODE,
  /* 04 */ INPUT_PLIST_NODE,
  /* 05 */ OUTPUT_PLIST_NODE,
  /* 06 */ LEAF_NODE,
  /* 07 */ ARRAY_TYPE_NODE,
  /* 08 */ DYNAMIC_RANGE_NODE,
  /* 09 */ STATEMENT_NODE,
  /* 10 */ INPUT_NODE,
  /* 11 */ PRINT_NODE,
  /* 12 */ ARRAY_NODE,
  /* 13 */ ASSIGN_STMT_NODE,
  /* 14 */ LVALUE_ID_NODE,
  /* 15 */ LVALUE_ARR_NODE,
  /* 16 */ MODULE_REUSE_STMT_NODE,
  /* 17 */ ID_LIST_NODE,
  /* 18 */ U_NODE,
  /* 19 */ N7_NODE,
  /* 20 */ N8_NODE,
  /* 21 */ ARITHMETIC_EXPR_NODE,
  /* 22 */ TERM_NODE,
  /* 23 */ DECLARE_STMT_NODE,
  /* 24 */ CONDITIONAL_STMT_NODE,
  /* 25 */ CASE_STMT_NODE,
  /* 26 */ FOR_ITERATIVE_STMT_NODE,
  /* 27 */ WHILE_ITERATIVE_STMT_NODE,
  /* 28 */ RANGE_NODE
};

enum factorEnum {
    FACTOR_EXPRESSION,
    FACTOR_VAR
};
/* ----- END ENUMS ----- */

/* ----- BEGIN STRUCTS ----- */
struct ProgramNode {
  struct ModuleDeclarationNode *ptr1;
  struct OtherModuleNode *ptr2;
  struct StatementNode *ptr3;
  struct OtherModuleNode *ptr4;
};

struct ModuleDeclarationNode {
  struct LeafNode *ptr1;
  struct ModuleDeclarationNode *ptr2;
};

struct OtherModuleNode {
  struct ModuleNode *ptr1;
  struct OtherModuleNode *ptr2;
};

struct ModuleNode {
  struct LeafNode *ptr1;
  struct InputPlistNode *ptr2;
  struct OutputPlistNode *ptr3;
  struct StatementNode *ptr4;
};

struct InputPlistNode {
  union DataTypeUnion *ptr1;
  enum ASTNodesEnum type;
  struct LeafNode *ptr2;
  struct InputPlistNode *ptr3;
};

struct OutputPlistNode {
  struct LeafNode *ptr1;
  struct LeafNode *ptr2;
  struct OutputPlistNode *ptr3;
};

struct LeafNode {
    enum terminal type; /* An enum TERMINAL value. One of BOOL, ID, NUM, RNUM. */
    union LeafNodeUnion value;
};

struct ArrayTypeNode {
    struct LeafNode *ptr1;
    struct DynamicRangeNode *ptr2;
};

struct DynamicRangeNode {
    struct LeafNode *ptr1;  /* Lower bound */
    struct LeafNode *ptr2; /* Upper bound */
};

struct StatementNode {
  union StatementUnion *ptr1;
  enum ASTNodesEnum type;
  struct StatementNode *ptr2;
};

struct InputNode {
  struct LeafNode *ptr1;
};

struct PrintNode {
  union ExtendedVarUnion *ptr1;
};

struct ArrayNode {
  struct LeafNode *ptr1;
  struct LeafNode *ptr2;
};

struct AssignStmtNode {
  struct LeafNode *ptr1;
  union WhichStmtUnion *ptr2;
  enum ASTNodesEnum type;
};


struct LvalueIDNode {
  union newExpressionUnion *ptr1;
  enum ASTNodesEnum type;
};

struct LvalueARRNode {
  struct LeafNode *ptr1; //index
  union newExpressionUnion *ptr2;
  enum ASTNodesEnum type;
};

struct ModuleReuseStmtNode {
  struct IdListNode *ptr1;
  struct IdListNode *ptr2;

};

struct IdListNode {
    struct LeafNode *ptr1;
    struct IdListNode *ptr2;
};

struct UNode {
  enum terminal op;
  union ExpressionUnion ptr1;
};

struct N7Node {
  union ExpressionUnion ptr1;
  enum terminal logicalOp;
  struct N7Node *ptr2;
};

struct N8Node {
    struct ArithmeticExprNode *ptr1; /* The inherited attribute. */
    enum terminal relationalOp;
    union ExpressionUnion ptr2;
};

struct ArithmeticExprNode {
    struct ArithmeticExprNode *ptr1; /* Sub Arithmetic Expression */
    enum terminal op;
    union ExpressionUnion ptr2; /* Term */
};

struct TermNode {
    struct TermNode *ptr1; /* Subterm */
    enum terminal op;
    union FactorUnion ptr2; /* Factor - If TermNode* == NULL then we use this.*/
};

struct DeclareStmtNode {
    union DataTypeUnion ptr1;
    enum ASTNodesEnum type;  /* For the above union. */
    struct IdListNode *ptr2;
};

struct ConditionalStmtNode {
    struct LeafNode *ptr1;
    struct CaseStmtNode *ptr2;
    struct StatementNode *ptr3;
};

struct CaseStmtNode {
    struct LeafNode *ptr1;
    struct StatementNode *ptr2;
    struct CaseStmtNode *ptr3;
};

struct ForIterativeStmtNode {
    struct LeafNode *ptr1;
    struct RangeNode *ptr2;
    struct StatementNode *ptr3;
};

struct WhileIterativeStmtNode {
    struct N7Node *ptr1_0;
    struct AnyTerm *ptr1_1;  /* If N7Node* == NULL then we use this. */
    struct StatementNode *ptr2;
};

struct RangeNode {
    struct LeafNode *ptr1;  /* Lower bound */
    struct LeafNode *ptr2; /* Upper bound */
};

struct Attribute {
  union ASTNodesUnion node;
  enum ASTNodesEnum type;
};

/* ----- BEGIN STRUCTS ----- */

/* ----- BEGIN PARSER STRUCTURES ----- */
enum typeOfSymbol {
	TERMINAL,
	NON_TERMINAL
};

union nodeValue {
	struct symbol terminal;
	enum nonTerminal non_terminal;
};

struct treeNode {
	int rule_number;  /* Use this to index into grammar[]. */
	union nodeValue symbol;
	enum typeOfSymbol flag;
	struct treeNode *child;
	struct treeNode *next;
  struct Attribute syn;
};

struct parseTree {
	struct treeNode *head;
};
/* ----- END PARSER STRUCTURES ----- */

#endif