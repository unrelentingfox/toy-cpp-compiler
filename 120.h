#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ytab.h"

// Token Struct
typedef struct Token {
	int category;   /* the integer code returned by yylex */
	char *text;     /* the actual string (lexeme) matched */
	int lineno;     /* the line number on which the token occurs */
	char *filename; /* the source file in which the token occurs */
	int ival;       /* for integer constants, store binary value here */
	double dval;	   /* for real constants, store binary value here */
	char *sval;     /* for string constants, malloc space, de-escape, store */
	/*    the string (less quotes and after escapes) here */
} Token;

// Leaf Struct
typedef struct LeafNode{
	Token *token;
} LeafNode;

// Token Linked List struct
typedef struct TokenNode {
	struct Token token;
	struct TokenNode *next;
} TokenNode;

// Filename Stack struct
typedef struct StackNode {
	char* filename;
	int lineno;
	struct StackNode *next;
} StackNode;

TokenNode* createNode(Token *token);

void printTokens(TokenNode *node);
// void printToken();