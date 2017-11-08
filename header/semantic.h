#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"
#include "../header/symtab.h"

int sem_init_global();

/* Functions for populating the symbol table */
void sem_populate(TreeNode *root);
void sem_populate_class_definition(TreeNode *treenode, Type *type);
Symtab *sem_populate_function_definition(TreeNode *treenode, Type *type, Symtab *symtab);
void sem_populate_parameter_declaration(TreeNode *treenode, Type *functype);
int sem_populate_parameter_definition(TreeNode *treenode, Type *functype, int paramcount);
void sem_populate_declarators(TreeNode *treenode, Type *type);

/* Functions for typechecking */
Type *sem_typecheck(TreeNode *treenode, Symtab *symtab);
int sem_typecheck_function_params(TreeNode *treenode, Symtab *symtab, Type *functype, int param);

/* Other helper functions */
Type *sem_get_type_from_token(TreeNode *treenode);
Token *sem_get_leaf(TreeNode *treenode);
Symtab *sem_get_function_symtab(Type *functype);
void sem_error_from_token(char *message, Token *token);
void sem_type_error(char *message, Token *token, Type *type1, Type *type2);
#endif