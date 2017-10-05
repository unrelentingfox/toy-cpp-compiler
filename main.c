#include <stdio.h>
#include <string.h>
#include "nonterm.h"
#include "tree.h"

extern FILE *yyin;
extern char *filename;
extern int lineno;
extern int yyparse();
extern TreeNode *astRoot;

int main(int argc, char **argv) {
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      yyin = fopen(argv[i], "r");
      if (!yyin) {
        //error
      } else {
        // Change filename and reset yylineno
        filename = strdup(argv[i]);
        lineno = 1;

        if (yyparse()) {
          printf("ERROR");
        }
      }
    }
  }

  printTree(astRoot, 0);
  return 0;
}