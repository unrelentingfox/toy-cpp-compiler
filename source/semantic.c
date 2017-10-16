#include "../header/semantic.h"
#include "../header/nonterm.h"
#include "../header/logger.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// global scope vairables
Symtab *sem_global;
Symtab *sem_current;

int sem_init_global() {
  //TODO: create a function to free symtab before init.
  sem_global = symtab_new_tree(NULL);
  sem_current = sem_global;
}

void sem_declaration(TreeNode *treenode) {
  int i;
  if (treenode == NULL) {
    printf("tree empty!\n");
    return;
  }
  switch (treenode->label) {
  case function_definition:
  case function_definition-1:
    printf("FUNCTION SCOPES NOT IMPLEMENTED YET! \n");
    //call sem_declaration_function
    break;
  case simple_declaration:
    sem_populate_init_declarator(treenode->children[1],
                                 type_new(treenode->children[0]->label));
    break;
  }

  if (treenode->label < 0) {
    for (i = 0; i < treenode->cnum; i++)
      sem_declaration(treenode->children[i]);
  }
}

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
    if ((symtab_insert(sem_current, treenode->token->text, type))) {
      log_sem_error(treenode->token->filename,
                    treenode->token->lineno,
                    "symbol was already declared",
                    treenode->token->text);
    }
    break;
  case declarator: /* it is a pointer and a direct_declarator */
    sem_populate_init_declarator(treenode->children[1], type);
    break;
  case direct_declarator-1: /* direct_declarator '(' parameter_declaration_clause ')' */
  case direct_declarator-2: /* CLASS_NAME '(' parameter_declaration_clause ')' */
  case direct_declarator-3: /* CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')' */
  case direct_declarator-4: { /* CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')' */
    Type *temp = type_new(FUNCTION_T);
    temp->info.function.returntype = type;
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
  // case direct_declarator-6: /* direct_declarator '[' ']' */
  //TODO: EMPTY ARRAY DECLARATOR IS JUST A POINTER
  // case direct_declarator-7: /* '(' declarator ')' */
  }
}

void sem_declaration_function(TreeNode *treenode);
void sem_declaration_class(TreeNode *treenode);