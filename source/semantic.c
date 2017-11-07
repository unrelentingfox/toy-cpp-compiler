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
      // TODO: clean up the line below. I don't like the nested children call.
      SymtabNode *node = symtab_lookup(symtab, treenode->children[1]->children[0]->token->text);
      Symtab *functionscope = sem_get_function_symtab(node->type);
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
      LOG_ASSERT(treenode->children[0]->token);
      LOG_ASSERT(treenode->children[0]->token->text);
      LOG_ASSERT(treenode->children[2]);
      // get the function type
      SymtabNode *node = symtab_lookup(symtab, treenode->children[0]->token->text);
      if (node->type && node->type->basetype == FUNCTION_T) { // make sure the function exists
        int nparams = sem_typecheck_function_params(treenode->children[2], symtab, node->type, 0);
        if (nparams != node->type->info.function.nparams) // check if there were enough parameters
          sem_error_from_token("not enough parameters for function call", treenode->children[0]->token);
        return node->type->info.function.returntype;
      } else {
        sem_error_from_token("function does not exist", treenode->children[0]->token);
        return type_get_basetype(UNKNOWN_T);
      }
    }
    break;
    case postfix_expression-4: { // .::IDENTIFIER
      return type_get_basetype(UNKNOWN_T);
    }
    break;
    case postfix_expression-5: { // .IDENTIFIER
      return type_get_basetype(UNKNOWN_T);
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
    SEM_ERROR, "SEMANTIC ERROR:%s:%d: Near the token '%s' %s (types: %s != %s) \n",
    token->filename,
    token->lineno,
    token->text,
    message,
    type_to_string(type1),
    type_to_string(type2)
  );
}