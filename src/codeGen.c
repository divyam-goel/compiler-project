#include "codeGen.h"

char reg_eax[] = "EAX";
char reg_ebx[] = "EBX";
char reg_ecx[] = "ECX";
char reg_edx[] = "EDX";

char reg_ax[] = "AX";
char reg_bx[] = "BX";
char reg_cx[] = "CX";
char reg_dx[] = "DX";

char reg_xmm0[] = "XMM0";
char reg_xmm1[] = "XMM1";


void cgICAddr(char *addr, ICAddr *ic_addr) {
  switch (ic_addr->type)
  {
  case NUM:
    sprintf(addr, "%d", ic_addr->value.num);
    break;
  case RNUM:
    sprintf(addr, "%f", ic_addr->value.rnum);
    break;
  case BOOLEAN_:
    if (ic_addr->value.boolean == true)
      sprintf(addr, "True");
    else
      sprintf(addr, "False");
    break;
  case IDENTIFIER:
    sprintf(addr, "[%s]", (char *)ic_addr->value.symbol);
    break;
  default:
    sprintf(addr, "NULL");
    break;
  }
}

void addLabel(char *instr_list, char *label){
  strcat(instr_list,label);
  strcat(instr_list, ":\n");
}

void loadConstReg(char *instr_list, char *reg1, int num)
{
  char asm_instr[50] = "";
  char num_string[50];
  itoa(num,num_string, 10);

  strcat(asm_instr, "MOV");
  strcat(asm_instr, "\t");
  
  strcat(asm_instr, reg1);
  strcat(asm_instr, " , ");
  strcat(asm_instr, num_string);
  strcat(asm_instr,"\n");

  strcat(instr_list, asm_instr);
}

void instrOneOperand(char *instr_list, char *op, char *reg1) {
  char asm_instr[50] = "";

  strcat(asm_instr, op);
  strcat(asm_instr, "\t");
  
  strcat(asm_instr, reg1);
  strcat(asm_instr, "\n");
  
  strcat(instr_list, asm_instr);
}


void instrTwoOperand(char *instr_list, char *op, char *reg1, char *reg2) {
  char asm_instr[50] = "";

  strcat(asm_instr, op);
  strcat(asm_instr, "\t");
  
  strcat(asm_instr, reg1);
  strcat(asm_instr, " , ");
  
  strcat(asm_instr, reg2);
  strcat(asm_instr, "\n");
  
  strcat(instr_list, asm_instr);
}


void cgLoadINT(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[30];
  
  strcpy(asm_instr, "MOV");
  cgICAddr(addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, reg, addr);
  strcat(instr_list, asm_instr);
}


void cgLoadReal(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[30];
  
  strcpy(asm_instr, "MOVAPS");
  cgICAddr(addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, reg, addr);
  strcat(instr_list, asm_instr);
}


void cgStoreINT(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[30];
  
  strcpy(asm_instr, "MOV");
  cgICAddr(addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, addr, reg);
  strcat(instr_list, asm_instr);
}


void cgStoreREAL(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[30];
  
  strcpy(asm_instr, "MOVAPS");
  cgICAddr(addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, addr, reg);
  strcat(instr_list, asm_instr);
}


void cgADD_SUB_INT(ICInstr *ic_instr) {
  char instr_list[100] = "";
  char *op[10] = "";

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_bx, &(ic_instr->addr2));
  
  switch (ic_instr->op) {
  case icADD_INT:
    strcpy(op, "ADD");
    instrTwoOperand(instr_list, op, reg_ax, reg_bx);
    break;
  case icSUB_INT:
    strcpy(op, "SUB");
    instrTwoOperand(instr_list, op, reg_ax, reg_bx);
    break;
  default:
    printf("Error: Invalid Integer Operation\n");
  }

  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  printf("DEBUG: %s\n", instr_list);
}


void cgADD_SUB_REAL(ICInstr *ic_instr) {
  char instr_list[100] = "";
  char *op[10] = "";

  cgLoadREAL(instr_list, reg_xmm0, &(ic_instr->addr1));
  cgLoadREAL(instr_list, reg_xmm1, &(ic_instr->addr2));
  
  switch (ic_instr->op) {
  case icADD_REAL:
    strcpy(op, "ADDPS");
    instrTwoOperand(instr_list, op, reg_xmm0, reg_xmm1);
    break;
  case icSUB_REAL:
    strcpy(op, "SUBPS");
    instrTwoOperand(instr_list, op, reg_xmm0, reg_xmm1);
    break;
  default:
    printf("Error: Invalid Integer Operation\n");
  }

  cgStoreREAL(instr_list, reg_ax, &(ic_instr->addr3));

  printf("DEBUG: %s\n", instr_list);
}


void cgMUL_INT(ICInstr *ic_instr){ //---> so far only int
  char instr_list[100] = ""; //final instruction -temporary max length as 100

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  strcat(instr_list,"IMUL\tAX , CX\n");
  // The result of this MUL goes into DX:AX- for now we just store AX
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  // print out result
  printf("%s", instr_list);


}


void cgDIV_INT(ICInstr *ic_instr){
  char instr_list[100] = ""; //final instruction -temporary max length as 100

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  strcat(instr_list, "IDIV\tCX\n");
  // The result of this IDIV goes into DX:AX- AX has the quotient
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  // print out result
  printf("%s", instr_list);
}


void cgMUL_REAL(ICInstr *ic_instr){
  char instr_list[100] = ""; //final instruction -temporary max length as 100

  cgLoadReal(instr_list, reg_xmm0, &(ic_instr->addr1));
  cgLoadReal(instr_list, reg_xmm1, &(ic_instr->addr2));

  instrTwoOperand(instr_list, "MULPS", reg_xmm0 , reg_xmm1);
  // result in reg_xmm0
  cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));
}


void cgDIV_REAL(ICInstr *ic_instr){
  char instr_list[100] = ""; //final instruction -temporary max length as 100

  cgLoadReal(instr_list, reg_xmm0, &(ic_instr->addr1));
  cgLoadReal(instr_list, reg_xmm1, &(ic_instr->addr2));

  instrTwoOperand(instr_list, "DIVPS", reg_xmm0, reg_xmm1);
  // result in reg_xmm0
  cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));
}


void cgINC(ICInstr *ic_instr) {
  char instr_list[100];
  cgLoadReg(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "INC\tAX\n");
  cgStoreMem(instr_list, reg_ax, &(ic_instr->addr1));
}


void cgDEC(ICInstr *ic_instr) {
  char instr_list[100];
  cgLoadReg(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "DEC\tAX\n");
  cgStoreMem(instr_list, reg_ax, &(ic_instr->addr1));
}


void cgSTORE_INT(ICInstr *ic_instr){ //---> not fully working- indexed elements of arrays need to be added
  char instr_list[100] = "";

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  char addr3[30];
  sprintf(addr3, "mem(%s)", (char *)(ic_instr->addr3).value.symbol);
  char addr1[30];
  sprintf(addr1, "mem(%s)", (char *)(ic_instr->addr1).value.symbol);
  strcat(instr_list, addr3);
  strcat(instr_list, " , ");
  strcat(instr_list, addr1);

  // print out result
  printf("%s", instr_list);
}


void cgRelationalOp(ICInstr *ic_instr) {
  // for now, using 1 as truth value, 0 as false value:
  char instr_list[100] = ""; //final instruction -temporary max length as 100
  char label1[] = "L1";
  char label2[] = "L2";
  char operator[] = "";

  switch (ic_instr->op){
  case icEQ:
    strcat(operator, "JE");
    break;
  case icNE:
    strcat(operator, "JNE");
    break;
  case icLT:
    strcat(operator,"JL");
    break;
  case icGT:
    strcat(operator, "JG");
    break;
  case icLE:
    strcat(operator, "JLE");
    break;
  case icGE:
    strcat(operator, "JGE");
    break;
  default:
    strcat(operator, "BLAH1");
    break;
  }
  
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_bx, &(ic_instr->addr2));

  instrTwoOperand(instr_list,"CMP",reg_ax,reg_bx);
  //instr eg -> "JLE L1"
  instrOneOperand(instr_list,operator,label1); 
  //this runs if above condition isnt true
  loadConstReg(instr_list, reg_cx, 0); //false
  instrOneOperand(instr_list, "JMP", label2); //jump to label2

  addLabel(instr_list,label1); //true => it comes here
  loadConstReg(instr_list, reg_cx, 1); //true

  addLabel(instr_list,label2); // if false, the other condition jumps here
  // store value in addr3
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
}


/* Those with '-' next to them have been implemented*/
// -void cgINC(ICInstr *ic_instr) {}
// -void cgDEC(ICInstr *ic_instr) {}
// void cgAND(ICInstr *ic_instr) {}
// void cgOR(ICInstr *ic_instr) {}
// -void cgEQ(ICInstr *ic_instr) {}
// -void cgNE(ICInstr *ic_instr) {}
// -void cgLT(ICInstr *ic_instr) {}
// -void cgGT(ICInstr *ic_instr) {}
// -void cgLE(ICInstr *ic_instr) {}
// -void cgGE(ICInstr *ic_instr) {}
// void cgPLUS(ICInstr *ic_instr) {}
// void cgMINUS(ICInstr *ic_instr) {}

// void cgJUMP(ICInstr *ic_instr) {}
// void cgTJUMP(ICInstr *ic_instr) {}
// void cgFJUMP(ICInstr *ic_instr) {}


void generateASMCode(ICInstr *ic_instr) {
  while (ic_instr != NULL) {
    switch (ic_instr->op) {
      case icADD_INT:
      case icSUB_INT:
        cgADD_SUB_INT(ic_instr);
        break;
      case icMUL_INT:
        cgMUL_INT(ic_instr);
        break;
      case icDIV_INT:
        cgDIV_INT(ic_instr);
        break;
      case icADD_REAL:
      case icSUB_REAL:
        cgADD_SUB_REAL(ic_instr);
        break;
      case icMUL_REAL:
        cgMUL_REAL(ic_instr);
        break;
      case icDIV_REAL:
        cgDIV_REAL(ic_instr);
        break;
      case icINC:
        break;
      case icDEC:
        break;
      case icAND:
        break;
      case icOR:
        break;
      case icEQ:
        break;
      case icNE:
        break;
      case icLT:
        break;
      case icGT:
        break;
      case icLE:
        break;
      case icGE:
        break;
      case icPLUS:
        break;
      case icMINUS:
        break;
      case icCOPY:
        break;
      case icSTORE:
        break;
      case icJUMP:
        break;
      case icJUMPNZ:
        break;
      case icJUMPZ:
        break;
      case icLABEL:
        break;
      default:
        break;
    }

  }
} //question: whats the result of a less than operation? just windering what to put okayokan the addrr3