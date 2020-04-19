
// char reg_xmm0[] = "xmm0";
// char reg_xmm1[] = "xmm1";

// char tmp_float_var1[] = "float_temp1";
// char tmp_float_var2[] = "float_temp2";


// void cgRealOneOp(char *instr_list, char *op, char *type, char *addr) { // FLD dword[tmp1]
//   char rhs[50];
//   sprintf(rhs,"%s[%s]",type,addr);
//   instrOneOperand(instr_list, op, rhs);
// }


// void cgLoadRealConstIntoTmp(char *instr_list, char *type, char *tmp_var, ICAddr *ic_addr){ // mov dword[tmp1] , __float32__(23.0)
//   char op[] = "mov";
//   char addr1[30];
//   char addr2[30];

//   sprintf(addr1,"%s[%s]",type,tmp_var);
//   sprintf(addr2, "__float32__(%f)", ic_addr->value.rnum);
//   instrTwoOperand(instr_list, op, addr1, addr2);
// }


// void cgArithmeticOpReal(ICInstr *ic_instr) {
//   char instr_list[MAX_SIZE_INSTR] = "";
//   char addr1[20];
//   char type1[20];
//   char addr2[20];
//   char type2[20];
//   char addr3[20];
//   char op[10] = "";

//   // num1
//   if((&(ic_instr->addr1))->type == RNUM){
//     cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var1, &(ic_instr->addr1));
//     sprintf(addr1, "%s", tmp_float_var1);
//     strcpy(type1,"dword");
//   }
//   else{
//     sprintf(addr1, "%s", (char*)(&(ic_instr->addr1))->value.symbol);
//     strcpy(type1, "qword");
//   }
//   // num 2
//   if((&(ic_instr->addr2))->type == RNUM){
//     cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var2, &(ic_instr->addr2));
//     sprintf(addr2, "%s", tmp_float_var2);
//     strcpy(type2,"dword");
//   }
//   else{
//     sprintf(addr2, "%s", (char*)(&(ic_instr->addr2))->value.symbol);
//     strcpy(type2, "qword");
//   }
  
//   //now to load first number into float stack -eg. FLD dword[tmp1]
//   cgRealOneOp(instr_list,"fld",type1,addr1);

//   switch (ic_instr->op) {
//   case icADD_REAL:
//     strcpy(op, "fadd");
//     break;
//   case icSUB_REAL:
//     strcpy(op, "fsub");
//     break;
//   case icMUL_REAL:
//     strcpy(op, "fmul");
//     break;
//   case icDIV_REAL:
//     strcpy(op, "fdiv");
//     break;
//   default:
//     printf("Error: Invalid Float Operation\n");
//   }

//   // now, apply operation with the second number on the first number
//   cgRealOneOp(instr_list,op,type2,addr2);
//   // finally to store it in addr3 variable
//   sprintf(addr3, "%s", (char *)(&(ic_instr->addr3))->value.symbol);
//   cgRealOneOp(instr_list,"FSTP","qword",addr3); 

//   writeInstructionToOutput(instr_list);
// }


// void print_var(ICInstr *ic_instr){
//   //assuming the convention of intcode is PRINT *VAR* -> VAR can be NUM,RNUM,BOOLEAN, or IDENTIFIER
//   // also, assumption that the concerned data needed for formats etc. are in data section
//   // needed- print_fmt_int, print_fmt_float, print_fmt_bool_false, print_fmt_bool_true
//   char instr_list[MAX_SIZE_INSTR] = "";
//   ICAddr *ic_addr = &(ic_instr->addr1);
//   instrOneOperand(instr_list,"push","rbp");
//   if(ic_addr->type != IDENTIFIER){
//     print_output_type(instr_list,ic_addr->type,ic_addr,0);
//   }
//   else{
//     print_output_type(instr_list, ic_addr->type, ic_addr, 1); //to be implemented
//   }
//   instrOneOperand(instr_list, "pop", "rbp");

//   writeInstructionToOutput(instr_list);
// }


// void print_output_type(char *instr_list, enum terminal type,ICAddr *ic_addr, int is_identifier){
//   int num;
//   char rhs[20];
//   char fmt_smt[25];
//   char ch_num[20];
//   char float_mem_var[20];
//   switch (type){
//     case NUM:
//       num = ic_addr->value.num;
//       if(is_identifier == 0){
//         sprintf(ch_num,"%d",num);
//       }
//       else{
//         sprintf(ch_num, "[%s]", "Identifier name"); // ------------->to be done
//       }      
//       instrTwoOperand(instr_list,"mov","rdi","print_fmt_int");
//       instrTwoOperand(instr_list, "mov", "rsi", ch_num);
//       instrTwoOperand(instr_list, "mov", "rax", "0");
//       instrOneOperand(instr_list,"call","printf");
//       break;
//     case RNUM:
//       instrTwoOperand(instr_list, "mov", "rdi", "print_fmt_float");
//       if(is_identifier == 0){
//         cgLoadRealConstIntoTmp(instr_list, "dword", tmp_float_var1, ic_addr);
//         cgRealOneOp(instr_list,"fld","dword",tmp_float_var1);
//         cgRealOneOp(instr_list, "fstp", "qword", tmp_float_var1);
//         strcpy(float_mem_var,tmp_float_var1);
//       }
//       else{
//         strcpy(float_mem_var, "Identifier name");
//       }  
//       sprintf(rhs,"qword[%s]",float_mem_var);
//       instrTwoOperand(instr_list,"movq","xmm0",rhs);
//       instrTwoOperand(instr_list, "mov", "rax", "1");
//       instrOneOperand(instr_list, "call", "printf");
//       break;
//     case BOOLEAN_:
//       if (ic_addr->value.boolean == true)
//         strcpy(fmt_smt, "print_fmt_bool_true");
//       else
//         strcpy(fmt_smt, "print_fmt_bool_false");
//       instrTwoOperand(instr_list, "mov", "rdi", fmt_smt);
//       instrTwoOperand(instr_list, "mov", "rax", "0");
//       instrOneOperand(instr_list, "call", "printf");
//       break;
//     default:
//       break;
//   }
// }


// void loadConstReg(char *instr_list, char *reg1, int num) {
//   char asm_instr[30] = "";
//   char num_string[50];

//   // itoa(num,num_string, 10);
//   sprintf(num_string, "%d", num);

//   strcat(asm_instr, "\tmov");
//   strcat(asm_instr, "\t");
  
//   strcat(asm_instr, reg1);
//   strcat(asm_instr, " , ");
//   strcat(asm_instr, num_string);
//   strcat(asm_instr,"\n");

//   strcat(instr_list, asm_instr);
// }

// void insertVarFloat(char *tmp_var, char *name, char *type, float value) {
//   sprintf(tmp_var, "\t%s:\t%s\t%f\n", name, type, value);
// }


// void insertVarInt(char *tmp_var, char *name, char *type, int value) {
//   sprintf(tmp_var, "\t%s:\t%s\t%d\n", name, type, value);
// }


// void insert_fmt(char *tmp_var, char *name, char *per_val) {
//   sprintf(tmp_var, "\t%s:\tdb\t\"Output: %s\", 10, 0\n", name, per_val);
// }
