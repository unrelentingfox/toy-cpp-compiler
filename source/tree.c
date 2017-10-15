#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../header/tree.h"
#include "../header/nonterm.h"

// build a Node
TreeNode *tree_new(int nonterm, int va_num, ...) {
  TreeNode *ptr = NULL;
  va_list ap;

  if (va_num > 0) {
    if (va_num == 1) {

      // set $$ equal to the child
      va_start(ap, va_num);
      ptr = va_arg(ap, TreeNode *);
      if (ptr != NULL) {
        va_end(ap);
        // ptr->nonterm = nonterm;
      }
    } else {

      // create a new treenode and set the children if there are more than 1
      ptr = malloc(sizeof(TreeNode) + (va_num) * sizeof(TreeNode *));
      if (ptr == NULL) {
        fprintf(stderr, "alctree out of memory\n");
        exit(1);
      }
      ptr->leaf = false;
      ptr->nonterm = nonterm;

      // iterate through children and add them to the new treeNode
      int i = 0;
      int cnum = 0;
      va_start(ap, va_num);
      for (i = 0; i < va_num; i++) {
        ptr->children[cnum] = va_arg(ap, TreeNode *);
        if (ptr->children[cnum] != NULL)
          cnum++;
      }
      ptr->cnum = cnum;
      va_end(ap);
    }
  }
  //else return NULL
  return ptr;
}

int tree_print(TreeNode *node, int depth) {
  int i;

  if (node && node->cnum > 0) {
    printf("%*s %s: %d\n", depth * 2, " ", humanReadable(node->nonterm), node->cnum);

    for (i = 0; i < node->cnum; i++) {
      if (node->children[i]) {
        tree_print(node->children[i], depth + 1);
      }
    }
  } else if (node) {
    printf("%*s LEAF:\"%s\"\n", depth * 2, " ", node->token->text);
  }

}


// delete and free TreeNode and children
void tree_free(TreeNode *root) {

}