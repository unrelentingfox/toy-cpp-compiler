/**
 * Intermediate code generation source file.
 */
#include "../header/icodegen.h"
#include "../header/logger.h"
#include "type.h"

/**
 * @brief      Create a new TACInstruction.
 *
 * @param[in]  op    The TACOperationType
 * @param      a1    First MemoryAddress
 * @param      a2    Second MemoryAddress
 * @param      a3    Third MemoryAddress
 *
 * @return     A pointer to the newly created TACInstruction.
 */
struct TACInstruction *TAC_new_instr(enum TACOperationType op, struct MemoryAddress *a1, struct MemoryAddress *a2, struct MemoryAddress *a3) {
  LOG_ASSERT(a1);
  LOG_ASSERT(a2);
  LOG_ASSERT(a3);
  struct TACInstruction *instr = malloc(sizeof(struct TACInstruction));
  if (instr == NULL) {
    log_error(INTERNAL_ERROR, "Out of memory while generating Three Address Instruction.\n");
    exit(INTERNAL_ERROR);
  }
  instr->op = op;
  instr->a1 = a1;
  instr->a2 = a2;
  instr->a3 = a3;
  instr->next = NULL;
  return instr;
}

/**
 * @brief      Create a new TACList.
 *
 * @param      instr  The TACInstruction that the list contains.
 *
 * @return     A pointer to the newly created TACList.
 */
struct TACList *TAC_new_list(struct TACInstruction *instr) {
  LOG_ASSERT(instr);
  struct TACList *list = malloc(sizeof(struct TACList));
  if (list == NULL) {
    log_error(INTERNAL_ERROR, "Out of memory while generating Three Address List.\n");
    exit(INTERNAL_ERROR);
  }
  list->head = instr;
  list->tail = instr;
  list->count = 1;
  return list;
}

/**
 * @brief      Concatinate TACList a and TACList b the result is stored in TACList a.
 *
 * @param      a     TACList a the destination list
 * @param      b     TACList b the list to append
 */
void TAC_concat(struct TACList *a, struct TACList *b) {
  LOG_ASSERT(a);
  LOG_ASSERT(b);
  a->tail->next = b->head;
  a->tail = b->tail;
  a->count += b->count;
}

/**
 * @brief      Append a TACInstruction onto the tail of a TACList.
 *
 * @param      list   The TACList
 * @param      instr  The TACInstruction
 */
void TAC_append(struct TACList *list, struct TACInstruction *instr) {
  LOG_ASSERT(list);
  LOG_ASSERT(instr);
  list->tail->next = instr;
  list->tail = instr;
  list->count++;
}

/**
 * @brief      Prepend a TACInstruction to the head of a TACList.
 *
 * @param      list   The TACList
 * @param      instr  The TACInstruction
 */
void TAC_prepend(struct TACList *list, struct TACInstruction *instr) {
  LOG_ASSERT(list);
  LOG_ASSERT(instr);
  instr->next = list->head;
  list->head = instr;
  list->count++;
}
