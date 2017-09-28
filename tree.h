#ifndef TREE_H
#define TREE_H
/*
 * Abstract Syntax Tree declarations.
 */
#include "token.h"

// Abstract Syntax Tree
typedef struct TreeNode {
  int nonterm;                   // The production rule
  int cnum;                       // How many children
  struct TreeNode *children[1];   // Array of child nodes size from 0..cnum
  Token *leaf;                    // The token if its a leaf
} TreeNode;

// build a Node
TreeNode *newTreeNode(int nonterm, int cnum, ...);

// delete and free TreeNode and children
void freeTreeNode(TreeNode *root);
#endif
