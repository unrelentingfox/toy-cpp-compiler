#include "120++.h"

extern FILE *yyin;
extern struct Token yytoken;
extern int yylineno;
extern int yylex();

int main(int argc, char **argv) {

	// LinkedList
	struct TokenNode *tokenNodeHead = NULL;
	struct TokenNode *tokenNodeTail = NULL;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			yyin = fopen(argv[i], "r");
			if (!yyin) {
				//error
			} else {
				// Change filename and reset yylineno
				yytoken.filename = strdup(argv[i]);
				yylineno = 1;

				while (yylex()) {
					if (tokenNodeHead) {
						tokenNodeTail->next = createNode(&yytoken);
						tokenNodeTail = tokenNodeTail->next;
					}
					else {
						tokenNodeHead = createNode(&yytoken);
						tokenNodeTail = tokenNodeHead;
					}
				}
			}
		}
	}
	printTokens(tokenNodeHead);
	return 0;
}

// Copies the passed in token into a new node.
struct TokenNode* createNode(struct Token *token) {
	struct TokenNode *newNode = (struct TokenNode*)malloc(sizeof(struct TokenNode));
	newNode->token.category = token->category;
	if (token->text)
		newNode->token.text = strdup(token->text);
	else
		newNode->token.text = NULL;
	newNode->token.lineno = token->lineno;
	if (token->filename)
		newNode->token.filename = strdup(token->filename);
	else
		newNode->token.filename = NULL;
	newNode->token.ival = token->ival;
	newNode->token.dval = token->dval;
	if (token->sval)
		newNode->token.sval = strdup(token->sval);
	else
		newNode->token.sval = NULL;
	return newNode;
}

void printTokens(struct TokenNode *node) {
	printf("+----------------------------------------------------------------------------------+\n");
	printf("| Category    Text                Lineno    Filename            Ival/Sval  |\n");
	printf("|----------------------------------------------------------------------------------|\n");
	while (node) {
		printf("| %-12d%-20s%-10d%-10s |\n", node->token.category, node->token.text, node->token.lineno, node->token.filename);
		node = node->next;
	}
	printf("+----------------------------------------------------------------------------------+\n");
}

// void printToken() {
// 	printf("(%d)(%s)(%d)(%s)", yytoken.category, yytoken.text, yytoken.lineno, yytoken.filename);
// }