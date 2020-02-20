#include "utils.h"
#include "lexer.h"
#include "parserDef.h"

#ifndef PARSER_H
#define PARSER_H

struct hashMap *getTerminalMap();
struct hashMap *getNonTerminalMap();
void computeFirstAndFollow();
void computeParseTable();

// ComputeFirstAndFollowSets (grammar G, FirstAndFollow  F)
// createParseTable(FirstAndFollow F, table T)
// parseInputSourceCode(char *testcaseFile, table T)
// printParseTree(parseTree PT, char *outfile)

#endif
