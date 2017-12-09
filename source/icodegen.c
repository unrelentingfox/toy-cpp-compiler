/**
 * Intermediate code generation source file.
 */
#include "../header/icodegen.h"
#include "../header/logger.h"
#include "../header/type.h"
#include "../header/nonterm.h"

struct TACInstruction *TAC_new_instr(enum TACOperationType op, struct MemoryAddress *a1, struct MemoryAddress *a2, struct MemoryAddress *a3);
struct TACList *TAC_new_list(struct TACInstruction *instr);

void TAC_concat_list(struct TACList *a, struct TACList *b);
void TAC_append_list(struct TACList *list, struct TACInstruction *instr);
void TAC_prepend_list(struct TACList *list, struct TACInstruction *instr);

void TAC_set_symtab_addresses(Symtab *symtab, enum MemoryRegion region);


struct MemoryAddress *TAC_new_label();
void TAC_generate_first(TreeNode *treenode);
void TAC_generate_follow(TreeNode *treenode);

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


void TAC_intermediate_code_generation(TreeNode *treenode, Symtab *symtab) {
  TAC_generate_first(treenode);
  TAC_generate_follow(treenode);
  TAC_set_symtab_addresses(symtab, GLOBAL_R);
}

struct MemoryAddress *TAC_new_label() {
  return mem_new_address(LABEL_R, 1);
}

void TAC_generate_first(TreeNode *treenode) {
  if (!treenode) {
    return;
  }
  for (int i = 0; i < treenode->cnum; i++) {
    TAC_generate_first(treenode->children[i]);
  }
  switch (treenode->label) {
    case labeled_statement:
      treenode->first_l = TAC_new_label();
      break;
    case expression_statement:
      treenode->first_l = TAC_new_label();
      break;
    case compound_statement:
      treenode->first_l = TAC_new_label();
      break;
    case selection_statement:
      treenode->first_l = TAC_new_label();
      break;
    case iteration_statement:
      treenode->first_l = TAC_new_label();
      break;
    case jump_statement:
      treenode->first_l = TAC_new_label();
      break;
    case declaration_statement:
      treenode->first_l = TAC_new_label();
      break;
    default:
      break;
  }
}

void TAC_generate_follow(TreeNode *treenode) {
  if (!treenode) {
    return;
  }
  switch (treenode->label) {
    case statement_seq-2:
      treenode->children[0]->follow_l = treenode->children[1]->first_l;
      treenode->children[1]->follow_l = treenode->follow_l;
      break;
    case compound_statement:
      if (treenode->children[1] != NULL)
        treenode->children[1]->follow_l = treenode->follow_l;
      break;
    case selection_statement:

      break;
    case function_definition:
      treenode->children[2]->follow_l = TAC_new_label();
      /* .code must have this label and a return at the end! */
      break;
    default:
      break;
  }
  for (int i = 0; i < treenode->cnum; i++) {
    TAC_generate_follow(treenode->children[i]);
  }
}

void TAC_generate_true_false(TreeNode *treenode) {
  if (!treenode) {
    return;
  }
  switch (treenode->label) {
    case selection_statement-1:
      break;
    case selection_statement-2:
      break;
    case selection_statement-3:
      break;
    case iteration_statement-1:
      break;
    case iteration_statement-2:
      break;
    case iteration_statement-3:
      break;
    default:
      break;
  }
  for (int i = 0; i < treenode->cnum; i++) {
    TAC_generate_true_false(treenode->children[i]);
  }
}

void TAC_set_symtab_addresses(Symtab *symtab, enum MemoryRegion region) {
  SymtabNode *node;
  for (int i = 0; i < SYMTAB_SIZE; i++) { // traverse the symtab
    node = symtab->buckets[i];
    while (node != NULL) { // traverse the buckets
      LOG_ASSERT(node->type);
      node->address = mem_new_address(region, symtab_get_size_type(node->type));
      switch (node->type->basetype) { // set address for class or function symtabs
        case CLASS_T: {
          Symtab *classtab = node->type->info.class.public;
          if (classtab)
            TAC_set_symtab_addresses(classtab, LOCAL_R);
        }
        break;
        case FUNCTION_T: {
          Symtab *functab = node->type->info.function.symtab;
          if (functab)
            TAC_set_symtab_addresses(functab, LOCAL_R);
        }
        break;
        default:
          break;
      }
      node = node->next;
    }
  }
}

struct TACList *TAC_generate_code(TreeNode *treenode, Symtab *symtab) {
  if (!treenode) {
    return NULL;
  }
  switch (treenode->label) {

  }
}