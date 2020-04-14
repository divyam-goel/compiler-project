#include "codeGen.h"

char reg_eax[] = "EAX";
char reg_ebx[] = "EBX";
char reg_ecx[] = "ECX";
char reg_edx[] = "EDX";

char reg_ax[] = "AX";
char reg_bx[] = "BX";
char reg_cx[] = "CX";
char reg_dx[] = "DX";
char reg_r8w[] = "R8W";
char reg_r9w[] = "R9W";

char reg_xmm0[] = "XMM0";
char reg_xmm1[] = "XMM1";

char reg_al[] = "AL";
char reg_bl[] = "BL";
char reg_cl[] = "CL";

void cgICAddr(char *instr_list, char *addr, ICAddr *ic_addr) {
  char op[10];
  
  switch (ic_addr->type) {
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
  case ARRAY:
    strcpy(op, "MOV");
    cgICAddr(instr_list, addr, ic_addr->value.array.idx);
    instrTwoOperand(instr_list, op, reg_r8w, addr);
    
    strcpy(op, "MOV");
    strcpy(addr, ic_addr->value.array.var);
    instrTwoOperand(instr_list, op, reg_r9w, addr);
    
    sprintf(addr, "[%s + %s * 2]", reg_r9w, reg_r8w);
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


// void loadConstReg(char *instr_list, char *reg1, int num) {
//   char asm_instr[50] = "";
//   char num_string[50];
//   itoa(num,num_string, 10);

//   strcat(asm_instr, "MOV");
//   strcat(asm_instr, "\t");
  
//   strcat(asm_instr, reg1);
//   strcat(asm_instr, " , ");
//   strcat(asm_instr, num_string);
//   strcat(asm_instr,"\n");

//   strcat(instr_list, asm_instr);
// }


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
  char op[50];
  char addr[30];
  
  strcpy(op, "MOV");
  cgICAddr(instr_list, addr, ic_addr);  
  instrTwoOperand(instr_list, op, reg, addr);
}


void cgLoadREAL(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char op[50];
  char addr[30];

  strcpy(op, "MOVAPS");
  cgICAddr(instr_list, addr, ic_addr);  
  instrTwoOperand(instr_list, op, reg, addr);
}


void cgStoreINT(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[30];
  
  strcpy(asm_instr, "MOV");
  cgICAddr(instr_list, addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, addr, reg);
  // strcat(instr_list, asm_instr);
}


void cgStoreREAL(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[30];
  
  strcpy(asm_instr, "MOVAPS");
  cgICAddr(instr_list, addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, addr, reg);
  // strcat(instr_list, asm_instr);
}


void cgADD_SUB_INT(ICInstr *ic_instr) {
  char instr_list[100] = "";
  char op[10] = "";

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
  char op[10] = "";

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

  cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));

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

  cgLoadREAL(instr_list, reg_xmm0, &(ic_instr->addr1));
  cgLoadREAL(instr_list, reg_xmm1, &(ic_instr->addr2));

  instrTwoOperand(instr_list, "MULPS", reg_xmm0 , reg_xmm1);
  // result in reg_xmm0
  cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));
}


void cgDIV_REAL(ICInstr *ic_instr){
  char instr_list[100] = ""; //final instruction -temporary max length as 100

  cgLoadREAL(instr_list, reg_xmm0, &(ic_instr->addr1));
  cgLoadREAL(instr_list, reg_xmm1, &(ic_instr->addr2));

  instrTwoOperand(instr_list, "DIVPS", reg_xmm0, reg_xmm1);
  // result in reg_xmm0
  cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));
}


void cgINC(ICInstr *ic_instr) {
  char instr_list[100];
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "INC\tAX\n");
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
}


void cgDEC(ICInstr *ic_instr) {
  char instr_list[100];
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "DEC\tAX\n");
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
}


void cgSTOREVALUE_INT(ICInstr *ic_instr){ //---> not fully working- indexed elements of arrays need to be added
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


void cgRelationalOp(ICInstr *ic_instr) {  //----> for LT,GT,E,NE,LE,GE
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
  instrTwoOperand(instr_list, "MOV", reg_cl, "0x00"); //false
  instrOneOperand(instr_list, "JMP", label2); //jump to label2

  addLabel(instr_list,label1); //true => it comes here
  instrTwoOperand(instr_list, "MOV", reg_cl, "0x11"); //true

  addLabel(instr_list,label2); // if false, the other condition jumps here
  // store value in addr3
  cgStoreINT(instr_list, reg_cl, &(ic_instr->addr3));
}

void cgLogicalOp(ICInstr *ic_instr) {   //-----> for AND,OR
  char instr_list[100] = "";
  char op[10] = "";

  //assuming that booleans are a byte long
  cgLoadINT(instr_list, reg_al, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_bl, &(ic_instr->addr2));

  switch (ic_instr->op){
  case icAND:
    strcpy(op, "AND");
    instrTwoOperand(instr_list, op, reg_al, reg_bl);
    break;
  case icOR:
    strcpy(op, "OR");
    instrTwoOperand(instr_list, op, reg_al, reg_bl);
    break;
  default:
    printf("Error: Invalid Integer Operation\n");
  }
  //store result in reg_al to memory
  cgStoreINT(instr_list, reg_al, &(ic_instr->addr3));
}

void cgPLUS_MINUS(ICInstr *ic_instr) {
  char instr_list[100] = "";
  char op[10] = "";

  // we take the positive or negative of addr1 and pass it to addr3
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));

  switch (ic_instr->op){
  case icPLUS:
    break;
  case icMINUS:
    strcpy(op, "NEG");
    instrOneOperand(instr_list, op, reg_ax);
    break;
  default:
    printf("Error: Invalid Unary Op\n");
  }
  //store result in reg_al to memory
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
}

/* Those with '-' next to them have been implemented*/
// -void cgINC(ICInstr *ic_instr) {}
// -void cgDEC(ICInstr *ic_instr) {}
// -void cgAND(ICInstr *ic_instr) {}
// -void cgOR(ICInstr *ic_instr) {}
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
} 


//testing
void printInstrCG(ICInstr *ic_instr){
  switch (ic_instr->op){
    case icADD_INT:
    case icADD_REAL:
      printf("ADD\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icSUB_INT:
    case icSUB_REAL:
      printf("SUB\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icMUL_INT:
    case icMUL_REAL:
      printf("MUL\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);

      break;
    case icDIV_INT:
    case icDIV_REAL:
      printf("DIV\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      // cg_ADD_SUB_DIV_MUL(ic_instr);
      break;
    case icINC:
      printf("INC\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      break;
    case icDEC:
      printf("DEC\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      break;
    case icAND:
      printf("AND\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icOR:
      printf("OR\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icEQ:
      printf("EQ\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icNE:
      printf("NE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icLT:
      printf("LT\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icGT:
      printf("GT\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icLE:
      printf("LE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icGE:
      printf("GE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icPLUS:
      printf("PLUS\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      break;
    case icMINUS:
      printf("MINUS\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      break;
    // case icCOPY:
    //   printf("COPY\t");
    //   break;
    // case icLOAD:
    //   printf("LOAD\t");
    //   break;
    case icSTORE:
      printf("STORE\t");
      printICAddress(ic_instr->addr3);
      printICAddress(ic_instr->addr1);
      // cgSTORE(ic_instr);
      break;
    case icJUMP:
      printf("JUMP\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      break;
    case icJUMPNZ:
      printf("JUMPNZ\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    case icJUMPZ:
      printf("JUMPZ\t");
      printf("\t");
      printICAddress(ic_instr->addr1);
      printICAddress(ic_instr->addr2);
      break;
    // case icCALL:
    //   printf("CALL\t\t\t");
    //   break;
    case icLABEL:
      printf("LABEL\t");
      printICAddress(ic_instr->addr1);
      break;
    default:
      break;
  }
  printf("\n");
}

void printCodeGen(ICInstr *ic_instr){
  while (ic_instr != NULL)
  {
    printInstrCG(ic_instr);
    ic_instr = ic_instr->next;
  }
}