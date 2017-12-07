/**
 * Intermediate code generation header file.
 */
#ifndef ICODEGEN_H
#define ICODEGEN_H

#include "symtab.h"

enum MemoryRegion {
  GLOBAL_R = 2001,
  LOCAL_R = 2002,
  PARAM_R = 2003,
  CLASS_R = 2004,
  LABEL_R = 2005,
  CONST_R = 2006,
};

enum TACOperationType {
  /* Opcodes, per lecture notes */
  ADD_O   = 3001,
  SUB_O   = 3002,
  MUL_O   = 3003,
  DIV_O   = 3004,
  NEG_O   = 3005,
  ASN_O   = 3006,
  ADDR_O  = 3007,
  LCONT_O = 3008,
  SCONT_O = 3009,
  GOTO_O  = 3010,
  BLT_O   = 3011,
  BLE_O   = 3012,
  BGT_O   = 3013,
  BGE_O   = 3014,
  BEQ_O   = 3015,
  BNE_O   = 3016,
  BIF_O   = 3017,
  BNIF_O  = 3018,
  PARM_O  = 3019,
  CALL_O  = 3020,
  RET_O   = 3021,
  /* declarations/pseudo instructions */
  GLOB_D  = 3051,
  PROC_D  = 3052,
  LOCAL_D = 3053,
  LABEL_D = 3054,
  END_D   = 3055
};

typedef struct MemoryAddress {
  enum MemoryRegion region;
  int offset;
} Address;

struct TACInstruction {
  enum TACOperationType op;
  struct MemoryAddress a1;
  struct MemoryAddress a2;
  struct MemoryAddress a3;
  TACInstruction *next;
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
struct TACInstruction *TAC_new_instr(enum TACOperationType op, struct MemoryAddress *a1, struct MemoryAddress *a2, struct MemoryAddress *a3);
struct TACList *TAC_new_list(struct TACInstruction *instr);

void TAC_concat_list(struct TACList *a, struct TACList *b);
void TAC_append_list(struct TACList *list, struct TACInstruction *instr);
void TAC_prepend_list(struct TACList *list, struct TACInstruction *instr);

#endif