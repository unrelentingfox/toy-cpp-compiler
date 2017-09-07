#include "120++.h"

extern FILE *yyin;
extern struct Token yytoken;

int main(int argc, char **argv) {

	// LinkedList
	struct TokenNode *tokenNodeHead;
	struct TokenNode *tokenNodeTail;
	tokenNodeHead = (struct TokenNode*)malloc(sizeof(struct TokenNode));
	tokenNodeTail = tokenNodeHead;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			yyin = fopen(argv[i], "r");
			if (!yyin) {
				//error
			} else {
				while(yylex()){
					tokenNodeTail->next = createNode(yytoken);
					tokenNodeTail = tokenNodeTail->next;
				}
			}
		}
	}
	return 0;
}

// Copies the passed in token into a new node.
struct TokenNode* createNode(struct Token token){
	struct TokenNode *newNode = (struct TokenNode*)malloc(sizeof(struct TokenNode));
	newNode->token.category = token.category;
	newNode->token.text = strdup(token.text);
	newNode->token.lineno = token.lineno;
	newNode->token.filename = strdup(token.filename);
	newNode->token.ival = token.ival;
	newNode->token.dval = token.dval;
	newNode->token.sval = strdup(token.sval);

	return newNode;
}