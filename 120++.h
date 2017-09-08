#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ytab.h"

struct Token {
	int category;   /* the integer code returned by yylex */
	char *text;     /* the actual string (lexeme) matched */
	int lineno;     /* the line number on which the token occurs */
	char *filename; /* the source file in which the token occurs */
	int ival;       /* for integer constants, store binary value here */
	double dval;	   /* for real constants, store binary value here */
	char *sval;     /* for string constants, malloc space, de-escape, store */
	/*    the string (less quotes and after escapes) here */
};

struct TokenNode {
	struct Token token;
	struct TokenNode *next;
};

struct StackNode {
	char* filename;
	struct StackNode *next;
};

struct Token yytoken;

struct TokenNode* createNode(struct Token *token);

void printTokens(struct TokenNode *node);
// void printToken();