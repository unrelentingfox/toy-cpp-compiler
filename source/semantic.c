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
  sem_global = symtab_new_table(NULL, NULL);
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
  LOG_ASSERT(treenode);
  switch (treenode->label) {
    case statement_seq-1:
    case declaration_seq-1:
      sem_populate(treenode->children[0]);
      break;
    case statement_seq-2:
    case declaration_seq-2:
      sem_populate(treenode->children[0]);
      sem_populate(treenode->children[1]);
      break;
    case function_definition-1:
      sem_populate_function_definition(treenode, NULL, sem_current);
      break;
    case simple_declaration-1: { // class specifier
      sem_populate_declarators(treenode->children[1],
                               sem_get_type_from_token(treenode->children[0]));
    }
    break;
    case simple_declaration-2: { // normal variable declaration
      sem_populate_class_definition(treenode->children[0], NULL);
    }
    break;
      // case IDENTIFIER: {
      //   if (!symtab_lookup(sem_current, treenode->token->text))
      //     log_sem_error(treenode->token->filename,
      //                   treenode->token->lineno,
      //                   "symbol has not been declared in this scope",
      //                   treenode->token->text);
      // }
    break;
    default: {
      int i;
      for (i = 0; i < treenode->cnum; i++)
        sem_populate(treenode->children[i]);
    }
  }
}

void sem_populate_class_definition(TreeNode *treenode, Type *type) {
  LOG_ASSERT(treenode);
  switch (treenode->label) {
    case class_specifier:
      sem_populate_class_definition(treenode->children[0], type);
      sem_populate_class_definition(treenode->children[2], type);
      sem_current = sem_current->parent;
      break;
    case class_head: {
      /* insert class into symtab*/
      SymtabNode *tempnode = symtab_lookup(sem_current, treenode->children[1]->token->text);
      if (tempnode) {
        //printf("NODE EXISTS!\n");
        if (tempnode->type->basetype != CLASS_T) {
          /* if the symbol was defined as a different type, error */
          log_sem_error(treenode->children[1]->token->filename,
                        treenode->children[1]->token->lineno,
                        "symbol was already declared",
                        treenode->children[1]->token->text);
          return;
        } else if (tempnode->type->info.class.status == CLASS_DEFINED) {
          /* if it was already defined, error*/
          log_sem_error(treenode->children[1]->token->filename,
                        treenode->children[1]->token->lineno,
                        "class was already defined",
                        treenode->children[1]->token->text);
          return;
        } else { /* if it was declared with no errors, set the type to that function */
          type = tempnode->type;
        }
      } else {
        /* class hasn't been declared so create a new one */
        type = type_new_class(treenode->children[1]->token->text);
        // type->info.class.name = strdup(treenode->children[1]->token->text);
        symtab_insert(sem_current, type->info.class.name, type);
      }
      /* set sem_current to class scope to add all members */
      type->info.class.public = symtab_new_table(sem_current, type);
      sem_current = type->info.class.public;
    }
    break;
    case member_specification-1: {
      sem_populate_class_definition(treenode->children[0], type);
      if (treenode->children[1])
        sem_populate_class_definition(treenode->children[1], type);
    }
    break;
    case member_declaration: {
      if (treenode->children[0]->label == class_specifier)
        sem_populate_class_definition(treenode->children[0], type);
      else if (treenode->children[0]->label == function_definition
               || treenode->children[0]->label == function_definition - 1)
        sem_populate_function_definition(treenode->children[0], type, sem_current);
      else if (treenode->children[1]->label == direct_declarator)
        sem_populate_declarators(treenode->children[1],
                                 sem_get_type_from_token(treenode->children[0]));
      else if (treenode->children[1]->label == IDENTIFIER) {
        type = sem_get_type_from_token(treenode->children[0]);
        if (symtab_insert(sem_current, treenode->children[1]->token->text, type) == SYM_REDECLARED)
          log_sem_error(treenode->children[1]->token->filename,
                        treenode->children[1]->token->lineno,
                        "symbol was already declared",
                        treenode->children[1]->token->text);
      }
    }
    break;
    case member_specification-2: { // TODO: support private variables.
      LOG_ASSERT(treenode->children[2]);
      sem_populate_class_definition(treenode->children[2], type);
    }
    break;
  }
}


/**
 * @brief      Specialized tree traversal used for populating the symboltable
 *             from function definitions.
 *
 * @param      treenode  The treenode
 * @param      type      The type
 */
Symtab *sem_populate_function_definition(TreeNode *treenode, Type *type, Symtab *symtab) {
  //classoffset used to allow the use of the same code for all the direct_declarators since class functios have more children.
  int classoffset = 0;
  LOG_ASSERT(treenode);
  switch (treenode->label) {
    case function_definition:
      type = type_get_basetype(VOID_T);
      symtab = sem_populate_function_definition(treenode->children[0], type, symtab);
      sem_populate_function_definition(treenode->children[1], type, symtab);
      return symtab;
      break;
    case function_definition-1:
      type = sem_get_type_from_token(treenode->children[0]);
      symtab = sem_populate_function_definition(treenode->children[1], type, symtab);
      sem_populate_function_definition(treenode->children[2], type, symtab);
      return symtab;
      break;
    // direct declarator 1 through 4 use the same code with slight variations.
    case direct_declarator-4: /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
      // this is a constructor so it returns void
      type = type_get_basetype(VOID_T);
    case direct_declarator-3: /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')'  */
      // since this is a class function we must change the scope to the class scope.
      classoffset = 2;
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[0]->token);
      LOG_ASSERT(treenode->children[0]->token->text);
      SymtabNode *class = symtab_lookup(symtab, treenode->children[0]->token->text);
      if (!class || !class->type || class->type->basetype != CLASS_T) {
        sem_error_from_token("class does not exist in this scope", treenode->children[0]->token);
        return symtab;
      } else {
        LOG_ASSERT(class->type->info.class.public);
        symtab = class->type->info.class.public;
      }
    case direct_declarator-1:   /* direct_declarator '(' parameter_declaration_clause ')' */
    case direct_declarator-2: { /* CLASS_NAME '(' parameter_declaration_clause ')'  */
      LOG_ASSERT(type);
      SymtabNode *tempnode = NULL;
      tempnode = symtab_lookup_local(symtab, treenode->children[0 + classoffset]->token->text);
      if (tempnode) { /* check if function has been declared */
        if (tempnode->type->basetype != FUNCTION_T) {
          /* if the symbol was defined as a different type, error */
          log_sem_error(treenode->children[0 + classoffset]->token->filename,
                        treenode->children[0 + classoffset]->token->lineno,
                        "symbol was already declared",
                        treenode->children[0 + classoffset]->token->text);
          return symtab;
        } else if (tempnode->type->info.function.status == FUNC_DEFINED) {
          /* if it was already defined, error*/
          log_sem_error(treenode->children[0 + classoffset]->token->filename,
                        treenode->children[0 + classoffset]->token->lineno,
                        "function was already defined",
                        treenode->children[0 + classoffset]->token->text);
          return symtab;
        } else if (type_compare(tempnode->type->info.function.returntype, type) == TYPE_NOT_EQUAL) {
          /* if it was declared, but the return types don't match, error */
          sem_type_error("return type does not match declaration",
                         treenode->children[0 + classoffset]->token,
                         tempnode->type->info.function.returntype,
                         type
                        );
          return symtab;
        } else { /* if it was declared with no errors, set the type to that function */
          type = tempnode->type;
        }
      } else {
        type = type_new_function(type);
        if (symtab_insert(symtab, treenode->children[0 + classoffset]->token->text, type) == SYM_REDECLARED) {
          log_sem_error(treenode->children[0 + classoffset]->token->filename,
                        treenode->children[0 + classoffset]->token->lineno,
                        "symbol was already declared",
                        treenode->children[0 + classoffset]->token->text);
        }
      }
      /* create function symbtab */
      type->info.function.symtab = symtab_new_table(symtab, type);
      /* add parameters to symtab */
      symtab = type->info.function.symtab;
      if (treenode->children[2 + classoffset]->label == parameter_declaration ||
          treenode->children[2 + classoffset]->label == parameter_declaration_list) {
        sem_populate_parameter_definition(treenode->children[2 + classoffset], type, 0);
        type->info.function.status = FUNC_DEFINED;
      }
    }

    return symtab;
    break;
    /* TODO: implement class method definitions */
    case compound_statement: {
      Symtab *prev = sem_current;
      sem_current = symtab;
      sem_populate(treenode->children[1]);
      sem_current = prev;
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
  //printf("PARAM_DELC\n");
  LOG_ASSERT(treenode);
  LOG_ASSERT(functype);
  if (!treenode || functype->basetype != FUNCTION_T)
    return;
  switch (treenode->label) {
    case parameter_declaration_list:
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      sem_populate_parameter_declaration(treenode->children[0], functype);
      sem_populate_parameter_declaration(treenode->children[2], functype);
      break;
    case parameter_declaration: {
      int nparams = functype->info.function.nparams;
      Type *type = sem_get_type_from_token(treenode->children[0]);
      functype->info.function.parameters[nparams] = type_new_parameter(type);
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
  //printf("PARAM_DEFI\n");
  LOG_ASSERT(treenode);
  if (functype->basetype != FUNCTION_T)
    return paramcount;
  switch (treenode->label) {
    case parameter_declaration_list:
      paramcount = sem_populate_parameter_definition(treenode->children[0], functype, paramcount);
      paramcount = sem_populate_parameter_definition(treenode->children[2], functype, paramcount);
      break;
    case parameter_declaration: {
      /* create the parameter type */
      Type *temptype = sem_get_type_from_token(treenode->children[0]);
      switch (functype->info.function.status) {
        case FUNC_NEW: {
          /* if the function is being defined and declared at the same time
          create the parameters and insert them into the symbol table. */
          if (symtab_insert(functype->info.function.symtab, treenode->children[1]->token->text, temptype) == SYM_REDECLARED) {
            log_sem_error(treenode->children[1]->token->filename,
                          treenode->children[1]->token->lineno,
                          "symbol was already declared",
                          treenode->children[1]->token->text);
          }
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
              if (symtab_insert(functype->info.function.symtab, treenode->children[1]->token->text, temptype) == SYM_REDECLARED) {
                log_sem_error(treenode->children[1]->token->filename,
                              treenode->children[1]->token->lineno,
                              "symbol was already declared",
                              treenode->children[1]->token->text);
              }
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
    paramcount++;
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
void sem_populate_declarators(TreeNode *treenode, Type *type) {
  LOG_ASSERT(treenode);
  switch (treenode->label) {
    case init_declarator_list: {
      sem_populate_declarators(treenode->children[0], type);
      sem_populate_declarators(treenode->children[2], type);
    }
    break;
    case init_declarator-1:
    case init_declarator-2: {
      sem_populate_declarators(treenode->children[0], type);
    }
    break;
    case IDENTIFIER: {
      if ((symtab_insert(sem_current, treenode->token->text, type)) == SYM_REDECLARED) {
        log_sem_error(treenode->token->filename,
                      treenode->token->lineno,
                      "symbol was already declared",
                      treenode->token->text);
      }
    }
    break;
    case declarator: { /* it is a pointer and a direct_declarator */
      sem_populate_declarators(treenode->children[1], type);
    }
    break;
    case direct_declarator-1:     /* direct_declarator '(' parameter_declaration_clause ')' */
    case direct_declarator-2: {   /* CLASS_NAME '(' parameter_declaration_clause ')' */
      Type *temp = type_new_function(type);
      temp->info.function.status = FUNC_DECLARED;
      if (treenode->children[2]->label == parameter_declaration ||
          treenode->children[2]->label == parameter_declaration_list)
        sem_populate_parameter_declaration(treenode->children[2], temp);
      type = temp;
      if (symtab_insert(sem_current, treenode->children[0]->token->text, type) == SYM_REDECLARED) {
        log_sem_error(treenode->children[0]->token->filename,
                      treenode->children[0]->token->lineno,
                      "symbol was already declared",
                      treenode->children[0]->token->text);
      }
    }
    break;
    case direct_declarator-5: { /* direct_declarator '[' constant_expression ']' */
      Type *temp = type_new_array(type, treenode->children[2]->token->ival);
      type = temp;
      symtab_insert(sem_current, treenode->children[0]->token->text, type);
    }
    break;
  }
}

Type *sem_typecheck(TreeNode *treenode, Symtab *symtab) {
  if (!treenode || !symtab) {
    return type_get_basetype(UNKNOWN_T);
  }
  switch (treenode->label) {
    case declaration_seq-1:
    case statement_seq-1: {
      LOG_ASSERT(treenode->children[0]);
      sem_typecheck(treenode->children[0], symtab);
    }
    break;
    case declaration_seq-2:
    case statement_seq-2: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      sem_typecheck(treenode->children[0], symtab);
      sem_typecheck(treenode->children[1], symtab);
    }
    break;
    case function_definition-1: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      LOG_ASSERT(treenode->children[1]->children[0]);
      LOG_ASSERT(treenode->children[1]->children[0]->token);
      LOG_ASSERT(treenode->children[2]);
      Type *functype = sem_typecheck(treenode->children[1], symtab);
      if (!functype || functype->basetype != FUNCTION_T) {
        return type_get_basetype(UNKNOWN_T);
      }
      Symtab *functionscope = sem_get_function_symtab(functype);
      LOG_ASSERT(functionscope);
      sem_typecheck(treenode->children[2], functionscope);
      return sem_typecheck(treenode->children[0], symtab);
    }
    break;
    case compound_statement: {
      LOG_ASSERT(treenode->children[1]);
      if (treenode->children[1]->label != '}')
        return sem_typecheck(treenode->children[1], symtab);
      else
        return type_get_basetype(UNKNOWN_T);
    }
    break;
    case jump_statement-2:   // return;
    case jump_statement-3: { // return expression;
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      Type *functype = symtab->type;
      if (!functype || functype->basetype != FUNCTION_T) {
        sem_error_from_token("return outside of function", sem_get_leaf(treenode->children[0]));
        return type_get_basetype(UNKNOWN_T);
      } else {
        Type *type1 = functype->info.function.returntype;
        Type *type2 = NULL;
        if (treenode->children[1]->label != ';')
          type2 = sem_typecheck(treenode->children[1], symtab);
        else
          type2 = type_get_basetype(VOID_T);
        if (type_compare(type1, type2) != TYPE_EQUAL) {
          sem_type_error("return does not match function type", sem_get_leaf(treenode->children[0]), type1, type2);
          return type_get_basetype(UNKNOWN_T);
        } else {
          return type2;
        }
      }
    }
    break;
    case expression_statement: {
      LOG_ASSERT(treenode->children[0]);
      return sem_typecheck(treenode->children[0], symtab);
    }
    break;
    case selection_statement-1: { // IF
      LOG_ASSERT(treenode->children[2]);
      return sem_typecheck(treenode->children[2], symtab);
    }
    break;
    case selection_statement-2: { // IF ELSE
      LOG_ASSERT(treenode->children[2]);
      LOG_ASSERT(treenode->children[4]);
      LOG_ASSERT(treenode->children[6]);
      sem_typecheck(treenode->children[2], symtab);
      sem_typecheck(treenode->children[4], symtab);
      sem_typecheck(treenode->children[6], symtab);
    }
    break;
    case selection_statement-3: { // SWITCH
      //TODO: Support switch statements.
      return type_get_basetype(UNKNOWN_T);
    }
    break;
    case logical_and_expression-2:
    case logical_or_expression-2: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      sem_typecheck(treenode->children[0], symtab);
      sem_typecheck(treenode->children[2], symtab);
      return type_get_basetype(INT_T);
    }
    break;
    case simple_declaration-1: {
      LOG_ASSERT(treenode->children[1]);
      return sem_typecheck(treenode->children[1], symtab);
    }
    break;
    case init_declarator_list: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      sem_typecheck(treenode->children[0], symtab);
      sem_typecheck(treenode->children[2], symtab);
    }
    break;
    case init_declarator-2: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      Type *type1 = sem_typecheck(treenode->children[0], symtab);
      if (type1->basetype == ARRAY_T) {
        type1 = type1->info.array.type;
      }
      if (treenode->cnum > 1) {
        Type *type2 = sem_typecheck(treenode->children[1], symtab);
        if (type_compare(type1, type2) != TYPE_EQUAL) {
          sem_type_error("incorrect assignment type", sem_get_leaf(treenode->children[1]), type1, type2);
          return type_get_basetype(UNKNOWN_T);
        }
      }
      return type1;
    }
    break;
    case direct_declarator-4: /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
    case direct_declarator-3: /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')'  */
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[0]->token);
      LOG_ASSERT(treenode->children[0]->token->text);
      LOG_ASSERT(treenode->children[2]);
      LOG_ASSERT(treenode->children[2]->token);
      LOG_ASSERT(treenode->children[2]->token->text);
      SymtabNode *classnode = symtab_lookup(symtab, treenode->children[0]->token->text);
      if (!classnode || !classnode->type || !classnode->type->info.class.public) {
        sem_error_from_token("clsss was not declared in this scope", treenode->children[0]->token);
        return type_get_basetype(UNKNOWN_T);
      } else {
        symtab = classnode->type->info.class.public;
      }
      SymtabNode *funcnode = symtab_lookup(symtab, treenode->children[2]->token->text);
      if (!funcnode || !funcnode->type || funcnode->type->basetype != FUNCTION_T) {
        sem_error_from_token("function was not declared in this scope", treenode->children[2]->token);
        return type_get_basetype(UNKNOWN_T);
      } else {
        return funcnode->type;
      }
    case direct_declarator-1:   /* direct_declarator '(' parameter_declaration_clause ')' */
    case direct_declarator-2: { /* CLASS_NAME '(' parameter_declaration_clause ')'  */
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[0]->token);
      LOG_ASSERT(treenode->children[0]->token->text);
      SymtabNode *funcnode = symtab_lookup(symtab, treenode->children[0]->token->text);
      if (!funcnode || !funcnode->type || funcnode->type->basetype != FUNCTION_T) {
        sem_error_from_token("function was not declared in this scope", treenode->children[0]->token);
        return type_get_basetype(UNKNOWN_T);
      } else {
        return funcnode->type;
      }
    }
    break;
    case direct_declarator-5: {
      LOG_ASSERT(treenode->children[0]);
      return sem_typecheck(treenode->children[0], symtab);
    }
    break;
    case initializer: {
      LOG_ASSERT(treenode->children[1]);
      return sem_typecheck(treenode->children[1], symtab);
    }
    break;
    case initializer_clause: {
      return sem_typecheck(treenode->children[1], symtab);
    }
    break;
    case initializer_list: {
      Type *type = sem_typecheck(treenode->children[0], symtab);
      if (type_compare(type, sem_typecheck(treenode->children[2], symtab)) != TYPE_EQUAL) {
        return type_get_basetype(UNKNOWN_T);
      } else {
        return type;
      }
    }
    break;
    case postfix_expression-1: { // Array subscript
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[0]->token);
      LOG_ASSERT(treenode->children[0]->token->text);
      LOG_ASSERT(treenode->children[2]);
      Type *type1 = sem_typecheck(treenode->children[2], symtab);
      Type *type2 = type_get_basetype(INT_T);
      if (type_compare(type1, type2) != TYPE_EQUAL)
        sem_type_error("array subscript is not an integer", sem_get_leaf(treenode->children[2]), type1, type2);
      return symtab_lookup(symtab, treenode->children[0]->token->text)->type;
    }
    break;
    case postfix_expression-2: { // Function call without params
      LOG_ASSERT(treenode->children[0]);
    }
    break;
    case postfix_expression-3: { // Function call with params
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      // get the function type
      Type *type = sem_typecheck(treenode->children[0], symtab);
      if (type && type->basetype == FUNCTION_T) { // make sure the function exists
        int nparams = sem_typecheck_function_params(treenode->children[2], symtab, type, 0);
        if (nparams != -1 && nparams != type->info.function.nparams) // check if there were enough parameters
          sem_error_from_token("not enough parameters for function call", sem_get_leaf(treenode->children[0]));
        return type->info.function.returntype;
      } else {
        sem_error_from_token("function was not declared in this scope", sem_get_leaf(treenode->children[0]));
        return type_get_basetype(UNKNOWN_T);
      }
    }
    break;
    case postfix_expression-4: { // .::IDENTIFIER
      return type_get_basetype(UNKNOWN_T);
    }
    break;
    case postfix_expression-5: { // IDENTIFIER.IDENTIFIER
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      Type *type = sem_typecheck(treenode->children[0], symtab);
      if (!type || type->basetype != CLASS_INSTANCE_T) {
        return type_get_basetype(UNKNOWN_T);
      } else {
        LOG_ASSERT(type->info.classinstance.classtype);
        LOG_ASSERT(type->info.classinstance.classtype->info.class.public);
        return sem_typecheck(treenode->children[2], type->info.classinstance.classtype->info.class.public);
      }

    }
    break;
    case postfix_expression-6: { // ->::IDENTIFIER
      return type_get_basetype(UNKNOWN_T);
    }
    break;
    case postfix_expression-7: { // ->IDENTIFIER
      return type_get_basetype(UNKNOWN_T);
    }
    break;
    case postfix_expression-8:   // IDENTIFIER++
    case postfix_expression-9: { // IDENTIFIER--
      // make sure the type is INTEGER
      LOG_ASSERT(treenode->children[0]);
      Type *type1 = type_get_basetype(INT_T);
      Type *type2 = sem_typecheck(treenode->children[0], symtab);
      if (type_compare(type1, type2) != TYPE_EQUAL) {
        sem_type_error("++ and -- operators only work on integers in 120++", sem_get_leaf(treenode->children[0]), type1, type2);
      }
      return type2;
    }
    break;
    case unary_expression-6: { // !IDENTIFIER
      LOG_ASSERT(treenode->children[1]);
      sem_typecheck(treenode->children[1], symtab);
      return type_get_basetype(INT_T);
    }
    case additive_expression: // +
    case multiplicative_expression: // *
    case assignment_expression: // =
    case relational_expression: // > <
    case equality_expression: { // ==
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      Type *type1 = sem_typecheck(treenode->children[0], symtab);
      Type *type2 = sem_typecheck(treenode->children[2], symtab);
      if (type_compare(type1, type2) == TYPE_EQUAL) {
        return type1;
      } else {
        sem_type_error("infix expression type error", sem_get_leaf(treenode->children[0]), type1, type2);
        return type_get_basetype(UNKNOWN_T);
      }
    }
    break;
    case primary_expression: {
      LOG_ASSERT(treenode->children[1]);
      return sem_typecheck(treenode->children[1], symtab);
    }
    break;
    case IDENTIFIER:
    case CLASS_NAME: {
      LOG_ASSERT(treenode->token);
      LOG_ASSERT(treenode->token->text);
      SymtabNode *node = symtab_lookup(symtab, treenode->token->text);
      if (!node) {
        sem_error_from_token("symbol has not been declared in this scope", treenode->token);
        return type_get_basetype(UNKNOWN_T);
      } else {
        return node->type;
      }
    }
    break;
    case INTEGER: {
      return type_get_basetype(INT_T);
    }
    break;
    case CHARACTER: {
      return type_get_basetype(CHAR_T);
    }
    break;
    case FLOATING: {
      return type_get_basetype(FLOAT_T);
    }
    break;
    case STRING: {
      // TODO: implement strings
      return type_get_basetype(UNKNOWN_T);
    }
    break;
    case TRUE:
    case FALSE: {
      return type_get_basetype(INT_T);
    }
    break;
    default:
      return type_get_basetype(UNKNOWN_T);
  }
}

/**
 * @brief      Helper function to typecheck function parameters and number of
 *             parameters. Implemented as a post-order traversal.
 *
 * @param      treenode  A treenode that points to either an expression_list or
 *                       a single parameter
 * @param      functype  A type pointer that points to the function being called
 * @param[in]  param     The index of the current parameter being checked
 *                       (should start at 0 when this function is called)
 *
 * @return     Returns the number of parameters that the function call had. returns -1 if there was an error.
 */
int sem_typecheck_function_params(TreeNode *treenode, Symtab *symtab, Type *functype, int param) {
  if (!treenode || !functype) {
    return -1;
  }
  switch (treenode->label) {
    case expression_list: { // Iterate through the parameters
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      param = sem_typecheck_function_params(treenode->children[0], symtab, functype, param);
      if (param == -1) // don't do anything more if there was an error
        return -1;
      sem_typecheck_function_params(treenode->children[2], symtab, functype, param);
    }
    break;
    default: {
      if (param < functype->info.function.nparams) { // make sure there aren't too many parameters
        LOG_ASSERT(functype->info.function.parameters);
        LOG_ASSERT(functype->info.function.parameters[param]);
        LOG_ASSERT(functype->info.function.parameters[param]->type);
        Type *type1 = functype->info.function.parameters[param]->type;
        Type *type2 = sem_typecheck(treenode, symtab);  // get the type of the parameter
        if (type_compare(type1, type2) != TYPE_EQUAL) {
          sem_type_error("parameter does not match function definition", sem_get_leaf(treenode), type1, type2);
          return -1;
        }
      } else {
        sem_error_from_token("too many parameters for function call", sem_get_leaf(treenode));
        return -1;
      }
      return ++param;
    }
    break;
  }
}

/**
 * @brief      Attempt to get a type from a token. If the type looks like a
 *             class, lookup that class. If the class exists, return an instance
 *             of it. If the class does not exist, throw an error and return
 *             UNKNOWN_T.
 *
 * @param      treenode  The treenode
 *
 * @return     { description_of_the_return_value }
 */
Type *sem_get_type_from_token(TreeNode *treenode) {
  LOG_ASSERT(treenode);
  LOG_ASSERT(treenode->token);
  Type *type;
  // check if class exists and return an instance of that type if it does
  if (treenode->label == CLASS_NAME || treenode->label == IDENTIFIER) {
    type = symtab_lookup(sem_current, treenode->token->text)->type;
    if (type) {
      return type_new_class_instance(type);
    } else {
      log_sem_error(treenode->token->filename,
                    treenode->token->lineno,
                    "symbol has not been declared in this scope",
                    treenode->token->text);
      return type_get_basetype(UNKNOWN_T);
    }
  }
  // if it wasn't a class then get basetype.
  type = type_from_terminal(treenode->label);
  return type;
}

/**
 * @brief      Gets the token associated with the nearest leaf child. This
 *             helper function is for logging errors.
 *
 * @param      treenode  The treenode
 *
 * @return     { description_of_the_return_value }
 */
Token *sem_get_leaf(TreeNode *treenode) {
  if (treenode->token)
    return treenode->token;
  else {
    LOG_ASSERT(treenode->children[0]);
    return sem_get_leaf(treenode->children[0]);
  }
  return NULL;
}

Symtab *sem_get_function_symtab(Type *functype) {
  if (!functype || functype->basetype != FUNCTION_T)
    return NULL;
  if (functype->info.function.symtab) {
    return functype->info.function.symtab;
  }
}

void sem_error_from_token(char *message, Token *token) {
  log_sem_error(token->filename,
                token->lineno,
                message,
                token->text);
}

void sem_type_error(char *message, Token *token, Type *type1, Type *type2) {
  log_error(
    SEM_ERROR, "%s:%d:SEMANTIC ERROR: Near the token '%s' %s (types: %s != %s) \n",
    token->filename,
    token->lineno,
    token->text,
    message,
    type_to_string(type1),
    type_to_string(type2)
  );
}
