/**
 * Intermediate code generation header file.
 */
#ifndef ICODEGEN_H
#define ICODEGEN_H

#include "symtab.h"

enum TACOperationType {
  NEG,
  ADD,
  SUB,
  MUL,
  DIV,
  ASN,
  ADDR,
  LCOUNT,
  SCOUNT,
  GOTO,
  BLT,
  BIF,
  BNIF,
  PARM,
  CALL,
  RET
};

struct TACInstruction {
  enum TACOperationType op;
  int a1;
  int a2;
  int a3;
};

struct TACList {
  struct TACInstruction *head;
  struct TACInstruction *tail;
  int count;
};

/*
 * Operations for Three Address Code Data types.
 * All TAC functions start with TAC_ for clarity.
 */
struct TACInstruction *TAC_new_neg(int a1, int a2);
struct TACInstruction *TAC_new_add(int a1, int a2, int a3);
struct TACInstruction *TAC_new_sub(int a1, int a2, int a3);
struct TACInstruction *TAC_new_mul(int a1, int a2, int a3);
struct TACInstruction *TAC_new_div(int a1, int a2, int a3);
struct TACInstruction *TAC_new_asn(int a1, int a2);
struct TACInstruction *TAC_new_addr(int a1, int a2);
struct TACInstruction *TAC_new_lcount(int a1, int a2);
struct TACInstruction *TAC_new_scount(int a1, int a2);
struct TACInstruction *TAC_new_goto(int a1, int a2);
struct TACInstruction *TAC_new_blt(int a1, int a2);
struct TACInstruction *TAC_new_bif(int a1, int a2);
struct TACInstruction *TAC_new_bnif(int a1, int a2);
struct TACInstruction *TAC_new_param(int a1, int a2);
struct TACInstruction *TAC_new_call(int a1, int a2, int a3);
struct TACInstruction *TAC_new_ret(int a1, int a2);


icode_new_temp(Type *type);
icode_new_label();
icode_new_instr(enum TACOperationType op,)
int ic_calc_mem_address(Sytab *symtab);

#endif