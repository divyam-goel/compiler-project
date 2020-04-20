#ifndef INTCODEDEF_H

#define INTCODEDEF_H

#include "utils.h"

#define IC_MAX_LABEL_LENGTH 255
#define IC_MAX_LABEL_BUFFER_SIZE 256

typedef enum IntermediateCodeOperation {
  /* arithmetic */
  icADD_INT, icSUB_INT, icMUL_INT, icDIV_INT,
  icADD_REAL, icSUB_REAL, icMUL_REAL, icDIV_REAL,

  /* utility arithmetic */
  icINC, icDEC,

  /* logic */
  icAND, icOR, //12

  /* comparison */
  icEQ, icNE, icLT, icGT, icLE, icGE, //18

  /* unary */
  icPLUS, icMINUS, //20

  /* data */
  icMOV, icPRINT, icGET, //23

  /* control */
  icJUMP, icJUMPZ, icJUMPNZ, //26

  /* array */
  icARRAY, //27

  /* module resuse */
  icCALL, icENTER, icRET, // 30
  icPUSH, icPOP, icPUSH_SHADOW, icPOP_SHADOW,

  /* others */
  icLABEL, icErrARRAY
} ICOp;


typedef struct IntermediateCodeArrayAddress {
  void *var;
  struct IntermediateCodeAddress *idx;
} ICArrayAddr;


typedef union IntermediateCodeAddressUnion {
  int num;
  float rnum;
  bool boolean;
  void *symbol;
  ICArrayAddr array;
} ICAddrUnion;


typedef struct IntermediateCodeAddress {
  enum terminal type;
  bool is_label;
  ICAddrUnion value;
} ICAddr;


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
