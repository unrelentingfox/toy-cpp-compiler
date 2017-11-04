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
      sem_populate_function_definition(treenode, NULL);
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
    case IDENTIFIER: {
      if (!symtab_lookup(sem_current, treenode->token->text))
        log_sem_error(treenode->token->filename,
                      treenode->token->lineno,
                      "symbol has not been declared in this scope",
                      treenode->token->text);
    }
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
      type->info.class.public = symtab_new_table(sem_current);
      sem_current = type->info.class.public;
    }
    break;
    case member_specification-1:
      sem_populate_class_definition(treenode->children[0], type);
      if (treenode->children[1])
        sem_populate_class_definition(treenode->children[1], type);
      break;
    case member_declaration: {
      if (treenode->children[0]->label == class_specifier)
        sem_populate_class_definition(treenode->children[0], type);
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
  }
}


/**
 * @brief      Specialized tree traversal used for populating the symboltable
 *             from function definitions.
 *
 * @param      treenode  The treenode
 * @param      type      The type
 */
void sem_populate_function_definition(TreeNode *treenode, Type *type) {
  //printf("POPULATE FUNCTION DEF\n");
  LOG_ASSERT(treenode);
  switch (treenode->label) {
    case function_definition-1:
      type = sem_get_type_from_token(treenode->children[0]);
      sem_populate_function_definition(treenode->children[1], type);
      sem_populate_function_definition(treenode->children[2], type);
      break;
    case direct_declarator-1: { /* direct_declarator '(' parameter_declaration_clause ')' */
      //printf("function: %s\n", treenode->children[0]->token->text);
      //printf("nodes(%d) text(%s)", sem_current->nNodes, treenode->children[0]->token->text);
      SymtabNode *tempnode = NULL;
      tempnode = symtab_lookup(sem_current, treenode->children[0]->token->text);
      //printf("HERE\n");
      if (tempnode) { /* check if function has been declared */
        //printf("NODE EXISTS!\n");
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
        } else if (type_compare(tempnode->type->info.function.returntype, type) == TYPE_NOT_EQUAL) {
          /* if it was declared, but the return types don't match, error */
          log_sem_error(treenode->children[0]->token->filename,
                        treenode->children[0]->token->lineno,
                        "return type does not match declaration",
                        treenode->children[0]->token->text);
        } else { /* if it was declared with no errors, set the type to that function */
          type = tempnode->type;
        }
      } else { /* if not defined, create new function type and add to symtab */
        //printf("CREATING FUNCTION!!!!\n");
        Type *temptype = type_new_function(type);
        type = temptype;
        if (symtab_insert(sem_current, treenode->children[0]->token->text, type) == SYM_REDECLARED) {
          log_sem_error(treenode->children[0]->token->filename,
                        treenode->children[0]->token->lineno,
                        "symbol was already declared",
                        treenode->children[0]->token->text);
        }
      }
      /* create function symbtab */
      type->info.function.symtab = symtab_new_table(sem_current);
      /* add parameters to symtab */
      sem_current = type->info.function.symtab;
      if (treenode->children[2]->label == parameter_declaration ||
          treenode->children[2]->label == parameter_declaration_list) {
        sem_populate_parameter_definition(treenode->children[2], type, 0);
        type->info.function.status = FUNC_DEFINED;
      }
      sem_current = sem_current->parent;
      /* set sem_current to function scope */
      sem_current = type->info.function.symtab;
    }
    break;
    // case direct_declarator-2: /* CLASS_NAME '(' parameter_declaration_clause ')'  */
    // case direct_declarator-3: /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')'  */
    // case direct_declarator-4:  CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')'
    //   /* check if the class exists */
    //   /* if it does, change scope to that class scope and handle function */
    //   /* if it does not, throw and error and ignore the function */
    //   //printf("CLASS FOUND BRUHHHHHHHHHHH\n");
    //   break;
    /* TODO: implement class method definitions */
    case compound_statement: {
      sem_populate(treenode->children[1]);
      sem_current = sem_current->parent;
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
  if (!treenode || functype->basetype != FUNCTION_T)
    return;
  switch (treenode->label) {
    case parameter_declaration_list:
      sem_populate_parameter_declaration(treenode->children[0], functype);
      sem_populate_parameter_declaration(treenode->children[2], functype);
      break;
    case parameter_declaration: {
      functype->info.function.parameters[functype->info.function.nparams]->type = sem_get_type_from_token(treenode->children[0]);
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
          if (symtab_insert(functype->info.function.symtab, treenode->children[1]->token->text, temptype)) {
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
              if (symtab_insert(functype->info.function.symtab, treenode->children[1]->token->text, temptype)) {
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
    case direct_declarator-1: {   /* direct_declarator '(' parameter_declaration_clause ')' */
//    case direct_declarator-2:   /* CLASS_NAME '(' parameter_declaration_clause ')' */
//    case direct_declarator-3:   /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')' */
//    case direct_declarator-4:   /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
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

Type *sem_typecheck(TreeNode *treenode) {
  LOG_ASSERT(treenode);
  switch (treenode->label) {
    case declaration_seq-1:
    case statement_seq-1: {
      LOG_ASSERT(treenode->children[0]);
      sem_typecheck(treenode->children[0]);
    }
    break;
    case declaration_seq-2:
    case statement_seq-2: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      sem_typecheck(treenode->children[0]);
      sem_typecheck(treenode->children[1]);
    }
    break;
    case function_definition-1: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      LOG_ASSERT(treenode->children[1]->children[0]);
      LOG_ASSERT(treenode->children[1]->children[0]->token);
      LOG_ASSERT(treenode->children[2]);
      // TODO: clean up the line below. I don't like the nested children call.
      SymtabNode *node = symtab_lookup(sem_current, treenode->children[1]->children[0]->token->text);
      Symtab *temp = sem_current;
      sem_current = sem_get_function_symtab(node->type);
      LOG_ASSERT(sem_current);
      sem_typecheck(treenode->children[2]);
      sem_current = temp;
      return sem_typecheck(treenode->children[0]);
    }
    break;
    case compound_statement: {
      LOG_ASSERT(treenode->children[1]);
      if (treenode->children[1]->label != '}')
        return sem_typecheck(treenode->children[1]);
      else
        return type_get_basetype(UNKNOWN_T);
    }
    break;
    case expression_statement: {
      LOG_ASSERT(treenode->children[0]);
      return sem_typecheck(treenode->children[0]);
    }
    break;
    case simple_declaration-1: {
      LOG_ASSERT(treenode->children[1]);
      return sem_typecheck(treenode->children[1]);
    }
    break;
    case init_declarator_list: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      sem_typecheck(treenode->children[0]);
      sem_typecheck(treenode->children[2]);
    }
    break;
    case init_declarator-2: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[1]);
      Type *type1 = sem_typecheck(treenode->children[0]);
      if (type1->basetype == ARRAY_T) {
        type1 = type1->info.array.type;
      }
      Type *type2 = sem_typecheck(treenode->children[1]);
      if (treenode->cnum > 1 && type_compare(type1, type2) != TYPE_EQUAL) {
        sem_error_from_token("incorrect assignment type", sem_get_leaf(treenode->children[1]));
        return type_get_basetype(UNKNOWN_T);
      } else {
        return type1;
      }
    }
    break;
    case direct_declarator-5: {
      LOG_ASSERT(treenode->children[0]);
      return sem_typecheck(treenode->children[0]);
    }
    break;
    case initializer: {
      LOG_ASSERT(treenode->children[1]);
      return sem_typecheck(treenode->children[1]);
    }
    break;
    case initializer_clause: {
      return sem_typecheck(treenode->children[1]);
    }
    break;
    case initializer_list: {
      Type *type = sem_typecheck(treenode->children[0]);
      if (type_compare(type, sem_typecheck(treenode->children[2])) != TYPE_EQUAL) {
        return type_get_basetype(UNKNOWN_T);
      } else {
        return type;
      }
    }
    break;
    case postfix_expression: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[0]->token);
      LOG_ASSERT(treenode->children[0]->token->text);
      LOG_ASSERT(treenode->children[2]);
      if (sem_typecheck(treenode->children[2]) != type_from_terminal(INT))
        sem_error_from_token("array subscript is not an integer", sem_get_leaf(treenode->children[2]));
      return symtab_lookup(sem_current, treenode->children[0]->token->text)->type;
    }
    break;
    case additive_expression:
    case multiplicative_expression:
    case assignment_expression:
    case relational_expression: {
      LOG_ASSERT(treenode->children[0]);
      LOG_ASSERT(treenode->children[2]);
      Type *type1 = sem_typecheck(treenode->children[0]);
      Type *type2 = sem_typecheck(treenode->children[2]);
      if (type_compare(type1, type2) == TYPE_EQUAL) {
        return type1;
      } else {
        sem_type_error(sem_get_leaf(treenode->children[0]), type1, type2);
        return type_get_basetype(UNKNOWN_T);
      }
    }
    break;
    case primary_expression: {
      LOG_ASSERT(treenode->children[1]);
      return sem_typecheck(treenode->children[1]);
    }
    break;
    case IDENTIFIER:
    case CLASS_NAME: {
      LOG_ASSERT(treenode->token);
      LOG_ASSERT(treenode->token->text);
      SymtabNode *node = symtab_lookup(sem_current, treenode->token->text);
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

void sem_type_error(Token *token, Type *type1, Type *type2) {
  log_error(
    SEM_ERROR, "SEMANTIC ERROR:%s:%d: Near the token '%s' types %s and %s do not match. \n",
    token->filename,
    token->lineno,
    token->text,
    type_to_string(type1),
    type_to_string(type2)
  );
}