#include "codeGen.h"

char *output_file = "output/main_asm.asm";

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

void insertVarFloat(char *tmp_var, char *name, char *type, float value)
{
  sprintf(tmp_var, "\t%s:\t%s\t%f\n", name, type, value);
}
void insertVarInt(char *tmp_var, char *name, char *type, int value)
{
  sprintf(tmp_var, "\t%s:\t%s\t%d\n", name, type, value);
}

void insert_fmt(char *tmp_var, char *name, char *per_val)
{
  sprintf(tmp_var, "\t%s:\tdb\t\"Output: %s\", 10, 0\n", name, per_val);
}
void populateASMdata(char *output_file){
  char data_list[400];
  char tmp_var[50];
  printf("\nWriting assembled code to: %s\n",output_file);
  strcpy(data_list,"extern printf\n");
  strcat(data_list,"SECTION .data\n");
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
  strcat(data_list, "SECTION .text\n");
  strcat(data_list, "\tglobal main\n");
  strcat(data_list,"main:\n");
  //add to file
  FILE *fptr = fopen(output_file,"w");
  fputs(data_list,fptr);
  fclose(fptr);
}

void appendData(char *data){
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


/* Actual functions for populating asm code */

void cgADD_SUB_INT(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  char op[10] = "";

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_bx, &(ic_instr->addr2));
  
  switch (ic_instr->op) {
  case icADD_INT:
    strcpy(op, "add");
    instrTwoOperand(instr_list, op, reg_ax, reg_bx);
    break;
  case icSUB_INT:
    strcpy(op, "sub");
    instrTwoOperand(instr_list, op, reg_ax, reg_bx);
    break;
  default:
    printf("Error: Invalid Integer Operation\n");
  }

  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
  
  appendData(instr_list);
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

  appendData(instr_list);
}


void cgMUL_INT(ICInstr *ic_instr){ //---> so far only int
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  // strcat(instr_list,"imul\tax , cx\n");
  instrTwoOperand(instr_list,"imul",reg_ax,reg_cx);
  // The result of this MUL goes into DX:AX- for now we just store AX
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  appendData(instr_list);
}


void cgDIV_INT(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = ""; //final instruction -temporary max length as 100

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_cx, &(ic_instr->addr2));

  // strcat(instr_list, "idiv\tcx\n");
  instrOneOperand(instr_list,"idiv",reg_cx);
  // The result of this IDIV goes into DX:AX- AX has the quotient
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
 
  appendData(instr_list);
}

void cgINC(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "inc\tax\n");
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  appendData(instr_list);
}


void cgDEC(ICInstr *ic_instr) {
  char instr_list[MAX_SIZE_INSTR] = "";
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  strcat(instr_list, "dec\tax\n");
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr1));
  
  appendData(instr_list);
}


void cgSTOREVALUE_INT(ICInstr *ic_instr){ //---> not fully working- indexed elements of arrays need to be added
  char instr_list[MAX_SIZE_INSTR] = "";

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));

  appendData(instr_list);
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
      strcpy(operator, "je");
      break;
    case icNE:
      strcpy(operator, "jne");
      break;
    case icLT:
      strcpy(operator,"jl");
      break;
    case icGT:
      strcpy(operator, "jg");
      break;
    case icLE:
      strcpy(operator, "jle");
      break;
    case icGE:
      strcpy(operator, "jge");
      break;
    default:
      strcpy(operator, "BLAH1");
      break;
  } 
  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_bx, &(ic_instr->addr2));

  instrTwoOperand(instr_list,"cmp",reg_ax,reg_bx);
  //instr eg -> "JLE L1"
  instrOneOperand(instr_list,operator,label1); 
  //this runs if above condition isnt true
  instrTwoOperand(instr_list, "mov", reg_cl, "0x00"); //false
  instrOneOperand(instr_list, "jmp", label2); //jump to label2

  addLabel(instr_list,label1); //true => it comes here
  instrTwoOperand(instr_list, "mov", reg_cl, "0x11"); //true

  addLabel(instr_list,label2); // if false, the other condition jumps here
  // store value in addr3
  cgStoreINT(instr_list, reg_cl, &(ic_instr->addr3));
  
  appendData(instr_list);
}

void cgLogicalOp(ICInstr *ic_instr) {   //-----> for AND,OR
  char instr_list[MAX_SIZE_INSTR] = "";
  char op[10] = "";

  //assuming that booleans are a byte long
  cgLoadINT(instr_list, reg_al, &(ic_instr->addr1));
  cgLoadINT(instr_list, reg_bl, &(ic_instr->addr2));

  switch (ic_instr->op){
  case icAND:
    strcpy(op, "and");
    instrTwoOperand(instr_list, op, reg_al, reg_bl);
    break;
  case icOR:
    strcpy(op, "or");
    instrTwoOperand(instr_list, op, reg_al, reg_bl);
    break;
  default:
    printf("Error: Invalid Integer Operation\n");
  }
  //store result in reg_al to memory
  cgStoreINT(instr_list, reg_al, &(ic_instr->addr3));

  appendData(instr_list);
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
    strcpy(op, "neg");
    instrOneOperand(instr_list, op, reg_ax);
    break;
  default:
    printf("Error: Invalid Unary Op\n");
  }
  //store result in reg_al to memory
  cgStoreINT(instr_list, reg_ax, &(ic_instr->addr3));
 
  appendData(instr_list);
}

void cgJUMP(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char label[50];
  cgICAddr(instr_list, label, &(ic_instr->addr1));
  instrOneOperand(instr_list,"jmp",label);
 
  appendData(instr_list);
}

void cgJUMP_NZ_Z(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  char instr[10];
  char label[20];
  cgICAddr(instr_list, label, &(ic_instr->addr2)); //label

  cgLoadINT(instr_list, reg_ax, &(ic_instr->addr1));
  instrTwoOperand(instr_list,"cmp",reg_ax,"0");
  if(ic_instr->op == icJUMPZ){
    strcpy(instr,"je");
  }
  else{
    strcpy(instr, "jne");
  }
  instrOneOperand(instr_list,instr,label);
 
  appendData(instr_list);
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

  appendData(instr_list);
}

void cgLabel(ICInstr *ic_instr){
  char instr_list[MAX_SIZE_INSTR] = "";
  ICAddr *a = &(ic_instr->addr1);
  char *label = (char *)(a->value.symbol);
  addLabel(instr_list,label);
  appendData(instr_list);
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
      cgLabel(ic_instr);
      break;
    default:
      break;
  }
  // printf("\n");
}

void printCodeGen(ICInstr *ic_instr){
  populateASMdata(output_file);
  while (ic_instr != NULL)
  {
    printInstrCG(ic_instr);
    ic_instr = ic_instr->next;
  }
}


