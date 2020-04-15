#include "intCode.h"

#define MAX_SIZE_INSTR 150
void cgICAddr(char *instr_list, char *addr, ICAddr *ic_addr);
void addLabel(char *instr_list, char *label);
void loadConstReg(char *instr_list, char *reg1, int num);
void instrOneOperand(char *instr_list, char *op, char *reg1); 
void instrTwoOperand(char *instr_list, char *op, char *reg1, char *reg2);
void cgLoadINT(char *instr_list, char *reg, ICAddr *ic_addr);
void cgLoadReal(char *instr_list, char *reg, ICAddr *ic_addr);
void cgStoreINT(char *instr_list, char *reg, ICAddr *ic_addr);
void cgStoreREAL(char *instr_list, char *reg, ICAddr *ic_addr);
void cgADD_SUB_INT(ICInstr *ic_instr);
void cgMUL_INT(ICInstr *ic_instr);
void cgDIV_INT(ICInstr *ic_instr);
void cgMUL_REAL(ICInstr *ic_instr);
void cgDIV_REAL(ICInstr *ic_instr);
void cgINC(ICInstr *ic_instr);
void cgDEV(ICInstr *ic_instr);
void cgSTOREVALUE_INT(ICInstr *ic_instr);
void cgRelationalOp(ICInstr *ic_instr);
void cgLogicalOp(ICInstr *ic_instr);
void cgPLUS_MINUS(ICInstr *ic_instr);
// void generateASMCode(ICInstr *ic_instr);
void printCodeGen(ICInstr *ic_instr);
void printInstrCG(ICInstr *ic_instr);
