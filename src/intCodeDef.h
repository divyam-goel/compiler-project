#ifndef INTCODEDEF_H

#define INTCODEDEF_H

#include "utils.h"

typedef enum IntermediateCodeOperation {
  /* arithmetic */
  icADD, icSUB, icMUL, icDIV,

  /* logic */
  icAND, icOR,

  /* comparison */
  icEQ, icNE, icLT, icGT, icLE, icGE,

  /* unary */
  icPLUS, icMINUS,

  /* data */
  icCOPY, icLOAD, icSTORE,

  /* control */
  icGOTO, icCALL,

  /* others */
  icLABEL
} ICOp;


typedef union IntermediateCodeAddressUnion {
  int num;
  float rnum;
  bool boolean;
  void *symbol;
} ICAddrUnion;


typedef struct IntermediateCodeAddress {
  enum terminal type;
  ICAddrUnion value;
}ICAddr;


typedef struct IntermediateCodeInstruction {
  ICAddr addr1; /* operand 1 */
  ICAddr addr2; /* operand 2 */
  ICAddr addr3; /* result */
  ICOp op; /* operation */
  struct IntermediateCodeInstruction *next; /* next instruction in sequence */
} ICInstr;


struct IntermediateCode {
  ICInstr *head;
} IC;

#endif
