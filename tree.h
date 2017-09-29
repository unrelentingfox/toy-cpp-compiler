#ifndef TREE_H
#define TREE_H
/*
 * Abstract Syntax Tree declarations.
 */
#include "token.h"

// Boolean functionality for readability
typedef int bool;
#define true 1
#define false 0

// Abstract Syntax Tree
typedef struct TreeNode {
  bool leaf;                      // tell whether this is a leaf note or not.
  int nonterm;
  int cnum;                       // How many children
  struct TreeNode *children[10];   // Array of child nodes size from 0..cnum
  Token *token;                    // The token if its a leaf
} TreeNode;

// Build a Node
TreeNode *newTreeNode(int nonterm, int cnum, ...);

// Print the tree from node
int printTree(TreeNode *t, int depth);

// Delete and free TreeNode and children
void freeTreeNode(TreeNode *root);
#endif
