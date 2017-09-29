#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tree.h"
#include "nonterm.h"

// build a Node
TreeNode *newTreeNode(int nonterm, int cnum, ...) {
  TreeNode *ptr = NULL;
  va_list ap;

  if (cnum > 0) {
    if (cnum == 1) {
      // set $$ equal to the child
      va_start(ap, cnum);
      ptr = va_arg(ap, TreeNode *);
      if (ptr == NULL) {
        fprintf(stderr, "error in newTreeNode function\n");
        exit(1);
      }
      va_end(ap);
      ptr->nonterm = nonterm;

    } else {
      // create a new treenode and set the children if there are more than 1
      int i;
      ptr = malloc(sizeof(TreeNode) + (cnum - 1) * sizeof(TreeNode *));
      if (ptr == NULL) {
        fprintf(stderr, "alctree out of memory\n");
        exit(1);
      }
      ptr->leaf = false;
      ptr->nonterm = nonterm;
      ptr->cnum = cnum;

      // iterate through children and add them to the new treeNode
      va_start(ap, cnum);
      for (i = 0; i < cnum; i++)
        ptr->children[i] = va_arg(ap, TreeNode *);
      va_end(ap);

    }
  }
  //else return NULL
  return ptr;
}

int printTree(TreeNode *node, int depth) {
  int i;

  printf("%*s %s: %d\n", depth * 2, " ", humanReadable(node->nonterm), node->cnum);

  for (i = 0; i < node->cnum; i++) {
    if (node->children[i]) {
      printTree(node->children[i], depth + 1);
    }
  }
}


// delete and free TreeNode and children
void freeTreeNode(TreeNode *root) {

}