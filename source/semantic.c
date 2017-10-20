#include "../header/semantic.h"
#include "../header/nonterm.h"
#include "../header/logger.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// global scope vairables
Symtab *sem_global;
Symtab *sem_current; /* TODO: remove global current and just make it a parameter for recursive functions */

int sem_init_global() {
  //TODO: create a function to free symtab before init.
  sem_global = symtab_new_table(NULL);
  sem_current = sem_global;
}

/**
 * @brief      General top level tree traversal that looks for subtrees of
 *             interest and calls one of the other specialized tree traversals
 *             to do the work.
 *
 * @param      treenode  The treenode
 */
void sem_populate(TreeNode *treenode) {
  int i;
  if (treenode == NULL) {
    printf("tree empty!\n");
    return;
  }
  switch (treenode->label) {
    case declaration_seq-1:
      sem_populate(treenode->children[0]);
      break;
    case declaration_seq-2:
      sem_populate(treenode->children[0]);
      sem_populate(treenode->children[1]);
      break;
    case function_definition-1:
      sem_populate_function_definition(treenode, NULL);
      break;
    case simple_declaration:
      sem_populate_init_declarator(treenode->children[1],
                                   type_new(type_from_terminal(treenode->children[0]->label)));
      break;
  }

  // if (treenode->label < 0) {
  //   for (i = 0; i < treenode->cnum; i++)
  //     sem_populate(treenode->children[i]);
  // }
}

/**
 * @brief      Specialized tree traversal used for populating the symboltable
 *             from function definitions.
 *
 * @param      treenode  The treenode
 * @param      type      The type
 */
void sem_populate_function_definition(TreeNode *treenode, Type *type) {
  printf("POPULATE FUNCTION DEF\n");
  switch (treenode->label) {
    case function_definition-1:
      /* get the function return type and pass it into the recursive call */
      if (treenode->children[0]->label == IDENTIFIER) { /* class instance */
        type = sem_get_class_type(treenode->children[0]->token);
      }
      if (!type) {
        type = type_new(type_from_terminal(treenode->children[0]->label));
      }
      sem_populate_function_definition(treenode->children[1], type);
      sem_populate_function_definition(treenode->children[2], type);
      break;
    case direct_declarator-1: { /* direct_declarator '(' parameter_declaration_clause ')' */
      SymtabNode *tempnode = symtab_lookup(sem_current, treenode->children[0]->token->text);
      if (tempnode) { /* check if function has been declared */
        printf("NODE EXISTS!\n");
        if (tempnode->type->basetype != FUNCTION_T) {
          /* if the symbol was defined as a different type, error */
          log_sem_error(treenode->children[0]->token->filename,
                        treenode->children[0]->token->lineno,
                        "symbol was already declared",
                        treenode->children[0]->token->text);
          return;
        } else if (tempnode->type->info.function.status == FUNC_DEFINED) {
          /* if it was already defined, error*/
          log_sem_error(treenode->children[0]->token->filename,
                        treenode->children[0]->token->lineno,
                        "function was already defined",
                        treenode->children[0]->token->text);
          return;
        } else if (type_compare(tempnode->type->info.function.returntype, type) == TYPE_EQUAL) {
          /* if it was declared, but the return types don't match, error */
          log_sem_error(treenode->children[0]->token->filename,
                        treenode->children[0]->token->lineno,
                        "return type does not match declaration",
                        treenode->children[0]->token->text);
        } else { /* if it was declared with no errors, set the type to that function */
          type = tempnode->type;
        }
      } else { /* if not defined, create new function type and add to symtab */
        printf("CREATING FUNCTION!!!!\n");
        Type *temptype = type_new(FUNCTION_T);
        temptype->info.function.returntype = type;
        type = temptype;
        symtab_insert(sem_current, treenode->children[0]->token->text, type);
      }
      /* create function symbtab */
      type->info.function.symtab = symtab_new_table(sem_current);
      /* add parameters to symtab */
      if (treenode->children[2]->label == parameter_declaration ||
          treenode->children[2]->label == parameter_declaration_list) {
        sem_populate_parameter_definition(treenode->children[2], type, 0);
        type->info.function.status = FUNC_DEFINED;
      }
    }
    break;
//  case direct_declarator-2: /* CLASS_NAME '(' parameter_declaration_clause ')'  */
//  case direct_declarator-3: /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')'  */
//  case direct_declarator-4: /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
    /* TODO: implement class method definitions */
    case compound_statement: {
      if (type->info.function.symtab) {
        sem_current = type->info.function.symtab;
        // sem_populate(treenode->children[1]);
        sem_current = sem_current->parent;
      }
    }
    break;
  }
}

/**
 * @brief      Specialized tree traversal used to populate the parameters of a
 *             function type inside a function declaration
 *
 * @param      treenode  The treenode
 * @param      functype  The type of the parent function
 */
void sem_populate_parameter_declaration(TreeNode *treenode, Type *functype) {
  if (functype->basetype != FUNCTION_T) {
    return;
  }
  switch (treenode->label) {
    case parameter_declaration_list:
      sem_populate_parameter_declaration(treenode->children[0], functype);
      sem_populate_parameter_declaration(treenode->children[2], functype);
      break;
    case parameter_declaration: {
      functype->info.function.parameters[functype->info.function.nparams] = type_new_parameter(type_new(type_from_terminal(treenode->children[0]->label)));
      functype->info.function.nparams++;
    }
    break;
  }
}

/**
 * @brief      Specialized tree traversal used to populate and insert parameters
 *             into the symbol table for a function in a function definition.
 *
 * @param      treenode    The treenode
 * @param      functype    The type
 * @param[in]  paramcount  The paramcount
 *
 * @return     Returns paramcount in order to keep track of how many parameters
 *             we have seen so far.
 */
int sem_populate_parameter_definition(TreeNode *treenode, Type *functype, int paramcount) {
  if (functype->basetype != FUNCTION_T) {
    return paramcount;
  }
  switch (treenode->label) {
    case parameter_declaration_list:
      paramcount = sem_populate_parameter_definition(treenode->children[0], functype, paramcount);
      paramcount = sem_populate_parameter_definition(treenode->children[2], functype, paramcount);
      break;
    case parameter_declaration: {
      /* create the parameter type */
      Type *temptype;
      if (treenode->children[0]->label == IDENTIFIER) { /* class instance */
        temptype = sem_get_class_type(treenode->children[0]->token);
      }
      if (!temptype) {
        temptype = type_new(type_from_terminal(treenode->children[0]->label));
      }
      switch (functype->info.function.status) {
        case FUNC_NEW: {
          /* if the function is being defined and declared at the same time
          create the parameters and insert them into the symbol table. */
          symtab_insert(functype->info.function.symtab, treenode->children[1]->token->text, temptype);
          functype->info.function.parameters[functype->info.function.nparams] = type_new_parameter(temptype);
          functype->info.function.nparams++;
        }
        break;
        case FUNC_DECLARED: {
          /* if the function was already declared,
          check that the parameters are the right type
          and insert them into the symbol table */
          if (paramcount == functype->info.function.nparams) {
            log_sem_error(treenode->children[1]->token->filename,
                          treenode->children[1]->token->lineno,
                          "too many parameters, does not match declaration",
                          treenode->children[1]->token->text);
          } else if (functype->info.function.parameters[paramcount]) {
            if (type_compare(functype->info.function.parameters[paramcount]->type, temptype) == TYPE_EQUAL) {
              symtab_insert(functype->info.function.symtab, treenode->children[1]->token->text, temptype);
            } else {
              log_sem_error(treenode->children[1]->token->filename,
                            treenode->children[1]->token->lineno,
                            "parameter type does not match declaration",
                            treenode->children[1]->token->text);
            }
          }
        }
      }
    }
    break;
  }
  return paramcount;
}

/**
 * @brief      Specialized tree traversal for populating the symbol table from
 *             an init_declarator.
 *
 * @param      treenode  The treenode
 * @param      type      The type
 */
void sem_populate_init_declarator(TreeNode *treenode, Type *type) {
  switch (treenode->label) {
    case init_declarator_list:
      sem_populate_init_declarator(treenode->children[0], type);
      sem_populate_init_declarator(treenode->children[1], type);
      break;
    case init_declarator:
      sem_populate_init_declarator(treenode->children[0], type);
      break;
    case IDENTIFIER:
      if ((symtab_insert(sem_current, treenode->token->text, type)) == SYM_REDECLARED) {
        log_sem_error(treenode->token->filename,
                      treenode->token->lineno,
                      "symbol was already declared",
                      treenode->token->text);
      }
      break;
    case declarator: /* it is a pointer and a direct_declarator */
      sem_populate_init_declarator(treenode->children[1], type);
      break;
    case direct_declarator-1:   /* direct_declarator '(' parameter_declaration_clause ')' */
    case direct_declarator-2:   /* CLASS_NAME '(' parameter_declaration_clause ')' */
    case direct_declarator-3:   /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')' */
    case direct_declarator-4: { /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
      Type *temp = type_new(FUNCTION_T);
      temp->info.function.status = FUNC_DECLARED;
      temp->info.function.returntype = type;
      if (treenode->children[2]->label == parameter_declaration ||
          treenode->children[2]->label == parameter_declaration_list)
        sem_populate_parameter_declaration(treenode->children[2], temp);
      type = temp;
      symtab_insert(sem_current, treenode->children[0]->token->text, type);
    }
    break;
    case direct_declarator-5: { /* direct_declarator '[' constant_expression ']' */
      Type *temp = type_new(ARRAY_T);
      temp->info.array.type = type;
      temp->info.array.size = treenode->children[2]->token->ival;
      type = temp;
      symtab_insert(sem_current, treenode->children[0]->token->text, type);
    }
    break;
  }
}

/**
 * @brief      Helper function that checks if a class has been declared yet, if
 *             not, throws an error and returns NULL.
 *
 * @param      token  The token that contains the class name.
 *
 * @return     Either the class type or NULL.
 */
Type *sem_get_class_type(Token *token) {
  if (!token)
    return NULL;
  Type *newtype = symtab_lookup(sem_current, token->text)->type;
  if (!newtype) {
    log_sem_error(token->filename,
                  token->lineno,
                  "symbol has not been declared in this scope",
                  token->text);
  }
  return newtype;
}