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
  switch (treenode->nonterm) {
  case function_definition:
  case function_definition-1:
    printf("FUNCTION SCOPES NOT IMPLEMENTED YET! \n");
    //call sem_declaration_function
    break;
  case simple_declaration:
    sem_handle_init_declarator(treenode->children[1], treenode->children[0]->token->text);
    break;
  }

  if (treenode->cnum > 0) {
    for (i = 0; i < treenode->cnum; i++)
      sem_declaration(treenode->children[i]);
  }
}

void sem_handle_init_declarator(TreeNode *treenode, char *basetype) {
  log_assert(treenode, "treenode");
  log_assert(basetype, "basetype");

  Type *newtype = (Type *)malloc(sizeof(Type));
  if (treenode->children[0]->leaf == true) {
    printf("VARIABLE DECLARATION\n");
    newtype->basetype = INT_T;
    symtab_insert(sem_global, treenode->children[0]->token->text, newtype);
  } else
    printf("FUNCTION DELCARATION\n");
}

void sem_declaration_function(TreeNode *treenode);
void sem_declaration_class(TreeNode *treenode);