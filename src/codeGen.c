#include "codeGen.h"
#include <math.h>

#define OFFSET 8

char *output_file = "output/code.asm";

char reg_rax[] = "rax";
char reg_rbx[] = "rbx";
char reg_rcx[] = "rcx";
char reg_rdx[] = "rdx";
char reg_rsp[] = "rsp";
char reg_rbp[] = "rbp";
char reg_rsi[] = "rsi";
char reg_rdi[] = "rdi";
char reg_r8[] = "r8";

char instr_mov[] = "mov";
char instr_add[] = "add";
char instr_sub[] = "sub";
char instr_mul[] = "imul";
char instr_div[] = "idiv";
char instr_jmp[] = "jmp";
char instr_je[] = "je";
char instr_jne[] = "jne";
char instr_cmp[] = "cmp";
char instr_call[] = "call";
char instr_ret[] = "ret";
char instr_push[] = "push";
char instr_pop[] = "pop";
char instr_inc[] = "inc";
char instr_dec[] = "dec";

char const_0[] = "0";
char const_true[] = "true";
char const_false[] = "false";


/* Global number for labels used */
int rel_op_label_no = 0;
int bool_label_no = 0;
int loop_label_no = 0;


/* functions declarations */
void booleanData(char *data_list);
void outputData(char *data_list);
void inputData(char *data_list);
void errorData(char *data_list);


/* Utility functions */


void initializeOutputFile(char *target_output_file) {
  output_file = target_output_file;

  FILE *fptr = fopen(output_file, "w");
  if (fptr == NULL) {
    fprintf(stderr, "Error!! Failed to open output file.\n");
    exit(-1);
  }
}


void writeToOutputFile(char *data) {
  FILE *fptr = fopen(output_file, "a");
  if (fptr == NULL) {
    fprintf(stderr, "Error!! Failed to open output file.\n");
    exit(-1);
  }
  strcat(data,"\n");
  fputs(data, fptr);
  fclose(fptr);
}


/* Functions for initializing and exiting ASM */


void initializeDirectives() {
  char data_list[400];
  
  /* initialize variables */
  strcpy(data_list, "");

  /* directives */
  strcat(data_list, "global main\n");
  strcat(data_list,"extern printf\n");
  strcat(data_list,"extern scanf\n");
  strcat(data_list,"\n");

  writeToOutputFile(data_list);
}


void initializeTextSection() {
  char data_list[400];
  char tmp_arr[50];
  struct ModuleEntry *entry;
  
  /* initialize variables */
  strcpy(data_list, "");

  /* introduce TEXT section */
  strcat(data_list, "SECTION .text\n");
  strcat(data_list,"\n");
  
  /* label for MAIN */
  strcat(data_list,"main:\n");

  /* instructions for stack alignment */
  strcat(data_list,"\tpush rbx\n");
  strcat(data_list,"\tmov rbp, rsp\n");
  // strcat(data_list,"\tsub rsp, 128\n");
  
  /* push activation record for driver on stack */
  strcpy(tmp_arr, "driver");
  entry = resolveModule(tmp_arr);
  // sprintf(tmp_arr, "\tmov QWORD [rbp + 8], %d\n", entry->activation_record_size - 8);
  sprintf(tmp_arr, "\tsub rsp, %d\n", entry->activation_record_size);
  strcat(data_list, tmp_arr);
  
  writeToOutputFile(data_list);
}


void initializeDataSection() {
  char data_list[1000];

  /* initialize variables */
  strcpy(data_list, "");

  /* introduce DATA section */
  strcat(data_list,"SECTION .data\n");

  /* constants true and false (booleans) */
  booleanData(data_list);
  
  /*
    message & format for printing integer,real and boolean
    contstants and arrays
  */
  outputData(data_list);
  
  /*
    message & format for taking input integer, real and boolean
    contstants and arrays
  */
  inputData(data_list);

  /*
    error messages for various run time checks
  */
  errorData(data_list);

  strcat(data_list, "newline:\n");
  strcat(data_list, "\tdb 10, 0\n");

  writeToOutputFile(data_list);
}


void booleanData(char *data_list) {
  /* constants true and false (booleans) */
  strcat(data_list, "true:\n");
  strcat(data_list, "\tdb \"true\", 0\n");
  strcat(data_list, "false:\n");
  strcat(data_list, "\tdb \"false\", 0\n");
}


void outputData(char *data_list) {
  /* message & format for printing integer */
  strcat(data_list, "output_int_msg:\n");
  strcat(data_list, "\tdb \"Output: %ld\", 10, 0\n");

  /* message & format for printing real */
  strcat(data_list, "output_real_msg:\n");
  strcat(data_list, "\tdb \"Output: %2f\", 10, 0\n");

  /* message & format for printing boolean */
  strcat(data_list, "output_bool_msg:\n");
  strcat(data_list, "\tdb \"Output: %s\", 10, 0\n");

  /* message & format for printing array */
  strcat(data_list, "output_array_msg_1:\n");
  strcat(data_list, "\tdb \"Output: \", 0\n");
  strcat(data_list, "output_array_msg_2_int:\n");
  strcat(data_list, "\tdb \"%ld \", 0\n");
  strcat(data_list, "output_array_msg_2_real:\n");
  strcat(data_list, "\tdb \"%2f \", 0\n");
  strcat(data_list, "output_array_msg_2_bool:\n");
  strcat(data_list, "\tdb \"%s \", 0\n");
}


void inputData(char *data_list) {
  /* message for taking input integer */
  strcat(data_list, "input_int_msg:\n");
  strcat(data_list, "\tdb \"Input: Enter an integer value:\", 10, 0\n");
  /* format for taking input numbers */
  strcat(data_list, "input_int_format:\n");
  strcat(data_list, "\tdb \"%ld\", 0\n");

  /* message for taking input real */
  strcat(data_list, "input_real_msg:\n");
  strcat(data_list, "\tdb \"Input: Enter a real value:\", 10, 0\n");
  /* format for taking input real */
  strcat(data_list, "input_real_format:\n");
  strcat(data_list, "\tdb \"%2f\", 0\n");

  /* message for taking input boolean */
  strcat(data_list, "input_bool_msg:\n");
  strcat(data_list, "\tdb \"Input: Enter a boolean value:\", 10, 0\n");
  /* format for taking input booleans */
  strcat(data_list, "input_bool_format:\n");
  strcat(data_list, "\tdb \"%ld\", 0\n");

  /* message for taking input array */
  strcat(data_list, "input_array_msg_1:\n");
  strcat(data_list, "\tdb \"Input: Enter %d array elements of \", 0\n");

  strcat(data_list, "input_array_msg_2_int:\n");
  strcat(data_list, "\tdb \"integer type for range \", 0\n");

  strcat(data_list, "input_array_msg_2_real:\n");
  strcat(data_list, "\tdb \"real type for range \", 0\n");

  strcat(data_list, "input_array_msg_2_bool:\n");
  strcat(data_list, "\tdb \"boolean type for range \", 0\n");

  strcat(data_list, "input_array_msg_3:\n");
  strcat(data_list, "\tdb \"%d to \", 0\n");

  strcat(data_list, "input_array_msg_4:\n");
  strcat(data_list, "\tdb \"%d\", 10, 0\n");

}


void errorData(char *data_list) {
  /* error message: array out of bounds */
  strcat(data_list, "array_out_of_bounds_error:\n");
  strcat(data_list, "\tdb \"Array index is out of bounds\", 10, 0\n");
}


void exitCodeTextSection() {
  char data_list[400];
  // char tmp_var[50];
  // struct ModuleEntry *entry;

  /* intiliaze variables */
  strcpy(data_list, "");

  /* label to jump to exit code */
  strcat(data_list,"\nexit:\n");

  /* pop the activation record of driver from stack */
  // strcpy(tmp_var, "driver");
  // entry = resolveModule(tmp_var);
  // sprintf(tmp_var, "\tadd rbp, %d\n", entry->activation_record_size);
  // strcat(data_list, tmp_var);

  /* reverse instructions for stack alignment */
  // strcat(data_list,"\tadd rbp, 32\n");
  strcat(data_list,"\tmov rsp, rbp\n");
  strcat(data_list,"\tpop rbx\n");
  
  /* return from main */
  strcat(data_list,"\tret\n");

  writeToOutputFile(data_list);
}


/* Utility functions for Code Generation */


void getLabelRelationalOp(char *label) {
  sprintf(label, "RelOpL%d", rel_op_label_no);
  rel_op_label_no += 1;
}


void getLabelBoolean(char *label) {
  sprintf(label, "BoolL%d", bool_label_no);
  bool_label_no += 1;
}


void getLabelLoop(char *label) {
  sprintf(label, "LoopL%d", loop_label_no);
  loop_label_no += 1;
}


void labelUtil(char *instr_list, char *label) { 
  strcat(instr_list,label);
  strcat(instr_list, ":\n");
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


/* Other funcions for Code Generation */


void cgICAddr(char *instr_list, char *addr, ICAddr *ic_addr) {
  struct VariableEntry *symbol_entry;
  char tmp[50];
  int data_size;
  
  /* for static arrays */
  char lower_bound_str[50];
  int base_offset, num_elements;
  int lower_bound, upper_bound;
  
  switch (ic_addr->type) {
  case INTEGER:
    sprintf(addr, "%d", ic_addr->value.num);
    break;
  case REAL:
    sprintf(addr, "%f", ic_addr->value.rnum);
    break;
  case BOOLEAN_:
    if (ic_addr->value.boolean == true)
      sprintf(addr, "1");
    else
      sprintf(addr, "0");
    break;
  case IDENTIFIER:
    if (ic_addr->is_label) {
      strcpy(addr, (char *) ic_addr->value.symbol);
    }
    else {
      symbol_entry = (struct VariableEntry *)ic_addr->value.symbol;
      if (symbol_entry->mem_offset >= 0)
        sprintf(addr, "[rbp - %d]", symbol_entry->mem_offset + OFFSET);
      else
        sprintf(addr, "[rbp + %d]", abs(symbol_entry->mem_offset) + OFFSET);
    }
    break;
  case ARRAY:
    symbol_entry = (struct VariableEntry *) ic_addr->value.array.var;
    
      base_offset = symbol_entry->mem_offset;
      data_size = 8;

    if (symbol_entry->isStatic) {
      lower_bound = symbol_entry->lower_bound->value.num;
      upper_bound = symbol_entry->upper_bound->value.num;

      num_elements = upper_bound - lower_bound + 1;
      base_offset = base_offset + OFFSET * 3 + num_elements * data_size;

      instrTwoOperand(instr_list, instr_mov, reg_rdx, reg_rbp);
      
      sprintf(tmp, "%d", base_offset);
      instrTwoOperand(instr_list, instr_sub, reg_rdx, tmp);

      sprintf(lower_bound_str, "%d", symbol_entry->lower_bound->value.num);
    }
    else {
      /*
        load the base address of array from memory location
        of array pointer
      */
      sprintf(tmp, "[rbp - %d]", base_offset + OFFSET);
      instrTwoOperand(instr_list, instr_mov, reg_rdx, tmp);
      
      sprintf(lower_bound_str, "[rbp - %d]",
        symbol_entry->lower_bound->mem_offset + OFFSET);
    }

    /* load the index of array access in register RBX */
    cgICAddr(instr_list, addr, ic_addr->value.array.idx);
    instrTwoOperand(instr_list, instr_mov, reg_rbx, addr);

    /* subtract the lower bound from the index in register RBX */
    instrTwoOperand(instr_list, instr_sub, reg_rbx, lower_bound_str);
    
    sprintf(addr, "[%s + %s * %d]", reg_rdx, reg_rbx, data_size);
    break;

  default:
    sprintf(addr, "NULL");
    break;
  }
}


void cgMoveFromMemToReg(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char addr[50];
  
  cgICAddr(instr_list, addr, ic_addr);  
  instrTwoOperand(instr_list, instr_mov, reg, addr);
}


void cgMoveFromRegToMem(char *instr_list, char *reg, ICAddr *ic_addr) { 
  char addr[50];
  
  cgICAddr(instr_list, addr, ic_addr);
  instrTwoOperand(instr_list, instr_mov, addr, reg);
}


/* Functions for generating ASM Code from Intermediate Code */


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
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));

  /* instruction: negate the value in reg AX using unary minus */
  strcpy(op, "neg");
  instrOneOperand(instr_list, op, reg_rax);

  /* instruction: store the value of reg AX in mem addr3 */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr3));
 
  writeToOutputFile(instr_list);
}


void cgLogicalOp(ICInstr *ic_instr) {
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
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: load second operand from mem addr2 to reg BL */
  cgMoveFromMemToReg(instr_list, reg_rbx, &(ic_instr->addr2));

  /*
    instruction: perform logical operation AND or OR
    between regs AL and BL, and store the result in AL
  */
  instrTwoOperand(instr_list, op, reg_rax, reg_rbx);
  
  /* instruction: store the value from reg AL to mem addr3  */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr3));

  writeToOutputFile(instr_list);
}


void cgRelationalOp(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char label1[20];
  char label2[20];
  char op[5];

  getLabelRelationalOp(label1);
  getLabelRelationalOp(label2);
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
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: load second operand from mem addr2 to reg BX */
  cgMoveFromMemToReg(instr_list, reg_rbx, &(ic_instr->addr2));

  /* instruction: compare values in regs AX and BX */
  instrTwoOperand(instr_list, "cmp", reg_rax, reg_rbx);
  
  /* instruction: perform jump operation w.r.t relational op */
  instrOneOperand(instr_list, op, label1); 
  
  /*
    False Case:
    if the result of above relational comparison is false,
    then execute:
      1. instruction: store value false (0x00) in reg CL
      2. instruction: jump to label2 (end of set of relational instrutions)
  */
  instrTwoOperand(instr_list, "mov", reg_rcx, "0"); //false
  instrOneOperand(instr_list, "jmp", label2); //jump to label2

  /*
    True Case:
    if the result of above relational comparison is true,
    then jump to label1, and execute:
      1. instruction: set label for value for label1
      2. instruction: store value true (0x11) in reg CL
  */
  labelUtil(instr_list, label1);
  instrTwoOperand(instr_list, "mov", reg_rcx, "1");

  /* instruction: set label for value of label2*/
  labelUtil(instr_list,label2);
  
  /* instruction: store value from reg CL to mem addr3 */
  cgMoveFromRegToMem(instr_list, reg_rcx, &(ic_instr->addr3));
  
  writeToOutputFile(instr_list);
}


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
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));

  /* instruction: load from mem addr2 to reg BX */
  cgMoveFromMemToReg(instr_list, reg_rbx, &(ic_instr->addr2));
  
  /*
    instruction: perform arithmetic op between values in regs
    AX and BX, and store the result in reg AX
  */
  instrTwoOperand(instr_list, op, reg_rax, reg_rbx);

  /* instruction: store the Quotient from reg AX to mem addr3 */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr3));
  
  writeToOutputFile(instr_list);
}


void cgMultiplyInteger(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";

  /* instruction: load from mem addr1 to reg AX */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: load from mem addr2 to reg BX */
  cgMoveFromMemToReg(instr_list, reg_rcx, &(ic_instr->addr2));

  /*
    instruction: multiply value in reg AX with value in reg BX
    the result is store in DX:AX, as follow:
      1. DX stores the upper 16 bytes
      2. AX stores the lower 16 bytes
  */
  instrTwoOperand(instr_list,"imul", reg_rax, reg_rcx);
  
  /*
    instruction: store the last 16 bytes of the result
    from reg AX to mem addr3
  */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr3));

  writeToOutputFile(instr_list);
}


void cgDivisionInteger(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";

  /* instruction: load from mem addr1 to reg AX */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: load from mem addr2 to reg BX */
  cgMoveFromMemToReg(instr_list, reg_rcx, &(ic_instr->addr2));

  /*
    instruction: divide value in reg AX by value in reg BX
    the result is store in DX:AX, as follow:
      1. DX stores the remainder
      2. AX stores the quotient
  */
  instrOneOperand(instr_list, "idiv", reg_rcx);
  
  /* instruction: store the Quotient from reg AX to mem addr3 */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr3));
 
  writeToOutputFile(instr_list);
}


void cgJumpUnconditional(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char label[50];
  
  /* intialize label with the jump label value */
  cgICAddr(instr_list, label, &(ic_instr->addr1));
  
  /* instruction: jump to the label (unconditionally) */
  instrOneOperand(instr_list,"jmp",label);
 
  writeToOutputFile(instr_list);
}


void cgJumpConditional(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char instr[10];
  char label[20];
  
  /* intialize label with the jump label value */
  cgICAddr(instr_list, label, &(ic_instr->addr2));

  /* instruction: load test value in reg AX */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: compare the test value with 0 */
  instrTwoOperand(instr_list, "cmp", reg_rax, "0");
  
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
 
  writeToOutputFile(instr_list);
}


void cgIncrement(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";

  /* instruction: load from mem addr1 to reg AX */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));

  /* instruction: increment value in reg AX and store it in reg AX */
  strcat(instr_list, "inc\tax\n");

  /* instruction: store from reg AX to mem addr1 */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr1));
  
  writeToOutputFile(instr_list);
}


void cgDecrement(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  
  /* instruction: load from mem addr1 to reg AX */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: decrement value in reg AX and store it in reg AX */
  strcat(instr_list, "dec\tax\n");
  
  /* instruction: store from reg AX to mem addr1 */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr1));
  
  writeToOutputFile(instr_list);
}


void cgMoveFromMemToMem(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";

  /* instruction: load from mem addr1 to reg AX */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* instruction: store from reg AX to mem addr3 */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr3));

  writeToOutputFile(instr_list);
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
  
  getLabelBoolean(label1);
  getLabelBoolean(label2);

  instrTwoOperand(instr_list, instr_mov, reg_rax, addr);

  instrTwoOperand(instr_list, instr_cmp, reg_rax, const_0);
  instrOneOperand(instr_list, instr_je, label1);

  instrTwoOperand(instr_list, instr_mov, reg_rax, const_true);
  instrOneOperand(instr_list, instr_jmp, label2);

  labelUtil(instr_list, label1);
  instrTwoOperand(instr_list, instr_mov, reg_rax, const_false);

  labelUtil(instr_list, label2);
  strcat(instr_list, "\n");

  strcpy(addr, "rax");
}


void mem(char *mem, char *addr) {
  strcpy(mem, "");
  sprintf(mem, "[%s]", addr);
}


void staticArrayUtil(char *instr_list, struct VariableEntry *symbol_entry) {
  char tmp[50];
  int base_offset, data_size;
  int lower_bound, upper_bound;
  int num_elements;

  base_offset = symbol_entry->mem_offset;
  lower_bound = symbol_entry->lower_bound->value.num;
  upper_bound = symbol_entry->upper_bound->value.num;
  data_size = 8;

  num_elements = upper_bound - lower_bound + 1;
  base_offset = base_offset + OFFSET * 3 + num_elements * data_size;
  
  /* compute base of array */
  instrTwoOperand(instr_list, instr_mov, reg_rdx, reg_rbp);

  /* compute base of array */
  sprintf(tmp, "%d", base_offset);
  instrTwoOperand(instr_list, instr_sub, reg_rdx, tmp);
  
  /* load the lower bound in rcx for loop */
  sprintf(tmp, "%d", lower_bound);
  instrTwoOperand(instr_list, instr_mov, reg_rcx, tmp);

  /* load the upper bound in rax for loop */
  sprintf(tmp, "%d", upper_bound);
  instrTwoOperand(instr_list, instr_mov, reg_rax, tmp);

  /* load the size of a data element */
  sprintf(tmp, "%d", data_size);
  instrTwoOperand(instr_list, instr_mov, reg_rbx, tmp);

  /* calculate number of elements in array */
  sprintf(tmp, "%d", num_elements);
  instrTwoOperand(instr_list, instr_mov, reg_r8, tmp);
}


void dynamicArrayUtil(char *instr_list, struct VariableEntry *symbol_entry) {
  char tmp[50];
  int base_offset, data_size;
  int lower_bound_offset, upper_bound_offset;

  base_offset = symbol_entry->mem_offset;
  lower_bound_offset = symbol_entry->lower_bound->mem_offset;
  upper_bound_offset = symbol_entry->upper_bound->mem_offset;
  data_size = 8;

  /* load base address of array from array pointer */
  sprintf(tmp, "[rbp - %d]", base_offset + OFFSET);
  instrTwoOperand(instr_list, instr_mov, reg_rdx, tmp);

  /* load the lower bound in rcx for loop */
  sprintf(tmp, "[rbp - %d]", lower_bound_offset + OFFSET);
  instrTwoOperand(instr_list, instr_mov, reg_rcx, tmp);

  /* load the upper bound in rax for loop */
  sprintf(tmp, "[rbp - %d]", upper_bound_offset + OFFSET);
  instrTwoOperand(instr_list, instr_mov, reg_rax, tmp);

  /* load the size of a data element */
  sprintf(tmp, "%d", data_size);
  instrTwoOperand(instr_list, instr_mov, reg_rbx, tmp);

  /* calculate number of elements in array */
  instrTwoOperand(instr_list, instr_mov, reg_r8, reg_rax);
  instrTwoOperand(instr_list, instr_sub, reg_r8, reg_rcx);
  strcpy(tmp, "1");
  instrTwoOperand(instr_list, instr_add, reg_r8, tmp);
}


void pushRegUtil(char *instr_list) {
  instrOneOperand(instr_list, instr_push, reg_rdx);
  instrOneOperand(instr_list, instr_push, reg_rcx);
  instrOneOperand(instr_list, instr_push, reg_rbx);
  instrOneOperand(instr_list, instr_push, reg_rax);
  strcat(instr_list, "\n");
}


void popRegUtil(char *instr_list) {
  instrOneOperand(instr_list, instr_pop, reg_rax);
  instrOneOperand(instr_list, instr_pop, reg_rbx);
  instrOneOperand(instr_list, instr_pop, reg_rcx);
  instrOneOperand(instr_list, instr_pop, reg_rdx);
  strcat(instr_list, "\n");
}


void printArrayUtil(struct VariableEntry *symbol_entry) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char tmp[50], format[50], label[50];  

  if (symbol_entry->isStatic)
    staticArrayUtil(instr_list, symbol_entry);
  else
    dynamicArrayUtil(instr_list, symbol_entry);
  strcat(instr_list, "\n");

  pushRegUtil(instr_list);
  strcpy(format, "output_array_msg_1");
  printUtil(instr_list, format, NULL);
  popRegUtil(instr_list);
  
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

  strcpy(tmp, "1");
  instrTwoOperand(instr_list, instr_add, reg_rax, tmp);

  /* set up loop label */
  getLabelLoop(label);
  labelUtil(instr_list, label);

  mem(tmp, reg_rdx);

  pushRegUtil(instr_list);
  printUtil(instr_list, format, tmp);
  popRegUtil(instr_list);

  instrTwoOperand(instr_list, instr_add, reg_rdx, reg_rbx);

  /*
    1. increment counter
    2. cmp with upper bound
    3. jmp if not equal
  */
  instrOneOperand(instr_list, instr_inc, reg_rcx);
  instrTwoOperand(instr_list, instr_cmp, reg_rcx, reg_rax);
  instrOneOperand(instr_list, instr_jne, label);

  strcpy(format, "newline");
  printUtil(instr_list, format, NULL);

  writeToOutputFile(instr_list);
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
    // if (!symbol_entry->isStatic)
    //   return;
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

  writeToOutputFile(instr_list);
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
  char tmp[50], format[50], label[50];

  if (symbol_entry->isStatic)
    staticArrayUtil(instr_list, symbol_entry);
  else
    dynamicArrayUtil(instr_list, symbol_entry);
  strcat(instr_list, "\n");

  /* print number of elements in the array */
  pushRegUtil(instr_list);
  strcpy(format, "input_array_msg_1");
  printUtil(instr_list, format, reg_r8);
  popRegUtil(instr_list);
  
  /* print data type of the array */
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
  pushRegUtil(instr_list);
  printUtil(instr_list, format, NULL);
  popRegUtil(instr_list);

  /* print lower bound of the array */
  pushRegUtil(instr_list);
  strcpy(format, "input_array_msg_3");
  printUtil(instr_list, format, reg_rcx);
  popRegUtil(instr_list);

  /* print upper bound of the array */
  pushRegUtil(instr_list);
  strcpy(format, "input_array_msg_4");
  printUtil(instr_list, format, reg_rax);
  popRegUtil(instr_list);

  strcpy(tmp, "1");
  instrTwoOperand(instr_list, instr_add, reg_rax, tmp);

  /* set up loop label */
  getLabelLoop(label);
  labelUtil(instr_list, label);

  strcpy(format, "input_int_format");

  /* accept input for an array element */
  pushRegUtil(instr_list);
  inputUtil(instr_list, format, reg_rdx);
  popRegUtil(instr_list);

  /* compute the location of next element */
  instrTwoOperand(instr_list, instr_add, reg_rdx, reg_rbx);

  /*
    1. increment counter
    2. cmp with upper bound
    3. jmp if not equal
  */
  instrOneOperand(instr_list, instr_inc, reg_rcx);
  instrTwoOperand(instr_list, instr_cmp, reg_rcx, reg_rax);
  instrOneOperand(instr_list, instr_jne, label);

  writeToOutputFile(instr_list);
}


void cgInput(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  struct VariableEntry *symbol_entry;
  char msg[100], format[100], offset[50];

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
    load the address of where the scanf input  will be stored
      1. load rbp in rax
      2. subtract the mem offset to value in rax => [rbp - mem offset]
      3. store the value from rax to rsi
  */
  instrTwoOperand(instr_list, instr_mov, reg_rax, reg_rbp);
  sprintf(offset, "%d", symbol_entry->mem_offset + OFFSET);
  instrTwoOperand(instr_list, instr_sub, reg_rax, offset);

  inputUtil(instr_list, format, reg_rax);

  writeToOutputFile(instr_list);
}


void cgDynamicArrayAllocation(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char tmp[50];
  struct VariableEntry *symbol_entry;
  int lower_bound_offset, upper_bound_offset, data_size;

  symbol_entry = (struct VariableEntry *) ic_instr->addr1.value.symbol;
  lower_bound_offset = symbol_entry->lower_bound->mem_offset;
  upper_bound_offset = symbol_entry->upper_bound->mem_offset;
  data_size = 8;

  /*
    Compute total size of the array:
    1. load the upper bound in register RBX
    2. subtract lower bound from this
    3. add 1 to get the total count
    4. multiply with data size to get the total size
  */
  sprintf(tmp, "[rbp - %d]", upper_bound_offset + OFFSET);
  instrTwoOperand(instr_list, instr_mov, reg_rbx, tmp);

  sprintf(tmp, "[rbp - %d]", lower_bound_offset + OFFSET);
  instrTwoOperand(instr_list, instr_sub, reg_rbx, tmp);

  strcpy(tmp, "1");
  instrTwoOperand(instr_list, instr_add, reg_rbx, tmp);

  sprintf(tmp, "%d", data_size);
  instrTwoOperand(instr_list, instr_mul, reg_rbx, tmp);

  /*
    Compute the offset for base of array by subtracting
    the total size of the array (in register RBX) from 
    end of current activation record (in register RSP)
  */
  instrTwoOperand(instr_list, instr_sub, reg_rsp, reg_rbx);

  /*
    store the calculated base addres in memory
    location of the array pointer
  */
  cgICAddr(instr_list, tmp, &(ic_instr->addr1));
  instrTwoOperand(instr_list, instr_mov, tmp, reg_rsp);
  
  writeToOutputFile(instr_list);
}


void cgLabel(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char *label;

  /* intialize variables */
  label = (char *)(ic_instr->addr1.value.symbol);
  
  /* asm code for label */
  labelUtil(instr_list, label);

  writeToOutputFile(instr_list);
}


void cgArrayOutOfBoundsError(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char tmp[50];

  strcpy(tmp, "array_out_of_bounds_error");
  printUtil(instr_list, tmp, NULL);

  strcpy(tmp, "exit");
  instrOneOperand(instr_list, instr_jmp, tmp);

  writeToOutputFile(instr_list);
}


void cgCallFunction(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char addr[50];

  cgICAddr(instr_list, addr, &(ic_instr->addr1));
  instrOneOperand(instr_list, instr_call, addr);

  writeToOutputFile(instr_list);
}


void cgEnterFunction(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char addr[50];
  
  /*
    push the current value of base register to preserve
    the base address of activation record of caller
  */
  instrOneOperand(instr_list, instr_push, reg_rbp);

  /* update the base register with base address of callee */
  instrTwoOperand(instr_list, instr_mov, reg_rbp, reg_rsp);
  
  /*
    push the activation record of the callee by subtracting
    the size of the activation record from base pointer
  */
  cgICAddr(instr_list, addr, &(ic_instr->addr1));
  instrTwoOperand(instr_list, instr_sub, reg_rsp, addr);

  writeToOutputFile(instr_list);
}


void cgReturnFromFunction(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";

  /*
    discard the activation record of callee, and
    all the dynamic arrays (if any)
  */
  instrTwoOperand(instr_list, instr_mov, reg_rsp, reg_rbp);

  /*
    restore old value of base register i.e.
    base of activation record of caller
  */
  instrOneOperand(instr_list, instr_pop, reg_rbp);

  /* return to the caller function */
  strcat(instr_list, "ret\n");

  writeToOutputFile(instr_list);
}


void cgPushToStack(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  
  /* load the value to be pushed in register RAX from memory */
  cgMoveFromMemToReg(instr_list, reg_rax, &(ic_instr->addr1));
  
  /* push the value in register RAX on the stack */
  instrOneOperand(instr_list, instr_push, reg_rax);

  writeToOutputFile(instr_list);
}


void cgPopFromStack(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  
  /* pop the value from top of stack and store in register RAX */
  instrOneOperand(instr_list, instr_pop, reg_rax);

  /* store the value in register RAX in memory */
  cgMoveFromRegToMem(instr_list, reg_rax, &(ic_instr->addr1));

  writeToOutputFile(instr_list);
}


void cgShadowPushStack(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char addr[50];

  /* get the value in address */
  cgICAddr(instr_list, addr, &(ic_instr->addr1));

  /*
    add the above value from stack pointer to
    increase space on stack
  */
  instrTwoOperand(instr_list, instr_sub, reg_rsp, addr);

  writeToOutputFile(instr_list);
}


void cgShadowPopStack(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char addr[50];

  /* get the value in address */
  cgICAddr(instr_list, addr, &(ic_instr->addr1));

  /*
    add the above value from stack pointer to
    decrease space on stack
  */
  instrTwoOperand(instr_list, instr_add, reg_rsp, addr);

  writeToOutputFile(instr_list);
}


/* Functions for interfacing with ASM Code Generation */

void generateASMInstruction(ICInstr *ic_instr) {
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

    case icCALL:
      cgCallFunction(ic_instr);
      break;
    
    case icENTER:
      cgEnterFunction(ic_instr);
      break;
    
    case icRET:
      cgReturnFromFunction(ic_instr);
      break;
    
    case icPUSH:
      cgPushToStack(ic_instr);
      break;
    
    case icPOP:
      cgPopFromStack(ic_instr);
      break;
    
    case icPUSH_SHADOW:
      cgShadowPushStack(ic_instr);
      break;
    
    case icPOP_SHADOW:
      cgShadowPopStack(ic_instr);
      break;

    case icARRAY:
      cgDynamicArrayAllocation(ic_instr);
      break;

    case icLABEL:
      cgLabel(ic_instr);
      break;

    case icErrARRAY:
      cgArrayOutOfBoundsError(ic_instr);
      break;

    default:
      break;
  }
}


void generateASMCode(ICInstr *ic_instr, char *output_file) {
  initializeOutputFile(output_file);
  
  /* write directives and initialize text section */
  initializeDirectives();
  initializeTextSection();
  
  /*
    iterate through intermediate code instructions to
    generate corresponding asm instructions
  */
  ic_instr = ic_instr->next; /* ignore first instruction */
  while (ic_instr != NULL) {
    generateASMInstruction(ic_instr);
    ic_instr = ic_instr->next;
  }

  /* write instructions for exiting and populating data section */
  exitCodeTextSection();
  initializeDataSection();
}

/* -- end -- */
