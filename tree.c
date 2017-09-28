#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tree.h"

// build a Node
TreeNode *newTreeNode(int nonterm, int cnum, ...) {
  int i;
  va_list ap;
  TreeNode *ptr = malloc(sizeof(TreeNode)+(cnum-1)*sizeof(TreeNode *));
  if (ptr == NULL) {
    fprintf(stderr, "alctree out of memory\n");
    exit(1);
  }
  ptr->nonterm = nonterm;
  ptr->cnum = cnum;
  // iterate through children and add them to the new treeNode
  va_start(ap, cnum);
  for (i = 0; i < cnum; i++)
    ptr->children[i] = va_arg(ap, TreeNode *);
  va_end(ap);
  return ptr;
}

// delete and free TreeNode and children
void freeTreeNode(TreeNode *root) {

}