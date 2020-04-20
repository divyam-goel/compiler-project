#ifndef SEMANTICCHECK_H

#define SEMANTICCHECK_H

#include "astDef.h"


enum terminal leafType(struct LeafNode *leaf);
enum terminal arrayType(struct ArrayNode *array);
enum terminal attributeType(struct Attribute *attribute);
enum terminal logicalExpressionType(struct N7Node *logical_expression);
enum terminal relationalExpressionType(struct N8Node *relational_expression);
enum terminal ArithmeticExpressionType(struct ArithmeticExprNode *arithmetic_expression);
enum terminal TermExpressionType(struct TermNode *term_expression);
enum terminal expressionType(struct Attribute *expression);
bool checkInExpression(char *entry, struct Attribute *loop_expression);
void assignmentTypeChecker(struct AssignStmtNode *assignment_node);
void moduleReuseTypeChecker(struct ModuleReuseStmtNode *module_reuse_node);
void conditionalSemanticChecker(struct ConditionalStmtNode *conditional_node);
void forIterationSemanticChecker(struct ForIterativeStmtNode *for_iter_node);
void whileIterationSemanticChecker(struct WhileIterativeStmtNode *while_iter_node);
void statementSemanticChecker(struct StatementNode *statement_node);
void statementListSemanticChecker(struct StatementNode *statement_node);
void moduleDefinitionSemanticChecker(struct StatementNode *statement_node);
void modulesSemanticChecker(struct OtherModuleNode *other_module_node);
void semanticChecker(struct ProgramNode *AST);

#endif
