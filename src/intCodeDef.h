#ifndef INTCODEDEF_H

#define INTCODEDEF_H

#include "utils.h"

typedef enum IntermediateCodeOperation {
  /* arithmetic */
  icADD_INT, icSUB_INT, icMUL_INT, icDIV_INT,
  icADD_REAL, icSUB_REAL, icMUL_REAL, icDIV_REAL,

  /* utility arithmetic */
  icINC, icDEC,

  /* logic */
  icAND, icOR,

  /* comparison */
  icEQ, icNE, icLT, icGT, icLE, icGE,

  /* unary */
  icPLUS, icMINUS,

  /* data */
  icMOV,

  /* control */
  icJUMP, icJUMPZ, icJUMPNZ, icCALL,

  /* others */
  icLABEL, icERR
} ICOp;


typedef struct IntermediateCodeArrayAddress {
  void *var;
  struct IntermediateCodeAddress *idx;
}ICArrayAddr;


typedef union IntermediateCodeAddressUnion {
  int num;
  float rnum;
  bool boolean;
  void *symbol;
  ICArrayAddr array;
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
