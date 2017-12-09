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

MemoryAddress *TAC_generate_labels(TreeNode *treenode, MemoryAddress *label, Symtab *symtab) {
  if (!treenode || !symtab) {
    return
  }
  switch (treenode->label) {
  }
}

// TACList *TAC_create(TreeNode *treenode, Symtab *symtab) {
//   if (!treenode || !symtab) {
//     return
//   }
//   switch (treenode->label) {
//     case declaration_seq-1:
//     case statement_seq-1: {
//       LOG_ASSERT(treenode->children[0]);
//       return TAC_create(treenode->children[0], symtab);
//     }
//     break;
//     case declaration_seq-2:
//     case statement_seq-2: {
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[1]);
//       TAC_create(treenode->children[0], symtab);
//       TAC_create(treenode->children[1], symtab);
//     }
//     break;
//     case function_definition-1: {
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[1]);
//       LOG_ASSERT(treenode->children[1]->children[0]);
//       LOG_ASSERT(treenode->children[1]->children[0]->token);
//       LOG_ASSERT(treenode->children[2]);
//       Type *functype = TAC_create(treenode->children[1], symtab);
//       if (!functype || functype->basetype != FUNCTION_T) {
//         return type_get_basetype(UNKNOWN_T);
//       }
//       Symtab *functionscope = sem_get_function_symtab(functype);
//       LOG_ASSERT(functionscope);
//       TAC_create(treenode->children[2], functionscope);
//       return TAC_create(treenode->children[0], symtab);
//     }
//     break;
//     case compound_statement: {
//       LOG_ASSERT(treenode->children[1]);
//       if (treenode->children[1]->label != '}')
//         return TAC_create(treenode->children[1], symtab);
//       else
//         return type_get_basetype(UNKNOWN_T);
//     }
//     break;
//     case jump_statement-2:   // return;
//     case jump_statement-3: { // return expression;
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[1]);
//       Type *functype = symtab->type;
//       if (!functype || functype->basetype != FUNCTION_T) {
//         sem_error_from_token("return outside of function", sem_get_leaf(treenode->children[0]));
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         Type *type1 = functype->info.function.returntype;
//         Type *type2 = NULL;
//         if (treenode->children[1]->label != ';')
//           type2 = TAC_create(treenode->children[1], symtab);
//         else
//           type2 = type_get_basetype(VOID_T);
//         if (type_compare(type1, type2) != TYPE_EQUAL) {
//           sem_type_error("return does not match function type", sem_get_leaf(treenode->children[0]), type1, type2);
//           return type_get_basetype(UNKNOWN_T);
//         } else {
//           return type2;
//         }
//       }
//     }
//     break;
//     case expression_statement: {
//       LOG_ASSERT(treenode->children[0]);
//       return TAC_create(treenode->children[0], symtab);
//     }
//     break;
//     case selection_statement-1: { // IF
//       LOG_ASSERT(treenode->children[2]);
//       return TAC_create(treenode->children[2], symtab);
//     }
//     break;
//     case selection_statement-2: { // IF ELSE
//       LOG_ASSERT(treenode->children[2]);
//       LOG_ASSERT(treenode->children[4]);
//       LOG_ASSERT(treenode->children[6]);
//       TAC_create(treenode->children[2], symtab);
//       TAC_create(treenode->children[4], symtab);
//       TAC_create(treenode->children[6], symtab);
//     }
//     break;
//     case selection_statement-3: { // SWITCH
//       //TODO: Support switch statements.
//       return type_get_basetype(UNKNOWN_T);
//     }
//     break;
//     case logical_and_expression-2:
//     case logical_or_expression-2: {
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[2]);
//       TAC_create(treenode->children[0], symtab);
//       TAC_create(treenode->children[2], symtab);
//       return type_get_basetype(INT_T);
//     }
//     break;
//     case simple_declaration-1: {
//       LOG_ASSERT(treenode->children[1]);
//       return TAC_create(treenode->children[1], symtab);
//     }
//     break;
//     case init_declarator_list: {
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[2]);
//       TAC_create(treenode->children[0], symtab);
//       TAC_create(treenode->children[2], symtab);
//     }
//     break;
//     case init_declarator-2: {
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[1]);
//       Type *type1 = TAC_create(treenode->children[0], symtab);
//       if (type1->basetype == ARRAY_T) {
//         type1 = type1->info.array.type;
//       }
//       if (treenode->cnum > 1) {
//         Type *type2 = TAC_create(treenode->children[1], symtab);
//         if (type_compare(type1, type2) != TYPE_EQUAL) {
//           sem_type_error("incorrect assignment type", sem_get_leaf(treenode->children[1]), type1, type2);
//           return type_get_basetype(UNKNOWN_T);
//         }
//       }
//       return type1;
//     }
//     break;
//     case direct_declarator-4: /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
//     case direct_declarator-3: /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')'  */
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[0]->token);
//       LOG_ASSERT(treenode->children[0]->token->text);
//       LOG_ASSERT(treenode->children[2]);
//       LOG_ASSERT(treenode->children[2]->token);
//       LOG_ASSERT(treenode->children[2]->token->text);
//       SymtabNode *classnode = symtab_lookup(symtab, treenode->children[0]->token->text);
//       if (!classnode || !classnode->type || !classnode->type->info.class.public) {
//         sem_error_from_token("clsss was not declared in this scope", treenode->children[0]->token);
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         symtab = classnode->type->info.class.public;
//       }
//       SymtabNode *funcnode = symtab_lookup(symtab, treenode->children[2]->token->text);
//       if (!funcnode || !funcnode->type || funcnode->type->basetype != FUNCTION_T) {
//         sem_error_from_token("function was not declared in this scope", treenode->children[2]->token);
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         return funcnode->type;
//       }
//     case direct_declarator-1:   /* direct_declarator '(' parameter_declaration_clause ')' */
//     case direct_declarator-2: { /* CLASS_NAME '(' parameter_declaration_clause ')'  */
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[0]->token);
//       LOG_ASSERT(treenode->children[0]->token->text);
//       SymtabNode *funcnode = symtab_lookup(symtab, treenode->children[0]->token->text);
//       if (!funcnode || !funcnode->type || funcnode->type->basetype != FUNCTION_T) {
//         sem_error_from_token("function was not declared in this scope", treenode->children[0]->token);
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         return funcnode->type;
//       }
//     }
//     break;
//     case direct_declarator-5: {
//       LOG_ASSERT(treenode->children[0]);
//       return TAC_create(treenode->children[0], symtab);
//     }
//     break;
//     case initializer: {
//       LOG_ASSERT(treenode->children[1]);
//       return TAC_create(treenode->children[1], symtab);
//     }
//     break;
//     case initializer_clause: {
//       return TAC_create(treenode->children[1], symtab);
//     }
//     break;
//     case initializer_list: {
//       Type *type = TAC_create(treenode->children[0], symtab);
//       if (type_compare(type, TAC_create(treenode->children[2], symtab)) != TYPE_EQUAL) {
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         return type;
//       }
//     }
//     break;
//     case postfix_expression-1: { // Array subscript
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[0]->token);
//       LOG_ASSERT(treenode->children[0]->token->text);
//       LOG_ASSERT(treenode->children[2]);
//       Type *type1 = TAC_create(treenode->children[2], symtab);
//       Type *type2 = type_get_basetype(INT_T);
//       if (type_compare(type1, type2) != TYPE_EQUAL)
//         sem_type_error("array subscript is not an integer", sem_get_leaf(treenode->children[2]), type1, type2);
//       return symtab_lookup(symtab, treenode->children[0]->token->text)->type;
//     }
//     break;
//     case postfix_expression-2: { // Function call without params
//       LOG_ASSERT(treenode->children[0]);
//     }
//     break;
//     case postfix_expression-3: { // Function call with params
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[2]);
//       // get the function type
//       Type *type = TAC_create(treenode->children[0], symtab);
//       if (type && type->basetype == FUNCTION_T) { // make sure the function exists
//         int nparams = TAC_create_function_params(treenode->children[2], symtab, type, 0);
//         if (nparams != -1 && nparams != type->info.function.nparams) // check if there were enough parameters
//           sem_error_from_token("not enough parameters for function call", sem_get_leaf(treenode->children[0]));
//         return type->info.function.returntype;
//       } else {
//         sem_error_from_token("function was not declared in this scope", sem_get_leaf(treenode->children[0]));
//         return type_get_basetype(UNKNOWN_T);
//       }
//     }
//     break;
//     case postfix_expression-4: { // .::IDENTIFIER
//       return type_get_basetype(UNKNOWN_T);
//     }
//     break;
//     case postfix_expression-5: { // IDENTIFIER.IDENTIFIER
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[2]);
//       Type *type = TAC_create(treenode->children[0], symtab);
//       if (!type || type->basetype != CLASS_INSTANCE_T) {
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         LOG_ASSERT(type->info.classinstance.classtype);
//         LOG_ASSERT(type->info.classinstance.classtype->info.class.public);
//         return TAC_create(treenode->children[2], type->info.classinstance.classtype->info.class.public);
//       }

//     }
//     break;
//     case postfix_expression-6: { // ->::IDENTIFIER
//       return type_get_basetype(UNKNOWN_T);
//     }
//     break;
//     case postfix_expression-7: { // ->IDENTIFIER
//       return type_get_basetype(UNKNOWN_T);
//     }
//     break;
//     case postfix_expression-8:   // IDENTIFIER++
//     case postfix_expression-9: { // IDENTIFIER--
//       // make sure the type is INTEGER
//       LOG_ASSERT(treenode->children[0]);
//       Type *type1 = type_get_basetype(INT_T);
//       Type *type2 = TAC_create(treenode->children[0], symtab);
//       if (type_compare(type1, type2) != TYPE_EQUAL) {
//         sem_type_error("++ and -- operators only work on integers in 120++", sem_get_leaf(treenode->children[0]), type1, type2);
//       }
//       return type2;
//     }
//     break;
//     case unary_expression-6: { // !IDENTIFIER
//       LOG_ASSERT(treenode->children[1]);
//       TAC_create(treenode->children[1], symtab);
//       return type_get_basetype(INT_T);
//     }
//     case additive_expression: // +
//     case multiplicative_expression: // *
//     case assignment_expression: // =
//     case relational_expression: // > <
//     case equality_expression: { // ==
//       LOG_ASSERT(treenode->children[0]);
//       LOG_ASSERT(treenode->children[2]);
//       Type *type1 = TAC_create(treenode->children[0], symtab);
//       Type *type2 = TAC_create(treenode->children[2], symtab);
//       if (type_compare(type1, type2) == TYPE_EQUAL) {
//         return type1;
//       } else {
//         sem_type_error("infix expression type error", sem_get_leaf(treenode->children[0]), type1, type2);
//         return type_get_basetype(UNKNOWN_T);
//       }
//     }
//     break;
//     case primary_expression: {
//       LOG_ASSERT(treenode->children[1]);
//       return TAC_create(treenode->children[1], symtab);
//     }
//     break;
//     case IDENTIFIER:
//     case CLASS_NAME: {
//       LOG_ASSERT(treenode->token);
//       LOG_ASSERT(treenode->token->text);
//       SymtabNode *node = symtab_lookup(symtab, treenode->token->text);
//       if (!node) {
//         sem_error_from_token("symbol has not been declared in this scope", treenode->token);
//         return type_get_basetype(UNKNOWN_T);
//       } else {
//         return node->type;
//       }
//     }
//     break;
//     case INTEGER: {
//       return type_get_basetype(INT_T);
//     }
//     break;
//     case CHARACTER: {
//       return type_get_basetype(CHAR_T);
//     }
//     break;
//     case FLOATING: {
//       return type_get_basetype(FLOAT_T);
//     }
//     break;
//     case STRING: {
//       return type_new_pointer(type_get_basetype(CHAR_T));
//     }
//     break;
//     case TRUE:
//     case FALSE: {
//       return type_get_basetype(INT_T);
//     }
//     break;
//     default:
//       break;
//   }
// }