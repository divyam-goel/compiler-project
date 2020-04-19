#include "intCode.h"

#define MAX_SIZE_INSTR 2000
void cgICAddr(char *instr_list, char *addr, ICAddr *ic_addr);
void addLabel(char *instr_list, char *label);
void loadConstReg(char *instr_list, char *reg1, int num);
void instrOneOperand(char *instr_list, char *op, char *reg1); 
void instrTwoOperand(char *instr_list, char *op, char *reg1, char *reg2);
void cgLoadINT(char *instr_list, char *reg, ICAddr *ic_addr);
void cgLoadReal(char *instr_list, ICAddr *ic_addr);
void cgRealOneOp(char *instr_list, char *op, char *type, char *addr);
void cgLoadRealConstIntoTmp(char *instr_list, char *type, char *tmp_var, ICAddr *ic_addr);
void cgStoreINT(char *instr_list, char *reg, ICAddr *ic_addr);
void cgStoreREAL(char *instr_list, char *type, char *addr);
void cgArithmeticOpInteger(ICInstr *ic_instr);
void cgMultiplyInteger(ICInstr *ic_instr);
void cgDivisionInteger(ICInstr *ic_instr);
void cgArithmeticOpReal(ICInstr *ic_instr);
void cgIncrement(ICInstr *ic_instr);
void cgDecrement(ICInstr *ic_instr);
void cgMoveFromMemToMem(ICInstr *ic_instr);
void cgRelationalOp(ICInstr *ic_instr);
void cgLogicalOp(ICInstr *ic_instr);
void cgUnaryOp(ICInstr *ic_instr);
void generateASMInstruction(ICInstr *ic_instr);
void generateASMCode(ICInstr *ic_instr,char *output_asm);
