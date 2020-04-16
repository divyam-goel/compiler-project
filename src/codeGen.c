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

char tmp_float_var1[] = "float_temp1";
char tmp_float_var2[] = "float_temp2";
/* Global number for labels used */
int rel_op_label_no = 0;
/* Utility functions */

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

void assignRelOpLabel(char *label){
  sprintf(label,"RelOpL%d",rel_op_label_no);
  rel_op_label_no++;
}

void addLabel(char *instr_list, char *label)
{
  strcat(instr_list,label);
  strcat(instr_list, ":\n");
}

void loadConstReg(char *instr_list, char *reg1, int num) {
  char asm_instr[50] = "";
  char num_string[50];

  // itoa(num,num_string, 10);
  sprintf(num_string, "%d", num);

  strcat(asm_instr, "MOV");
  strcat(asm_instr, "\t");
  
  strcat(asm_instr, reg1);
  strcat(asm_instr, " , ");
  strcat(asm_instr, num_string);
  strcat(asm_instr,"\n");

  strcat(instr_list, asm_instr);
}


void instrOneOperand(char *instr_list, char *op, char *reg1) {
  char asm_instr[70] = "";

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
  char addr[50];
  
  strcpy(op, "MOV");
  cgICAddr(instr_list, addr, ic_addr);  
  instrTwoOperand(instr_list, op, reg, addr);
}


void cgRealOneOp(char *instr_list, char *op, char *type, char *addr) { // FLD dword[tmp1]
  char rhs[50];
  sprintf(rhs,"%s[%s]",type,addr);
  instrOneOperand(instr_list, op, rhs);
}

void cgLoadRealConstIntoTmp(char *instr_list, char *type, char *tmp_var, ICAddr *ic_addr){ // mov dword[tmp1] , __float32__(23.0)
  char op[] = "MOV";
  char addr1[30];
  char addr2[30];

  sprintf(addr1,"%s[%s]",type,tmp_var);
  sprintf(addr2, "__float32__(%f)", ic_addr->value.rnum);
  instrTwoOperand(instr_list, op, addr1, addr2);
}

void cgStoreINT(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[50];
  
  strcpy(asm_instr, "MOV");
  cgICAddr(instr_list, addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, addr, reg);
  // strcat(instr_list, asm_instr);
}


/* Actual functions for populating asm code */

void cgADD_SUB_INT(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
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

  printf("%s", instr_list);
}


void cgArithmetic_REAL(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char addr1[20];
  char type1[20];
  char addr2[20];
  char type2[20];
  char addr3[20];
  char op[10] = "";

  // num1
  if((&(ic_instr->addr1))->type == RNUM){
    cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var1, &(ic_instr->addr1));
    sprintf(addr1, "%s", tmp_float_var1);
    strcpy(type1,"dword");
  }
  else{
    sprintf(addr1, "%s", (char*)(&(ic_instr->addr1))->value.symbol);
    strcpy(type1, "qword");
  }
  // num 2
  if((&(ic_instr->addr2))->type == RNUM){
    cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var2, &(ic_instr->addr2));
    sprintf(addr2, "%s", tmp_float_var2);
    strcpy(type2,"dword");
  }
  else{
    sprintf(addr2, "%s", (char*)(&(ic_instr->addr2))->value.symbol);
    strcpy(type2, "qword");
  }
  
  //now to load first number into float stack -eg. FLD dword[tmp1]
  cgRealOneOp(instr_list,"FLD",type1,addr1);

  switch (ic_instr->op) {
  case icADD_REAL:
    strcpy(op, "FADD");
    break;
  case icSUB_REAL:
    strcpy(op, "FSUB");
    break;
  case icMUL_REAL:
    strcpy(op, "FMUL");
    break;
  case icDIV_REAL:
    strcpy(op, "FDIV");
    break;
  default:
    printf("Error: Invalid Integer Operation\n");
  }

  // now, apply operation with the second number on the first number
  cgRealOneOp(instr_list,op,type2,addr2);
  // finally to store it in addr3 variable
  sprintf(addr3, "%s", (char *)(&(ic_instr->addr3))->value.symbol);
  cgRealOneOp(instr_list,"FSTP","qword",addr3);

  printf("%s", instr_list);
}


void cgMUL_INT(ICInstr *ic_instr){ //---> so far only int
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  strcat(instr_list,"IMUL\tAX , CX\n");
  // The result of this MUL goes into DX:AX- for now we just store AX
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  // print out result
  printf("%s", instr_list);


}


void cgDIV_INT(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  strcat(instr_list, "IDIV\tCX\n");
  // The result of this IDIV goes into DX:AX- AX has the quotient
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  // print out result
  printf("%s", instr_list);
}


// void cgMUL_REAL(ICInstr *ic_instr){
//   char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

//   cgLoadREAL(instr_list, reg_xmm0, &(ic_instr->addr1));
//   cgLoadREAL(instr_list, reg_xmm1, &(ic_instr->addr2));

//   instrTwoOperand(instr_list, "MULPS", reg_xmm0 , reg_xmm1);
//   // result in reg_xmm0
//   cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));
//   printf("%s", instr_list);
// }


// void cgDIV_REAL(ICInstr *ic_instr){
//   char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

//   cgLoadREAL(instr_list, reg_xmm0, &(ic_instr->addr1));
//   cgLoadREAL(instr_list, reg_xmm1, &(ic_instr->addr2));

//   instrTwoOperand(instr_list, "DIVPS", reg_xmm0, reg_xmm1);
//   // result in reg_xmm0
//   cgStoreREAL(instr_list, reg_xmm0, &(ic_instr->addr3));
//   printf("%s", instr_list);
// }


void cgINC(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "INC\tAX\n");
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
  printf("%s", instr_list);
}


void cgDEC(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "DEC\tAX\n");
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
  printf("%s", instr_list);
}


void cgSTOREVALUE_INT(ICInstr *ic_instr){ //---> not fully working- indexed elements of arrays need to be added
  char instr_list[MAX_SIZE_INSTR] = "";

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  // print out result
  printf("%s", instr_list);
}


void cgRelationalOp(ICInstr *ic_instr) {  //----> for LT,GT,E,NE,LE,GE
  // for now, using 1 as truth value, 0 as false value:
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100
  char label1[10];
  char label2[10];
  char operator[10];

  assignRelOpLabel(label1);
  assignRelOpLabel(label2);
  switch (ic_instr->op){
    case icEQ:
      strcpy(operator, "JE");
      break;
    case icNE:
      strcpy(operator, "JNE");
      break;
    case icLT:
      strcpy(operator,"JL");
      break;
    case icGT:
      strcpy(operator, "JG");
      break;
    case icLE:
      strcpy(operator, "JLE");
      break;
    case icGE:
      strcpy(operator, "JGE");
      break;
    default:
      strcpy(operator, "BLAH1");
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

  printf("%s", instr_list);
}

void cgLogicalOp(ICInstr *ic_instr) {   //-----> for AND,OR
  char instr_list[MAX_SIZE_INSTR] = "";
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

  printf("%s", instr_list);
}

void cgUNARY(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
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

  printf("%s", instr_list);
}

void cgJUMP(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char label[50];
  cgICAddr(instr_list, label, &(ic_instr->addr1));
  instrOneOperand(instr_list,"JMP",label);

  printf("%s", instr_list);
}

void cgJUMP_NZ_Z(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char instr[10];
  char label[20];
  cgICAddr(instr_list, label, &(ic_instr->addr2)); //label

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  instrTwoOperand(instr_list,"CMP",reg_ax,"0");
  if(ic_instr->op == icJUMPZ){
    strcpy(instr,"JE");
  }
  else{
    strcpy(instr, "JNE");
  }
  instrOneOperand(instr_list,instr,label);

  printf("%s", instr_list);
}

void print_var(ICInstr *ic_instr){
  //assuming the convention of intcode is PRINT *VAR* -> VAR can be NUM,RNUM,BOOLEAN, or IDENTIFIER
  // also, assumption that the concerned data needed for formats etc. are in data section
  // needed- print_fmt_int, print_fmt_float, print_fmt_bool_false, print_fmt_bool_true
  char instr_list[MAX_SIZE_INSTR] = "";
  ICAddr *ic_addr = &(ic_instr->addr1);
  int num;
  char rhs[20];
  char fmt_smt[25];
  instrOneOperand(instr_list,"push","rbp");
  switch (ic_instr->op){
    case NUM:
      num = ic_addr->value.num;
      char ch_num[10];
      sprintf(ch_num,"%d",num);
      instrTwoOperand(instr_list,"mov","rdi","print_fmt_int");
      instrTwoOperand(instr_list, "mov", "rsi", ch_num);
      instrTwoOperand(instr_list, "mov", "rax", "0");
      instrOneOperand(instr_list,"call","printf");
      break;
    case RNUM:
      instrTwoOperand(instr_list, "mov", "rdi", "print_fmt_float");
      cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var1, ic_addr);
      cgRealOneOp(instr_list,"fld","dword",tmp_float_var1);
      cgRealOneOp(instr_list, "fstp", "qword", tmp_float_var1);
      sprintf(rhs,"%s[%s]","qword",tmp_float_var1);
      instrTwoOperand(instr_list,"movq","xmm0",rhs);
      instrTwoOperand(instr_list, "mov", "rax", "1");
      instrOneOperand(instr_list, "call", "printf");
      break;
    case BOOLEAN_:
      if (ic_addr->value.boolean == true)
        strcpy(fmt_smt, "print_fmt_bool_true");
      else
        strcpy(fmt_smt, "print_fmt_bool_false");
      instrTwoOperand(instr_list, "mov", "rdi", fmt_smt);
      instrTwoOperand(instr_list, "mov", "rax", "0");
      instrOneOperand(instr_list, "call", "printf");
      break;
    case IDENTIFIER:
      /* code */
      break;
    default:
      break;
  }
  instrOneOperand(instr_list, "pop", "rbp");
}

//for testing

void printInstrCG(ICInstr *ic_instr){
  switch (ic_instr->op){
    case icADD_INT:
    case icSUB_INT:
      cgADD_SUB_INT(ic_instr);
      break;
    case icADD_REAL:
    case icSUB_REAL:
    case icMUL_REAL:
    case icDIV_REAL:
      cgArithmetic_REAL(ic_instr);
      break;
    case icMUL_INT:
      cgMUL_INT(ic_instr);
      break;
    case icDIV_INT:
      cgDIV_INT(ic_instr);
      break;
    case icINC:
      cgINC(ic_instr);
      break;
    case icDEC:
      cgDEC(ic_instr);
      break;
    case icAND:
    case icOR:
      cgLogicalOp(ic_instr);
      break;
    case icEQ:
    case icNE:
    case icLT:
    case icGT:
    case icLE:
    case icGE:
      cgRelationalOp(ic_instr);
      break;
    case icPLUS:
    case icMINUS:
      cgUNARY(ic_instr);
    // case icCOPY:
    //   printf("COPY\t");
    //   break;
    // case icLOAD:
    //   printf("LOAD\t");
    //   break;
    case icSTORE:
      cgSTOREVALUE_INT(ic_instr);
      break;
    case icJUMP:
      cgJUMP(ic_instr);
      break;
    case icJUMPNZ:
    case icJUMPZ:
      cgJUMP_NZ_Z(ic_instr);
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


