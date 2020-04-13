#include "codeGen.h"

void cg_ADD_SUB(ICInstr *ic_instr)
{
  char instr[100] = ""; //final instruction -temporary max length as 100

  char addr1[30];
  char store_reg1[50] = "\nMOV\tEAX , ";
  char addr2[30];
  char store_reg2[50] = "\nMOV\tEBX , ";
  // for addr1
  switch ((ic_instr->addr1).type)
  {
  case NUM:
    sprintf(addr1, "%d", (ic_instr->addr1).value.num);
    break;
  case RNUM:
    sprintf(addr1, "%f", (ic_instr->addr1).value.rnum);
    break;
  case BOOLEAN_:
    if ((ic_instr->addr1).value.boolean == true)
      sprintf(addr1, "True");
    else
      sprintf(addr1, "False");
    break;
  case IDENTIFIER:
    sprintf(addr1, "mem(%s)", (char *)(ic_instr->addr1).value.symbol);
    break;
  default:
    sprintf(addr1, "NULL");
    break;
  }
  // for addr2
  switch ((ic_instr->addr2).type)
  {
  case NUM:
    sprintf(addr2, "%d", (ic_instr->addr2).value.num);
    break;
  case RNUM:
    sprintf(addr2, "%f", (ic_instr->addr2).value.rnum);
    break;
  case BOOLEAN_:
    if ((ic_instr->addr2).value.boolean == true)
      sprintf(addr2, "True");
    else
      sprintf(addr2, "False");
    break;
  case IDENTIFIER:
    sprintf(addr2, "mem(%s)", (char *)(ic_instr->addr2).value.symbol);
    break;
  default:
    sprintf(addr2, "NULL");
    break;
  }
  // store instr
  strcat(store_reg1, addr1);
  strcat(store_reg2, addr2);
  // into code
  strcat(instr, store_reg1);
  strcat(instr, store_reg2);

  switch (ic_instr->op)
  {
  case icADD:
    strcat(instr, "\nADD\t");
    break;
  case icSUB:
    strcat(instr, "\nSUB\t");
    break;
  default:
    strcat(instr, "BLAH\t");
  }
  // operation
  strcat(instr, "EAX , EBX");
  // next is addr3- move the result in EAX to mem
  strcat(instr, "\nMOV\tEAX, ");
  strcat(instr, "mem(");
  strcat(instr, (char *)(ic_instr->addr3).value.symbol);
  strcat(instr, ")\t");

  // print out result
  printf("%s", instr);
}

void cgSTORE(ICInstr *ic_instr){ //---> not fully working- indexed elements of arrays need to be added

  char instr[100] = "\nMOV\t"; //final instruction -temporary max length as 100

  char addr3[30];
  fflush(stdout);
  sprintf(addr3, "mem(%s)", (char *)(ic_instr->addr3).value.symbol);
  char addr1[30];
  sprintf(addr1, "mem(%s)", (char *)(ic_instr->addr1).value.symbol);
  strcat(instr, addr3);
  strcat(instr, " , ");
  strcat(instr, addr1);

  // print out result
  printf("%s", instr);
}

// void cgINC(ICInstr *ic_instr) {}
// void cgDEC(ICInstr *ic_instr) {}
// void cgAND(ICInstr *ic_instr) {}
// void cgOR(ICInstr *ic_instr) {}
// void cgEQ(ICInstr *ic_instr) {}
// void cgNE(ICInstr *ic_instr) {}
// void cgLT(ICInstr *ic_instr) {}
// void cgGT(ICInstr *ic_instr) {}
// void cgLE(ICInstr *ic_instr) {}
// void cgGE(ICInstr *ic_instr) {}
// void cgPLUS(ICInstr *ic_instr) {}
// void cgMINUS(ICInstr *ic_instr) {}

// void cgJUMP(ICInstr *ic_instr) {}
// void cgTJUMP(ICInstr *ic_instr) {}
// void cgFJUMP(ICInstr *ic_instr) {}