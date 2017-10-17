#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"
#include "../header/symtab.h"

int sem_init_global();

void sem_populate(TreeNode *root);
void sem_declaration_function(TreeNode *node);
void sem_declaration_class(TreeNode *node);

void sem_populate_init_declarator(TreeNode *treenode, Type *type);

void sem_populate_parameter_declaration(TreeNode *treenode, Type *type);

#endif