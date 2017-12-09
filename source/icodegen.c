/**
 * Intermediate code generation source file.
 */
#include "../header/icodegen.h"
#include "../header/logger.h"
#include "../header/type.h"
#include "../header/nonterm.h"

extern Type *sem_typecheck(TreeNode *treenode, Symtab *symtab);

struct TACInstruction *TAC_new_instr(enum TACOperationType op, struct MemoryAddress *a1, struct MemoryAddress *a2, struct MemoryAddress *a3);
struct TACList *TAC_new_list(struct TACInstruction *instr);

struct TACList *TAC_concat_list(struct TACList *a, struct TACList *b);
void TAC_append_list(struct TACList *list, struct TACInstruction *instr);
void TAC_prepend_list(struct TACList *list, struct TACInstruction *instr);

void TAC_set_symtab_addresses(Symtab *symtab, enum MemoryRegion region);


struct MemoryAddress *TAC_new_label();
struct MemoryAddress *TAC_new_int_const(int value);
struct MemoryAddress *TAC_new_float_const(double value);
struct MemoryAddress *TAC_new_string_const(char *value);
struct MemoryAddress *TAC_new_const(union ConstantValue value, enum ConstantType type);
void TAC_generate_first(TreeNode *treenode);
void TAC_generate_follow(TreeNode *treenode);
void TAC_set_symtab_addresses(Symtab *symtab, enum MemoryRegion region);
void TAC_generate_code(TreeNode *treenode, Symtab *symtab);
void TAC_print_code(struct TACInstruction *instr);
char *TAC_op_to_str(enum TACOperationType op);

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
struct TACList *TAC_concat_list(struct TACList *a, struct TACList *b) {
  if (a && b) {
    a->tail->next = b->head;
    a->tail = b->tail;
    a->count += b->count;
  } else if (b) {
    a = b;
  }
  return a;
}

/**
 * @brief      Append a TACInstruction onto the tail of a TACList.
 *
 * @param      list   The TACList
 * @param      instr  The TACInstruction
 */
void TAC_append_list(struct TACList *list, struct TACInstruction *instr) {
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
void TAC_prepend_list(struct TACList *list, struct TACInstruction *instr) {
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
  TAC_generate_code(treenode, symtab);
  LOG_ASSERT(treenode->code);
  TAC_print_code(treenode->code->head);
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

struct MemoryAddress *TAC_new_label() {
  return mem_new_address(LABEL_R, 1);
}

struct MemoryAddress *TAC_new_int_const(int value) {
  union ConstantValue constval;
  constval.ival = value;
  return TAC_new_const(constval, IVAL);
}

struct MemoryAddress *TAC_new_float_const(double value) {
  union ConstantValue constval;
  constval.dval = value;
  return TAC_new_const(constval, DVAL);
}

struct MemoryAddress *TAC_new_string_const(char *value) {
  union ConstantValue constval;
  constval.sval = value;
  return TAC_new_const(constval, SVAL);
}

/* DO NOT CALL THIS FUNCTION, USE THE WRAPPER FUNCTIONS ABOVE */
struct MemoryAddress *TAC_new_const(union ConstantValue value, enum ConstantType type) {
  int size = 0;
  switch (type) {
    case IVAL:
    case DVAL:
      size = 8;
      break;
    case SVAL:
      // TODO: count the size of the string.
      size = 8;
      break;
  }
  MemoryAddress *address = mem_new_address(CONST_R, size);
  address->consttype = type;
  address->constval = value;
  return address;
}

struct MemoryAddress *TAC_new_temp(int size) {
  return mem_new_address(TEMP_R, size);
}

void TAC_generate_code(TreeNode *treenode, Symtab *symtab) {
  if (!treenode) {
    return;
  }
  switch (treenode->label) {
    case function_definition-1: {
      Type *functype = sem_typecheck(treenode->children[1], symtab);
      LOG_ASSERT(functype->info.function.symtab);
      TAC_generate_code(treenode->children[1], functype->info.function.symtab);
      TAC_generate_code(treenode->children[2], functype->info.function.symtab);
      treenode->code = TAC_concat_list(
                         treenode->children[1]->code,
                         treenode->children[2]->code
                       );
    }
    break;
    // case simple_declaration-1:
    //   break;
    // case additive_expression: // +
    //   break;
    // case multiplicative_expression: // *
    //   break;
    case assignment_expression: { // =
      TAC_generate_code(treenode->children[0], symtab);
      TAC_generate_code(treenode->children[2], symtab);
      LOG_ASSERT(treenode->children[0]->place);
      LOG_ASSERT(treenode->children[2]->place);
      treenode->code = TAC_new_list(TAC_new_instr(
                                      ASN_O,
                                      treenode->children[0]->place,
                                      treenode->children[2]->place,
                                      NULL
                                    ));
      treenode->code = TAC_concat_list(treenode->code, treenode->children[2]->code);
    }
    break;
    // case relational_expression: // > <
    //   break;
    // case equality_expression:  // ==
    //   break;
    // case iteration_statement:
    //   break;
    case jump_statement-3:
      LOG_ASSERT(treenode->children[1]);
      TAC_generate_code(treenode->children[1], symtab);
      LOG_ASSERT(treenode->children[1]->place);
      treenode->code = TAC_new_list(TAC_new_instr(RET_O, treenode->children[1]->place, NULL, NULL));
      break;
    /* LEAF NODES */
    case IDENTIFIER:
    case CLASS_NAME:
      LOG_ASSERT(treenode->token->text);
      SymtabNode *node = symtab_lookup(symtab, treenode->token->text);
      LOG_ASSERT(node->address);
      treenode->place = node->address;
      break;
    case INTEGER:
      //TODO: for some reason whenever 0 is an
      //integer constant ival doesn't exist?
      if (!treenode->token->ival)
        treenode->token->ival = 0;
      treenode->place = TAC_new_int_const(treenode->token->ival);
      break;
    case CHARACTER:
      LOG_ASSERT(treenode->token->sval);
      treenode->place = TAC_new_string_const(treenode->token->sval);
      break;
    case FLOATING:
      LOG_ASSERT(treenode->token->dval);
      treenode->place = TAC_new_float_const(treenode->token->dval);
      break;
    case STRING:
      LOG_ASSERT(treenode->token->sval);
      treenode->place = TAC_new_string_const(treenode->token->sval);
      break;
    case TRUE:
      treenode->place = TAC_new_int_const(1);
      break;
    case FALSE:
      treenode->place = TAC_new_int_const(0);
      break;
    default:
      for (int i = 0; i < treenode->cnum; i++) {
        TAC_generate_code(treenode->children[i], symtab);
        treenode->code = TAC_concat_list(treenode->code, treenode->children[i]->code);
      }
  }
}

void TAC_print_code(struct TACInstruction *instr) {
  if (!instr)
    return;
  else {
    printf("%s ", TAC_op_to_str(instr->op));
    printf("%s ", mem_address_to_str(instr->a1));
    printf("%s ", mem_address_to_str(instr->a2));
    printf("%s ", mem_address_to_str(instr->a3));
    printf("\n");
    TAC_print_code(instr->next);
  }
}

char *TAC_op_to_str(enum TACOperationType op) {
  switch (op) {
    case ADD_O:
      return strdup("ADD ");
    case SUB_O:
      return strdup("SUB ");
    case MUL_O:
      return strdup("MUL ");
    case DIV_O:
      return strdup("DIV ");
    case NEG_O:
      return strdup("NEG ");
    case ASN_O:
      return strdup("ASN ");
    case ADDR_O:
      return strdup("ADDR ");
    case LCONT_O:
      return strdup("LCONT ");
    case SCONT_O:
      return strdup("SCONT ");
    case GOTO_O:
      return strdup("GOTO ");
    case BLT_O:
      return strdup("BLT ");
    case BLE_O:
      return strdup("BLE ");
    case BGT_O:
      return strdup("BGT ");
    case BGE_O:
      return strdup("BGE ");
    case BEQ_O:
      return strdup("BEQ ");
    case BNE_O:
      return strdup("BNE ");
    case BIF_O:
      return strdup("BIF ");
    case BNIF_O:
      return strdup("BNIF ");
    case PARM_O:
      return strdup("PARM ");
    case CALL_O:
      return strdup("CALL ");
    case RET_O:
      return strdup("RET ");
    case GLOB_D:
      return strdup("GLOB ");
    case PROC_D:
      return strdup("PROC ");
    case LOCAL_D:
      return strdup("LOCAL ");
    case LABEL_D:
      return strdup("LABEL ");
    case END_D:
      return strdup("END ");
    default:
      return strdup("(INVALID)");
  }
}

