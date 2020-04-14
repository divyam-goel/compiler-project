#ifndef INTCODE_H

#define INTCODE_H

#include "st.h"
#include "semanticCheck.h"

void icLeafNode(struct LeafNode *leaf);
void icUnaryExpr(struct UNode *unary_expression);
void icLogicalExpression(struct N7Node *logical_expression);
void icRelationalExpression(struct N8Node *relational_expression);
void icArithmeticlExpression(struct ArithmeticExprNode *arithmetic_expression);
void icTermExpression(struct TermNode *term_expression);
void icExpression(struct Attribute *expression);
void icAssignmentStatement(struct AssignStmtNode *assignment);
void icConditionalStatement(struct ConditionalStmtNode *conditional);
void icForIterativeStatement(struct ForIterativeStmtNode *for_iteration);
void icWhileIterativeStatement(struct WhileIterativeStmtNode *while_iteration);
void icStatement(struct Attribute *attribute_node);
void icStatementList(struct StatementNode *statement_node);
void generateIntermediateCode(struct ProgramNode *AST);

void printICAddress(ICAddr ic_addr);
void printICInstruction(ICInstr *ic_instr);
void printICInstructionList(ICInstr *ic_instr);

#endif
