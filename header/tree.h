#ifndef TREE_H
#define TREE_H
/*
 * Abstract Syntax Tree declarations.
 */
#include "token.h"
#include <stdbool.h>
#define MAX_CHILDREN 10

// Abstract Syntax Tree
typedef struct TreeNode {
  bool leaf;                                  // tell whether this is a leaf note or not.
  int nonterm;
  int cnum;                                   // How many children
  struct TreeNode *children[MAX_CHILDREN];    // Array of child nodes size from 0..cnum
  Token *token;                               // The token if its a leaf
} TreeNode;

// Build a Node
TreeNode *tree_new(int nonterm, int cnum, ...);

// Print the tree from node
int tree_print(TreeNode *t, int depth);

// Delete and free TreeNode and children
void tree_free(TreeNode *root);
#endif
