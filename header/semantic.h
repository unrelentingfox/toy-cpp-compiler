#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"
#include "../header/symtab.h"

int sem_init_global();

/* Functions for populating the symbol table */
void sem_populate(TreeNode *root);
void sem_populate_class_definition(TreeNode *treenode, Type *type);
void sem_populate_function_definition(TreeNode *treenode, Type *type);
void sem_populate_parameter_declaration(TreeNode *treenode, Type *functype);
int sem_populate_parameter_definition(TreeNode *treenode, Type *functype, int paramcount);
void sem_populate_declarators(TreeNode *treenode, Type *type);

/* Functions for typechecking */
Type *sem_typecheck_expressions(TreeNode *treenode);

/* Other helper functions */
Type *sem_get_type_from_token(TreeNode *treenode);
#endif