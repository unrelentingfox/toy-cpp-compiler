#include <stdio.h>
#include <string.h>
#include "../header/nonterm.h"
#include "../header/tree.h"
#include "../header/semantic.h"
#include "../header/logger.h"

extern FILE *yyin;
extern char *filename;
extern int lineno;
extern int yyparse();
extern TreeNode *astRoot;
extern Symtab *sem_global;
extern int log_first_error;

int main(int argc, char **argv) {
  log_init_global();
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

  // tree_print(astRoot, 0);
  sem_init_global();
  sem_populate(astRoot);
  symtab_print_table(sem_global);
  if(symtab_lookup(sem_global, "testfunction")){
    printf("found function\n");
  }

  
  if (log_first_error) {
    log_final_status();
    return log_first_error;
  }
}