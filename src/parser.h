#include "utils.h"
#include "lexer.h"
#include "parserDef.h"

#ifndef PARSER_H
#define PARSER_H

struct hashMap *getTerminalMap();
struct hashMap *getNonTerminalMap();
struct hashMap *getTerminalLiteralMap();

struct rhsNode *newRule(
    enum nonTerminal non_terminal,
    enum terminal terminal,
    enum typeOfSymbol flag);
int extractRules(char *line, int ruleNumber);
void loadGrammar(const char* filename);
void printGrammar();

void initializeFirstAndFollow();
void computeFirstAndFollowSets();

void intializeParseTable();
void createParseTable();

void parseInputSourceCode(char *testcaseFile);
void printParseTree();
void printErrorMessage(struct symbol symbol);

// ComputeFirstAndFollowSets (grammar G, FirstAndFollow  F)
// createParseTable(FirstAndFollow F, table T)
// parseInputSourceCode(char *testcaseFile, table T)
// printParseTree(parseTree PT, char *outfile)

#endif
