#include <stdio.h>
#include <string.h>
#include "../header/nonterm.h"
#include "../header/tree.h"
#include "../header/semantic.h"
#include "../header/logger.h"

extern FILE *yyin;
extern int yyparse();
extern TreeNode *astRoot;
extern Symtab *sem_global;
extern int log_first_error;
extern void set_filename(char *fname);

int main(int argc, char **argv) {
  log_init_global();
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      printf("Attempting to open \"%s\"\n", argv[i]);
      yyin = fopen(argv[i], "r");
      if (yyin) {
        printf("Success.\n");
        // Change filename and reset yylineno
        set_filename(argv[i]);
        // Lexical and Syntax analysis
        yyparse();
        // Semantic analysis
        sem_init_global();
        sem_populate(astRoot);
        log_final_status();
        // debug
        printf("\nVisual Representation of hashtable:\n");
        symtab_print_table(sem_global, 0);
        fclose(yyin);
      } else {
        log_error(INTERNAL_ERROR, "Could not open \"%s\"\n", argv[i]);
      }
    }
  }
  
  if (log_first_error) {
    return log_first_error;
  } else {
    return 0;
  }
}