#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"
#include "../header/symtab.h"

int sem_init_global();

void sem_declaration(TreeNode *root);
void sem_declaration_function(TreeNode *node);
void sem_declaration_class(TreeNode *node);

void sem_populate_init_declarator(TreeNode *treenode, Type *type);

#endif