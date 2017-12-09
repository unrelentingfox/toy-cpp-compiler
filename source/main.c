#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/nonterm.h"
#include "../header/tree.h"
#include "../header/semantic.h"
#include "../header/logger.h"
#include "../header/icodegen.h"

extern FILE *yyin;
extern int yyparse();
extern TreeNode *astRoot;
extern Symtab *sem_global;
extern int log_first_error;
extern void set_filename(char *fname);

int DEBUG_SYMTAB = 0;
int DEBUG_TREE = 0;

int handle_options(int argc, char **argv);

int main(int argc, char **argv) {
  log_init_global();
  if (argc > 1) {
    for (int i = handle_options(argc, argv); i < argc; i++) {
      printf("Opening: \"%s\"\n", argv[i]);
      yyin = fopen(argv[i], "r");
      if (yyin) {
        // Change filename and reset yylineno
        set_filename(argv[i]);
        // Lexical and Syntax analysis
        yyparse();
        // Semantic analysis
        if (!log_first_error) {
          sem_init_global();
          sem_populate(astRoot);
        }
        // Type checking
        if (!log_first_error) {
          sem_typecheck(astRoot, sem_global);
        }
        // Code generation
        if (!log_first_error) {
          TAC_intermediate_code_generation(astRoot, sem_global);
        }
        log_final_status();
        fclose(yyin);
        // Optional debug output
        if (DEBUG_SYMTAB) {
          printf("\nVisual Representation of Symbol Table:\n");
          printf("----------------------------------------\n");
          if (sem_global) {
            symtab_print_table(sem_global, 0);
          }
          printf("----------------------------------------\n");
        }
        if (DEBUG_TREE) {
          printf("\nVisual Representation of Parse Tree:\n");
          printf("----------------------------------------\n");
          if (astRoot) {
            tree_print(astRoot, 0);
          }
          printf("----------------------------------------\n");
        }
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

int handle_options(int argc, char **argv) {
  int i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--tree") == 0 || strcmp(argv[i], "-t") == 0) {
      DEBUG_TREE = 1;
    } else if (strcmp(argv[i], "--symtab") == 0 || strcmp(argv[i], "-s") == 0) {
      DEBUG_SYMTAB = 1;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      printf("Usage: ./120++ [options] file...\n");
      printf("Options:\n");
      printf("  --help   | -h    print this help text.\n");
      printf("  --symtab | -s    print a visual representation of the symbol table.\n");
      printf("  --tree   | -t    print a visual representation of the parse tree.\n");
      exit(0);
    } else
      return i;
  }
  return i;
}