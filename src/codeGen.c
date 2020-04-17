#include "codeGen.h"

char *output_file = "output/code.asm";

char reg_eax[] = "EAX";
char reg_ebx[] = "EBX";
char reg_ecx[] = "ECX";
char reg_edx[] = "EDX";

char reg_ax[] = "ax";
char reg_bx[] = "bx";
char reg_cx[] = "cx";
char reg_dx[] = "dx";
char reg_r8w[] = "R8W";
char reg_r9w[] = "R9W";

char reg_xmm0[] = "xmm0";
char reg_xmm1[] = "xmm1";

char reg_al[] = "al";
char reg_bl[] = "bl";
char reg_cl[] = "cl";

char tmp_float_var1[] = "float_temp1";
char tmp_float_var2[] = "float_temp2";

/* Global number for labels used */
int rel_op_label_no = 0;


/* Utility functions */

void insertVarFloat(char *tmp_var, char *name, char *type, float value) {
  sprintf(tmp_var, "\t%s:\t%s\t%f\n", name, type, value);
}


void insertVarInt(char *tmp_var, char *name, char *type, int value) {
  sprintf(tmp_var, "\t%s:\t%s\t%d\n", name, type, value);
}


void insert_fmt(char *tmp_var, char *name, char *per_val) {
  sprintf(tmp_var, "\t%s:\tdb\t\"Output: %s\", 10, 0\n", name, per_val);
}


void initializeASMOutputFile(char *output_file){
  char data_list[400];
  char tmp_var[50];
  printf("\nWriting assembled code to: %s\n",output_file);

  /* initialize variables */
  strcpy(data_list, "");

  /* directives */
  strcat(data_list, "global main\n");
  strcat(data_list,"extern printf\n");
  strcat(data_list,"\n");

  /* introduce DATA section */
  strcat(data_list,"SECTION .data\n");
  
  /* populate data section */
  insertVarFloat(tmp_var,"float_temp1","dq",0.0);
  strcat(data_list, tmp_var);
  insertVarFloat(tmp_var,"float_temp2", "dq", 0.0);
  strcat(data_list, tmp_var);
  insert_fmt(tmp_var,"print_fmt_int","%d");
  strcat(data_list, tmp_var);
  insert_fmt(tmp_var,"print_fmt_float","%e");
  strcat(data_list, tmp_var);
  insert_fmt(tmp_var,"print_fmt_false","false");
  strcat(data_list, tmp_var);
  insert_fmt(tmp_var,"print_fmt_true","true");
  strcat(data_list, tmp_var);
  strcat(data_list,"\n");
  
  /* introduce TEXT section */
  strcat(data_list, "SECTION .text\n");
  strcat(data_list,"\n");
  
  /* label for MAIN */
  strcat(data_list,"main:\n");
  
  /* write to output file */
  FILE *fptr = fopen(output_file,"w");
  fputs(data_list,fptr);
  fclose(fptr);
}


void writeInstructionToOutput(char *data){
  FILE *fptr = fopen(output_file,"a");
  char instr[MAX_SIZE_INSTR];
  strcpy(instr,data);
  strcat(instr,"\n");
  fputs(instr,fptr);
  fclose(fptr);
}


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
    strcpy(op, "mov");
    cgICAddr(instr_list, addr, ic_addr->value.array.idx);
    instrTwoOperand(instr_list, op, reg_r8w, addr);
    
    strcpy(op, "mov");
    strcpy(addr, ic_addr->value.array.var);
    instrTwoOperand(instr_list, op, reg_r9w, addr);
    
    sprintf(addr, "[%s + %s * 2]", reg_r9w, reg_r8w);
    break;
  default:
    sprintf(addr, "NULL");
    break;
  }
}


void newRelationalOpLabel(char *label){
  sprintf(label, "RelOpL%d", rel_op_label_no);
  rel_op_label_no++;
}


void addLabel(char *instr_list, char *label) { 
  strcat(instr_list,label);
  strcat(instr_list, ":\n");
}


void loadConstReg(char *instr_list, char *reg1, int num) {
  char asm_instr[30] = "";
  char num_string[50];

  // itoa(num,num_string, 10);
  sprintf(num_string, "%d", num);

  strcat(asm_instr, "\tmov");
  strcat(asm_instr, "\t");
  
  strcat(asm_instr, reg1);
  strcat(asm_instr, " , ");
  strcat(asm_instr, num_string);
  strcat(asm_instr,"\n");

  strcat(instr_list, asm_instr);
}


void instrOneOperand(char *instr_list, char *op, char *reg1) {
  char asm_instr[70] = "";

  strcat(asm_instr,"\t");
  strcat(asm_instr, op);
  strcat(asm_instr, "\t");
  
  strcat(asm_instr, reg1);
  strcat(asm_instr, "\n");
  
  strcat(instr_list, asm_instr);
}


void instrTwoOperand(char *instr_list, char *op, char *reg1, char *reg2) {
  char asm_instr[50] = "";

  strcat(asm_instr, "\t");
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
  
  strcpy(op, "mov");
  cgICAddr(instr_list, addr, ic_addr);  
  instrTwoOperand(instr_list, op, reg, addr);
}


void cgRealOneOp(char *instr_list, char *op, char *type, char *addr) { // FLD dword[tmp1]
  char rhs[50];
  sprintf(rhs,"%s[%s]",type,addr);
  instrOneOperand(instr_list, op, rhs);
}


void cgLoadRealConstIntoTmp(char *instr_list, char *type, char *tmp_var, ICAddr *ic_addr){ // mov dword[tmp1] , __float32__(23.0)
  char op[] = "mov";
  char addr1[30];
  char addr2[30];

  sprintf(addr1,"%s[%s]",type,tmp_var);
  sprintf(addr2, "__float32__(%f)", ic_addr->value.rnum);
  instrTwoOperand(instr_list, op, addr1, addr2);
}


void cgStoreINT(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char asm_instr[50];
  char addr[50];
  
  strcpy(asm_instr, "mov");
  cgICAddr(instr_list, addr, ic_addr);
  instrTwoOperand(instr_list, asm_instr, addr, reg);
}

/* -- end -- */


/* Functions for populating asm code */

void cgArithmeticOpInteger(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char op[10] = "";

  switch (ic_instr->op) {
  case icADD_INT:
    strcpy(op, "add");
    break;
  case icSUB_INT:
    strcpy(op, "sub");
    break;
  default:
    printf("ERROR!! Invalid Integer Operation: %d\n", ic_instr->op);
  }

  /* instruction: load from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));

  /* instruction: load from mem addr2 to reg BX */
  cgLoadINT(instr_list, reg_bx, &(ic_instr->addr2));
  
  /*
    instruction: perform arithmetic op between values in regs
    AX and BX, and store the result in reg AX
  */
  instrTwoOperand(instr_list, op, reg_ax, reg_bx);

  /* instruction: store the Quotient from reg AX to mem addr3 */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
  
  writeInstructionToOutput(instr_list);
}


void cgMultiplyInteger(ICInstr *ic_instr){ //---> so far only int
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

  /* instruction: load from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  /* instruction: load from mem addr2 to reg BX */
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  /*
    instruction: multiply value in reg AX with value in reg BX
    the result is store in DX:AX, as follow:
      1. DX stores the upper 16 bytes
      2. AX stores the lower 16 bytes
  */
  instrTwoOperand(instr_list,"imul",reg_ax,reg_cx);
  
  /*
    instruction: store the last 16 bytes of the result
    from reg AX to mem addr3
  */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  writeInstructionToOutput(instr_list);
}


void cgDivisionInteger(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

  /* instruction: load from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  /* instruction: load from mem addr2 to reg BX */
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  /*
    instruction: divide value in reg AX by value in reg BX
    the result is store in DX:AX, as follow:
      1. DX stores the remainder
      2. AX stores the quotient
  */
  instrOneOperand(instr_list, "idiv", reg_cx);
  
  /* instruction: store the Quotient from reg AX to mem addr3 */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
 
  writeInstructionToOutput(instr_list);
}


void cgArithmeticOpReal(ICInstr *ic_instr) {
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
  cgRealOneOp(instr_list,"fld",type1,addr1);

  switch (ic_instr->op) {
  case icADD_REAL:
    strcpy(op, "fadd");
    break;
  case icSUB_REAL:
    strcpy(op, "fsub");
    break;
  case icMUL_REAL:
    strcpy(op, "fmul");
    break;
  case icDIV_REAL:
    strcpy(op, "fdiv");
    break;
  default:
    printf("Error: Invalid Float Operation\n");
  }

  // now, apply operation with the second number on the first number
  cgRealOneOp(instr_list,op,type2,addr2);
  // finally to store it in addr3 variable
  sprintf(addr3, "%s", (char *)(&(ic_instr->addr3))->value.symbol);
  cgRealOneOp(instr_list,"FSTP","qword",addr3); 

  writeInstructionToOutput(instr_list);
}


void cgIncrement(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";

  /* instruction: load from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));

  /* instruction: increment value in reg AX and store it in reg AX */
  strcat(instr_list, "inc\tax\n");

  /* instruction: store from reg AX to mem addr1 */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  writeInstructionToOutput(instr_list);
}


void cgDecrement(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  
  /* instruction: load from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  /* instruction: decrement value in reg AX and store it in reg AX */
  strcat(instr_list, "dec\tax\n");
  
  /* instruction: store from reg AX to mem addr1 */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  writeInstructionToOutput(instr_list);
}


void cgMoveFromMemToMem(ICInstr *ic_instr){ //---> not fully working- indexed elements of arrays need to be added
  char instr_list[MAX_SIZE_INSTR] = "";

  /* instruction: load from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  /* instruction: store from reg AX to mem addr3 */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  writeInstructionToOutput(instr_list);
}


void cgRelationalOp(ICInstr *ic_instr) {  //----> for LT,GT,E,NE,LE,GE
  char instr_list[MAX_SIZE_INSTR] = "";
  char label1[10];
  char label2[10];
  char op[10];

  newRelationalOpLabel(label1);
  newRelationalOpLabel(label2);
  switch (ic_instr->op){
    case icEQ:
      strcpy(op, "je");
      break;
    case icNE:
      strcpy(op, "jne");
      break;
    case icLT:
      strcpy(op,"jl");
      break;
    case icGT:
      strcpy(op, "jg");
      break;
    case icLE:
      strcpy(op, "jle");
      break;
    case icGE:
      strcpy(op, "jge");
      break;
    default:
      strcpy(op, "err");
      break;
  } 
  
  /* instruction: load first operand from mem addr1 to reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  /* instruction: load second operand from mem addr2 to reg BX */
  cgLoadINT(instr_list, reg_bx, &(ic_instr->addr2));

  /* instruction: compare values in regs AX and BX */
  instrTwoOperand(instr_list, "cmp", reg_ax, reg_bx);
  
  /* instruction: perform jump operation w.r.t relational op */
  instrOneOperand(instr_list, op, label1); 
  
  /*
    False Case:
    if the result of above relational comparison is false,
    then execute:
      1. instruction: store value false (0x00) in reg CL
      2. instruction: jump to label2 (end of set of relational instrutions)
  */
  instrTwoOperand(instr_list, "mov", reg_cl, "0x00"); //false
  instrOneOperand(instr_list, "jmp", label2); //jump to label2

  /*
    True Case:
    if the result of above relational comparison is true,
    then jump to label1, and execute:
      1. instruction: set label for value for label1
      2. instruction: store value true (0x11) in reg CL
  */
  addLabel(instr_list, label1);
  instrTwoOperand(instr_list, "mov", reg_cl, "0x11");

  /* instruction: set label for value of label2*/
  addLabel(instr_list,label2);
  
  /* instruction: store value from reg CL to mem addr3 */
  cgStoreINT(instr_list, reg_cl, &(ic_instr->addr3));
  
  writeInstructionToOutput(instr_list);
}


void cgLogicalOp(ICInstr *ic_instr) {   //-----> for AND,OR
  char instr_list[MAX_SIZE_INSTR] = "";
  char op[10] = "";

  switch (ic_instr->op){
    case icAND:
      strcpy(op, "and");
      break;
    case icOR:
      strcpy(op, "or");
      break;
    default:
      printf("ERROR!! Invalid Logical Operation: %d\n", ic_instr->op);
      strcpy(op, "err");
  }

  /* instruction: load first operand from mem addr1 to reg AL */
  cgLoadINT(instr_list, reg_al, &(ic_instr->addr1));
  
  /* instruction: load second operand from mem addr2 to reg BL */
  cgLoadINT(instr_list, reg_bl, &(ic_instr->addr2));

  /*
    instruction: perform logical operation AND or OR
    between regs AL and BL, and store the result in AL
  */
  instrTwoOperand(instr_list, op, reg_al, reg_bl);
  
  /* instruction: store the value from reg AL to mem addr3  */
  cgStoreINT(instr_list, reg_al, &(ic_instr->addr3));

  writeInstructionToOutput(instr_list);
}


void cgUnaryOp(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char op[10] = "";

  /*
    if op is icPLUS then do nothing as PLUS
    has NO effect on a value
  */
  if (ic_instr->op == icPLUS)
    return;

  /* instruction: load the value of mem addr1 in reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));

  /* instruction: negate the value in reg AX using unary minus */
  strcpy(op, "neg");
  instrOneOperand(instr_list, op, reg_ax);

  /* instruction: store the value of reg AX in mem addr3 */
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
 
  writeInstructionToOutput(instr_list);
}


void cgJumpUnconditional(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char label[50];
  
  /* intialize label with the jump label value */
  cgICAddr(instr_list, label, &(ic_instr->addr1));
  
  /* instruction: jump to the label (unconditionally) */
  instrOneOperand(instr_list,"jmp",label);
 
  writeInstructionToOutput(instr_list);
}


void cgJumpConditional(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char instr[10];
  char label[20];
  
  /* intialize label with the jump label value */
  cgICAddr(instr_list, label, &(ic_instr->addr2));

  /* instruction: load test value in reg AX */
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  /* instruction: compare the test value with 0 */
  instrTwoOperand(instr_list, "cmp", reg_ax, "0");
  
  /*
    instruction: jump to the label
      1. if test value IS 0 (if 'je')
      2. if test value is NOT 0 (if 'jne')
  */
  if(ic_instr->op == icJUMPZ)
    strcpy(instr, "je");
  else
    strcpy(instr, "jne");
  instrOneOperand(instr_list, instr, label);
 
  writeInstructionToOutput(instr_list);
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

  writeInstructionToOutput(instr_list);
}


void cgLabel(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char *label;

  /* intialize variables */
  label = (char *)(ic_instr->addr1.value.symbol);
  
  /* asm code for label */
  addLabel(instr_list, label);

  writeInstructionToOutput(instr_list);
}

/* -- end -- */


/* Functions for interfacing with ASM Code Generation */

void generateASMInstruction(ICInstr *ic_instr){
  switch (ic_instr->op){
    /* unary op */
    case icPLUS:
    case icMINUS:
      cgUnaryOp(ic_instr);
      break;
    
    /* logical op */
    case icAND:
    case icOR:
      cgLogicalOp(ic_instr);
      break;
    
    /* relational op */
    case icEQ:
    case icNE:
    case icLT:
    case icGT:
    case icLE:
    case icGE:
      cgRelationalOp(ic_instr);
      break;
    
    /* arithmetic op - integer */
    case icADD_INT:
    case icSUB_INT:
      cgArithmeticOpInteger(ic_instr);
      break;

    /* term op - integer */
    case icMUL_INT:
      cgMultiplyInteger(ic_instr);
      break;
    case icDIV_INT:
      cgDivisionInteger(ic_instr);
      break;

    /* arithmetic & term op - real */
    case icADD_REAL:
    case icSUB_REAL:
    case icMUL_REAL:
    case icDIV_REAL:
      cgArithmeticOpReal(ic_instr);
      break;

    /* increment & decrement */
    case icINC:
      cgIncrement(ic_instr);
      break;
    case icDEC:
      cgDecrement(ic_instr);
      break;

    case icMOV:
      cgMoveFromMemToMem(ic_instr);
      break;

    /* unconditional jump */
    case icJUMP:
      cgJumpUnconditional(ic_instr);
      break;

    /* conditional jump */
    case icJUMPNZ:
    case icJUMPZ:
      cgJumpConditional(ic_instr);
      break;

    // case icCALL:
    //   printf("CALL\t\t\t");
    //   break;

    case icLABEL:
      cgLabel(ic_instr);

      break;
    default:
      break;
  }
  // printf("\n");
}


void generateASMCode(ICInstr *ic_instr){
  /* write boiler-plate ASM code to output file */
  initializeASMOutputFile(output_file);
  
  /*
    iterate through intermediate code instructions to
    generate corresponding asm instructions
  */
  while (ic_instr != NULL) {
    generateASMInstruction(ic_instr);
    ic_instr = ic_instr->next;
  }
}

/* -- end -- */
