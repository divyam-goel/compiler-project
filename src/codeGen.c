#include "codeGen.h"

char *output_file = "output/code.asm";

char reg_rax[] = "rax";
char reg_rbx[] = "rbx";
char reg_rcx[] = "rcx";
char reg_rdx[] = "rdx";
char reg_rsp[] = "rsp";
char reg_rbp[] = "rbp";
char reg_rsi[] = "rsi";
char reg_rdi[] = "rdi";

char instr_mov[] = "mov";
char instr_add[] = "add";
char instr_sub[] = "sub";
char instr_mul[] = "imul";
char instr_div[] = "idiv";
char instr_jmp[] = "jmp";
char instr_je[] = "je";
char instr_jne[] = "jne";
char instr_cmp[] = "cmp";
char instr_push[] = "push";
char instr_pop[] = "pop";

char const_0[] = "0";
char const_true[] = "true";
char const_false[] = "false";

char reg_ax[] = "rax";
char reg_bx[] = "rbx";
char reg_cx[] = "rcx";
char reg_dx[] = "rdx";
char reg_r8w[] = "r8";
char reg_r9w[] = "r9";

char reg_xmm0[] = "xmm0";
char reg_xmm1[] = "xmm1";

char reg_al[] = "al";
char reg_bl[] = "bl";
char reg_cl[] = "cl";

char tmp_float_var1[] = "float_temp1";
char tmp_float_var2[] = "float_temp2";

/* Global number for labels used */
int rel_op_label_no = 0;
int bool_label_no = 0;


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
  struct ModuleEntry *entry;
  
  /* initialize variables */
  strcpy(data_list, "");

  /* directives */
  strcat(data_list, "global main\n");
  strcat(data_list,"extern printf\n");
  strcat(data_list,"extern scanf\n");
  strcat(data_list,"\n");

  /* introduce TEXT section */
  strcat(data_list, "SECTION .text\n");
  strcat(data_list,"\n");
  
  /* label for MAIN */
  strcat(data_list,"main:\n");
  strcat(data_list,"\tpush rbx\n");
  strcat(data_list,"\tsub rsp, 32\n");
  
  strcpy(tmp_var, "driver");
  entry = resolveModule(tmp_var);
  sprintf(tmp_var, "\tsub rsp, %d\n", entry->activation_record_size);
  strcat(data_list, tmp_var);

  strcat(data_list, "\n");
  
  /* write to output file */
  FILE *fptr = fopen(output_file, "w");
  if (fptr == NULL) {
    fprintf(stderr, "Line number %d: Failed to open output file.\n", __LINE__ - 2);
    exit(-1);
  }
  fputs(data_list, fptr);
  fclose(fptr);
}


void writeDataSectionToOutputFile(char *output_file) {
  char data_list[1000];

  /* initialize variables */
  strcpy(data_list, "");

  /* introduce DATA section */
  strcat(data_list,"SECTION .data\n");

  /* constants true and false (booleans) */
  strcat(data_list, "true:\n");
  strcat(data_list, "\tdb \"true\", 0\n");
  strcat(data_list, "false:\n");
  strcat(data_list, "\tdb \"false\", 0\n");

  /* message & format for printing integer */
  strcat(data_list, "output_int_msg:\n");
  strcat(data_list, "\tdb \"Output: %ld\", 10, 0\n");

  // /* message & format for printing real */
  // strcat(data_list, "output_real_msg:\n");
  // strcat(data_list, "\tdb \"Output: %2f\", 10, 0\n");

  /* message & format for printing boolean */
  strcat(data_list, "output_bool_msg:\n");
  strcat(data_list, "\tdb \"Output: %s\", 10, 0\n");

  /* message for taking input integer */
  strcat(data_list, "input_int_msg:\n");
  strcat(data_list, "\tdb \"Input: Enter an integer value:\", 10, 0\n");
  /* format for taking input numbers */
  strcat(data_list, "input_int_format:\n");
  strcat(data_list, "\tdb \"%ld\", 0\n");

  // /* message for taking input real */
  // strcat(data_list, "input_real_msg:\n");
  // strcat(data_list, "\tdb \"Input: Enter a real value:\", 10, 0\n");
  // /* format for taking input real */
  // strcat(data_list, "input_real_format:\n");
  // strcat(data_list, "\tdb \"%2f\", 0\n");

  /* message for taking input boolean */
  strcat(data_list, "input_bool_msg:\n");
  strcat(data_list, "\tdb \"Input: Enter a boolean value:\", 10, 0\n");
  /* format for taking input booleans */
  strcat(data_list, "input_bool_format:\n");
  strcat(data_list, "\tdb \"%ld\", 0\n");

  /* message & format for printing array */
  strcat(data_list, "output_array_msg_1:\n");
  strcat(data_list, "\tdb \"Output: \", 0\n");
  strcat(data_list, "output_array_msg_2_int:\n");
  strcat(data_list, "\tdb \"%ld \", 0\n");
  // strcat(data_list, "output_array_msg_2_real:\n");
  // strcat(data_list, "\tdb \"%2f \", 0\n");
  strcat(data_list, "output_array_msg_2_bool:\n");
  strcat(data_list, "\tdb \"%s \", 0\n");

  /* message for taking input array */
  strcat(data_list, "input_array_msg_1:\n");
  strcat(data_list, "\tdb \"Input: Enter %d array elements of \", 0\n");

  strcat(data_list, "input_array_msg_2_int:\n");
  strcat(data_list, "\tdb \"integer type for range \", 0\n");

  // strcat(data_list, "input_array_msg_2_real:\n");
  // strcat(data_list, "\tdb \"real type for range \", 0\n");

  strcat(data_list, "input_array_msg_2_bool:\n");
  strcat(data_list, "\tdb \"boolean type for range \", 0\n");

  strcat(data_list, "input_array_msg_3:\n");
  strcat(data_list, "\tdb \"%d to \", 0\n");

  strcat(data_list, "input_array_msg_4:\n");
  strcat(data_list, "\tdb \"%d\", 10, 0\n");

  strcat(data_list, "newline:\n");
  strcat(data_list, "\tdb 10, 0\n");

  /* write to output file */
  FILE *fptr = fopen(output_file,"a");
  fputs(data_list, fptr);
  fclose(fptr);
}


void writeExitToOutputFile(char *output_file) {
  char data_list[400];
  char tmp_var[50];
  struct ModuleEntry *entry;

  /* intiliaze variables */
  strcpy(data_list, "");

  strcpy(tmp_var, "driver");
  entry = resolveModule(tmp_var);
  sprintf(tmp_var, "\tadd rsp, %d\n", entry->activation_record_size);
  strcat(data_list, tmp_var);

  strcat(data_list,"\tadd rsp, 32\n");
  strcat(data_list,"\tpop rbx\n");
  strcat(data_list,"\tret\n");
  
   /* write to output file */
  FILE *fptr = fopen(output_file, "a");
  fputs(data_list, fptr);
  fclose(fptr);

  writeDataSectionToOutputFile(output_file);
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
  struct VariableEntry *symbol_entry;
  char tmp[50];
  int data_size;
  
  switch (ic_addr->type) {
  case INTEGER:
    sprintf(addr, "%d", ic_addr->value.num);
    break;
  case REAL:
    sprintf(addr, "%f", ic_addr->value.rnum);
    break;
  case BOOLEAN_:
    if (ic_addr->value.boolean == true)
      sprintf(addr, "true");
    else
      sprintf(addr, "false");
    break;
  case IDENTIFIER:
    if (ic_addr->is_label) {
      strcpy(addr, (char *) ic_addr->value.symbol);
    }
    else {
      symbol_entry = (struct VariableEntry *)ic_addr->value.symbol;
      sprintf(addr, "[rsp + %d]", symbol_entry->mem_offset);
    }
    break;
  case ARRAY:
    symbol_entry = (struct VariableEntry *) ic_addr->value.array.var;
    
    instrTwoOperand(instr_list, instr_mov, reg_rdx, reg_rsp);
    
    sprintf(tmp, "%d", symbol_entry->mem_offset);
    instrTwoOperand(instr_list, instr_add, reg_rdx, tmp);

    data_size = getMemorySizeofDatatype(symbol_entry->datatype, false);
    sprintf(tmp, "%d", data_size);
    instrTwoOperand(instr_list, instr_add, reg_rdx, tmp);

    cgICAddr(instr_list, addr, ic_addr->value.array.idx);
    instrTwoOperand(instr_list, instr_mov, reg_rbx, addr);

    sprintf(tmp, "%d", symbol_entry->lower_bound->value.num);
    instrTwoOperand(instr_list, instr_sub, reg_rbx, tmp);
    
    sprintf(addr, "[%s + %s * %d]", reg_rdx, reg_rbx, data_size);
    break;
  default:
    sprintf(addr, "NULL");
    break;
  }
}


void newRelationalOpLabel(char *label){
  sprintf(label, "RelOpL%d", rel_op_label_no);
  rel_op_label_no += 1;
}


void newBoolLabel(char *label){
  sprintf(label, "BoolL%d", bool_label_no);
  bool_label_no += 1;
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
  char label1[20];
  char label2[20];
  char op[5];

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

void print_output_type(char *instr_list, enum terminal type,ICAddr *ic_addr, int is_identifier){
  int num;
  char rhs[20];
  char fmt_smt[25];
  char ch_num[20];
  char float_mem_var[20];
  switch (type){
    case NUM:
      num = ic_addr->value.num;
      if(is_identifier == 0){
        sprintf(ch_num,"%d",num);
      }
      else{
        sprintf(ch_num, "[%s]", "Identifier name"); // ------------->to be done
      }      
      instrTwoOperand(instr_list,"mov","rdi","print_fmt_int");
      instrTwoOperand(instr_list, "mov", "rsi", ch_num);
      instrTwoOperand(instr_list, "mov", "rax", "0");
      instrOneOperand(instr_list,"call","printf");
      break;
    case RNUM:
      instrTwoOperand(instr_list, "mov", "rdi", "print_fmt_float");
      if(is_identifier == 0){
        cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var1, ic_addr);
        cgRealOneOp(instr_list,"fld","dword",tmp_float_var1);
        cgRealOneOp(instr_list, "fstp", "qword", tmp_float_var1);
        strcpy(float_mem_var,tmp_float_var1);
      }
      else{
        strcpy(float_mem_var, "Identifier name");
      }  
      sprintf(rhs,"qword[%s]",float_mem_var);
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
    default:
      break;
  }
}

void print_var(ICInstr *ic_instr){
  //assuming the convention of intcode is PRINT *VAR* -> VAR can be NUM,RNUM,BOOLEAN, or IDENTIFIER
  // also, assumption that the concerned data needed for formats etc. are in data section
  // needed- print_fmt_int, print_fmt_float, print_fmt_bool_false, print_fmt_bool_true
  char instr_list[MAX_SIZE_INSTR] = "";
  ICAddr *ic_addr = &(ic_instr->addr1);
  instrOneOperand(instr_list,"push","rbp");
  if(ic_addr->type != IDENTIFIER){
    print_output_type(instr_list,ic_addr->type,ic_addr,0);
  }
  else{
    print_output_type(instr_list, ic_addr->type, ic_addr, 1); //to be implemented
  }
  instrOneOperand(instr_list, "pop", "rbp");

  writeInstructionToOutput(instr_list);
}


void printUtil(char *instr_list, char *format, char *arg) {
  /* 
    load the address of the format (i.e. the predefined message
    with a format specifier for the second argument of printf)
  */
  instrTwoOperand(instr_list, instr_mov, reg_rdi, format);
  if (arg != NULL) {
    instrTwoOperand(instr_list, instr_mov, reg_rsi, arg);
  }
  /*
    call printf function
    Note: stack should be aligned before calling the function
  */
  strcat(instr_list, "\txor rax, rax\n");  
  strcat(instr_list, "\tcall printf\n");
  strcat(instr_list, "\n");
}


void printBoolUtil(char *instr_list, char *addr) {
  char label1[10], label2[10];
  
  newBoolLabel(label1);
  newBoolLabel(label2);

  instrTwoOperand(instr_list, instr_mov, reg_rax, addr);

  instrTwoOperand(instr_list, instr_cmp, reg_rax, const_0);
  instrOneOperand(instr_list, instr_je, label1);

  instrTwoOperand(instr_list, instr_mov, reg_rax, const_true);
  instrOneOperand(instr_list, instr_jmp, label2);

  addLabel(instr_list, label1);
  instrTwoOperand(instr_list, instr_mov, reg_rax, const_false);

  addLabel(instr_list, label2);
  strcat(instr_list, "\n");

  strcpy(addr, "rax");
}


void mem(char *mem, char *addr) {
  strcpy(mem, "");
  sprintf(mem, "[%s]", addr);
}


void printArrayUtil(struct VariableEntry *symbol_entry) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char tmp1[50], tmp2[50], format[50];
  int base_offset, lower_bound, upper_bound;
  int data_size;
  
  base_offset = symbol_entry->mem_offset;
  lower_bound = symbol_entry->lower_bound->value.num;
  upper_bound = symbol_entry->upper_bound->value.num;
  data_size = getMemorySizeofDatatype(symbol_entry->datatype, false);

  strcpy(format, "output_array_msg_1");
  printUtil(instr_list, format, NULL);
  
  switch(symbol_entry->datatype) {
    case INTEGER:
      strcpy(format, "output_array_msg_2_int");
      break;
    case REAL:
      strcpy(format, "output_array_msg_2_real");
      break;
    case BOOLEAN_:
      strcpy(format, "output_array_msg_2_bool");
      break;
    default:
      break;
  }

  instrTwoOperand(instr_list, instr_mov, reg_rdx, reg_rsp);

  sprintf(tmp1, "%d", base_offset);
  instrTwoOperand(instr_list, instr_add, reg_rdx, tmp1);
  
  sprintf(tmp1, "%d", data_size);
  instrTwoOperand(instr_list, instr_mov, reg_rbx, tmp1);

  instrTwoOperand(instr_list, instr_add, reg_rdx, reg_rbx);

  mem(tmp1, reg_rdx);
  mem(tmp2, reg_rbx);

  writeInstructionToOutput(instr_list);
  strcpy(instr_list, "");

  for (int i = lower_bound; i <= upper_bound; i++) {
    instrOneOperand(instr_list, instr_push, reg_rdx);
    instrOneOperand(instr_list, instr_push, reg_rbx);

    printUtil(instr_list, format, tmp1);

    instrOneOperand(instr_list, instr_pop, reg_rbx);
    instrOneOperand(instr_list, instr_pop, reg_rdx);

    instrTwoOperand(instr_list, instr_add, reg_rdx, reg_rbx);

    if (i % 3 == 0) {
      writeInstructionToOutput(instr_list);
      strcpy(instr_list, "");
    }
  }

  strcpy(format, "newline");
  printUtil(instr_list, format, NULL);

  writeInstructionToOutput(instr_list);
}


void cgPrint(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char addr[10], format[50];
  struct VariableEntry *symbol_entry;
  enum terminal data_type;

  if (ic_instr->addr1.type == IDENTIFIER) {
    symbol_entry = (struct VariableEntry *)ic_instr->addr1.value.symbol;
    if (symbol_entry->isArray) {
      printArrayUtil(symbol_entry);
      return;
    }
    data_type = symbol_entry->datatype;
  }
  else if (ic_instr->addr1.type == ARRAY) {
    symbol_entry = (struct VariableEntry *)ic_instr->addr1.value.array.var;
    if (!symbol_entry->isStatic)
      return;
    data_type = symbol_entry->datatype;
  }
  else
    data_type = ic_instr->addr1.type;
    
  switch (data_type) {
    case INTEGER:
      strcpy(format, "output_int_msg");
      break;
    case REAL:
      strcpy(format, "output_real_msg");
      break;
    case BOOLEAN_:
      strcpy(format, "output_bool_msg");
      break;
    default:
      break;
  }

	/*
    load the address of the second argument for printf
    (i.e. what is to be printed)
  */
  cgICAddr(instr_list, addr, &(ic_instr->addr1));
  if (ic_instr->addr1.type == IDENTIFIER && data_type == BOOLEAN_) {
    printBoolUtil(instr_list, addr);
  }

  printUtil(instr_list, format, addr);

  writeInstructionToOutput(instr_list);
}


void inputUtil(char *instr_list, char *format, char *arg) {
  /* 
    load the address of the format (i.e. the string containing
    with a format specifier for the input of scanf)
  */
  instrTwoOperand(instr_list, instr_mov, reg_rdi, format);
  if (arg != NULL) {
    instrTwoOperand(instr_list, instr_mov, reg_rsi, arg);
  }
  strcat(instr_list, "\txor rax, rax\n");
  strcat(instr_list, "\tcall scanf\n");
  strcat(instr_list, "\n");
}


void inputArrayUtil(struct VariableEntry *symbol_entry) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char tmp[50], format[50];
  int base_offset, lower_bound, upper_bound;
  int num_elements, data_size;
  
  base_offset = symbol_entry->mem_offset;
  lower_bound = symbol_entry->lower_bound->value.num;
  upper_bound = symbol_entry->upper_bound->value.num;
  data_size = getMemorySizeofDatatype(symbol_entry->datatype, false);

  strcpy(format, "input_array_msg_1");
  num_elements = upper_bound - lower_bound + 1;
  sprintf(tmp, "%d", num_elements);
  printUtil(instr_list, format, tmp);
  
  switch(symbol_entry->datatype) {
    case INTEGER:
      strcpy(format, "input_array_msg_2_int");
      break;
    case REAL:
      strcpy(format, "input_array_msg_2_real");
      break;
    case BOOLEAN_:
      strcpy(format, "input_array_msg_2_bool");
      break;
    default:
      break;
  }
  printUtil(instr_list, format, NULL);

  strcpy(format, "input_array_msg_3");
  sprintf(tmp, "%d", lower_bound);
  printUtil(instr_list, format, tmp);

  strcpy(format, "input_array_msg_4");
  sprintf(tmp, "%d", upper_bound);
  printUtil(instr_list, format, tmp);

  instrTwoOperand(instr_list, instr_mov, reg_rdx, reg_rsp);

  sprintf(tmp, "%d", base_offset);
  instrTwoOperand(instr_list, instr_add, reg_rdx, tmp);
  
  sprintf(tmp, "%d", data_size);
  instrTwoOperand(instr_list, instr_mov, reg_rbx, tmp);

  instrTwoOperand(instr_list, instr_add, reg_rdx, reg_rbx);

  strcpy(format, "input_int_format");

  writeInstructionToOutput(instr_list);
  strcpy(instr_list, "");

  for (int i = lower_bound; i <= upper_bound; i++) {
    instrOneOperand(instr_list, instr_push, reg_rdx);
    instrOneOperand(instr_list, instr_push, reg_rbx);

    inputUtil(instr_list, format, reg_rdx);

    instrOneOperand(instr_list, instr_pop, reg_rbx);
    instrOneOperand(instr_list, instr_pop, reg_rdx);

    instrTwoOperand(instr_list, instr_add, reg_rdx, reg_rbx);

    if (i % 3 == 0) {
      writeInstructionToOutput(instr_list);
      strcpy(instr_list, "");
    }
  }

  writeInstructionToOutput(instr_list);
}


void cgInput(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  struct VariableEntry *symbol_entry;
  char msg[100], format[100], offset[10];

  symbol_entry = (struct VariableEntry *)ic_instr->addr1.value.symbol;
  if (symbol_entry->isArray) {
    inputArrayUtil(symbol_entry);
    return;
  }

  switch (symbol_entry->datatype) {
    case INTEGER:
      strcpy(msg, "input_int_msg");
      strcpy(format, "input_int_format");
      break;
    case REAL:
      strcpy(msg, "input_real_msg");
      strcpy(format, "input_real_format");
      break;
    case BOOLEAN_:
      strcpy(msg, "input_bool_msg");
      strcpy(format, "input_bool_format");
      break;
    default:
      break;
  }

  /* print appropriate message for taking input from user */
  printUtil(instr_list, msg, NULL);

	/*
    load the address of the where the input for scanf will be stored
      1. load rsp in rax
      2. add the mem offset to value in rax => [rsp + mem offset]
      3. store the value from rax to rsi
  */
  instrTwoOperand(instr_list, instr_mov, reg_rax, reg_rsp);
  sprintf(offset, "%d", symbol_entry->mem_offset);
  instrTwoOperand(instr_list, instr_add, reg_rax, offset);

  inputUtil(instr_list, format, reg_rax);

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
    
    case icPRINT:
      cgPrint(ic_instr);
      break;
    
    case icGET:
      cgInput(ic_instr);
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
}


void generateASMCode(ICInstr *ic_instr){
  /* write boiler-plate ASM code to output file */
  initializeASMOutputFile(output_file);
  
  /*
    iterate through intermediate code instructions to
    generate corresponding asm instructions
  */
  ic_instr = ic_instr->next; /* ignore first instruction */
  while (ic_instr != NULL) {
    generateASMInstruction(ic_instr);
    ic_instr = ic_instr->next;
  }

  writeExitToOutputFile(output_file);
}

/* -- end -- */
