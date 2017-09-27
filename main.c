#include "structs.h"

extern FILE *yyin;
extern struct Token yytoken;
extern char* filename;
extern int lineno;
extern int yylex();
extern int yyparse();

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

				if(yyparse()){
					printf("ERROR FUCK");
				}
			}
		}
	}
	return 0;
}