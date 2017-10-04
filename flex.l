/*   $Id: lexer.l,v 1.2 1997/11/19 15:13:15 sandro Exp $   */

 /*
 * Copyright (c) 1997 Sandro Sigala <ssigala@globalnet.it>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /*
 * ISO C++ lexical analyzer.
 *
 * Based on the ISO C++ draft standard of December '96.
 * 
 * Adapted by Dustin Fox.
 */

%option noyywrap
%x PARSE_COMMENT

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "token.h"
#include "bison.tab.h"
#include "tree.h"

/*
 * Debug info 
 */
char* filename;
int errors;
int lineno;
int saw_iostream, saw_string, saw_std;


// Filename Stack struct
typedef struct FileNameStack {
  char* filename;
  int lineno;
  struct FileNameStack *next;
} FileNameStack;

FileNameStack *filenameStack;

int typenametable_insert(char *s, int cat);
int typenametable_lookup(char *s);

static void lexerr(char *s);
static void handle_include();
static int set_yylval(int category);
static void record_comment();
static void single_line_comment();

static void skip_until_eol(void);
static void skip_comment(void);
static int check_identifier(char *);
%}

intsuffix   ([uU][lL]?)|([lL][uU]?)
fracconst   ([0-9]*\.[0-9]+)|([0-9]+\.)
exppart     ([eE][-+]?[0-9]+)
floatsuffix ([fFlL])
chartext    ([^'])|(\\.)
stringtext  ([^"])|(\\.)

%%

"\n"                                  { ++lineno; }
[\t\f\v\r ]+                          { /* Ignore whitespace. */ }
"/*"                                  { skip_comment(); }
"//"                                  { skip_until_eol(); }
"{"                                   { return set_yylval('{'); }
"<%"                                  { return set_yylval('{'); }
"}"                                   { return set_yylval('}'); }
"%>"                                  { return set_yylval('}'); }
"["                                   { return set_yylval('['); }
"<:"                                  { return set_yylval('['); }
"]"                                   { return set_yylval(']'); }
":>"                                  { return set_yylval(']'); }
"("                                   { return set_yylval('('); }
")"                                   { return set_yylval(')'); }
";"                                   { return set_yylval(';'); }
":"                                   { return set_yylval(':'); }
"..."                                 { return set_yylval(ELLIPSIS); }
"?"                                   { return set_yylval('?'); }
"::"                                  { return set_yylval(COLONCOLON); }
"."                                   { return set_yylval('.'); }
".*"                                  { return set_yylval(DOTSTAR); }
"+"                                   { return set_yylval('+'); }
"-"                                   { return set_yylval('-'); }
"*"                                   { return set_yylval('*'); }
"/"                                   { return set_yylval('/'); }
"%"                                   { return set_yylval('%'); }
"^"                                   { return set_yylval('^'); }
"xor"                                 { return set_yylval('^'); }
"&"                                   { return set_yylval('&'); }
"bitand"                              { return set_yylval('&'); }
"|"                                   { return set_yylval('|'); }
"bitor"                               { return set_yylval('|'); }
"~"                                   { return set_yylval('~'); }
"compl"                               { return set_yylval('~'); }
"!"                                   { return set_yylval('!'); }
"not"                                 { return set_yylval('!'); }
"="                                   { return set_yylval('='); }
"<"                                   { return set_yylval('<'); }
">"                                   { return set_yylval('>'); }
"+="                                  { return set_yylval(ADDEQ); }
"-="                                  { return set_yylval(SUBEQ); }
"*="                                  { return set_yylval(MULEQ); }
"/="                                  { return set_yylval(DIVEQ); }
"%="                                  { return set_yylval(MODEQ); }
"^="                                  { return set_yylval(XOREQ); }
"xor_eq"                              { return set_yylval(XOREQ); }
"&="                                  { return set_yylval(ANDEQ); }
"and_eq"                              { return set_yylval(ANDEQ); }
"|="                                  { return set_yylval(OREQ); }
"or_eq"                               { return set_yylval(OREQ); }
"<<"                                  { return set_yylval(SL); }
">>"                                  { return set_yylval(SR); }
"<<="                                 { return set_yylval(SLEQ); }
">>="                                 { return set_yylval(SREQ); }
"=="                                  { return set_yylval(EQ); }
"!="                                  { return set_yylval(NOTEQ); }
"not_eq"                              { return set_yylval(NOTEQ); }
"<="                                  { return set_yylval(LTEQ); }
">="                                  { return set_yylval(GTEQ); }
"&&"                                  { return set_yylval(ANDAND); }
"and"                                 { return set_yylval(ANDAND); }
"||"                                  { return set_yylval(OROR); }
"or"                                  { return set_yylval(OROR); }
"++"                                  { return set_yylval(PLUSPLUS); }
"--"                                  { return set_yylval(MINUSMINUS); }
","                                   { return set_yylval(','); }
"->*"                                 { return set_yylval(ARROWSTAR); }
"->"                                  { return set_yylval(ARROW); }
"asm"                                 { return set_yylval(ASM); }
"auto"                                { return set_yylval(AUTO); }
"bool"                                { return set_yylval(BOOL); }
"break"                               { return set_yylval(BREAK); }
"case"                                { return set_yylval(CASE); }
"catch"                               { return set_yylval(CATCH); }
"char"                                { return set_yylval(CHAR); }
"class"                               { return set_yylval(CLASS); }
"const"                               { return set_yylval(CONST); }
"const_cast"                          { return set_yylval(CONST_CAST); }
"continue"                            { return set_yylval(CONTINUE); }
"default"                             { return set_yylval(DEFAULT); }
"delete"                              { return set_yylval(DELETE); }
"do"                                  { return set_yylval(DO); }
"double"                              { return set_yylval(DOUBLE); }
"dynamic_cast"                        { return set_yylval(DYNAMIC_CAST); }
"else"                                { return set_yylval(ELSE); }
"enum"                                { return set_yylval(ENUM); }
"explicit"                            { return set_yylval(EXPLICIT); }
"export"                              { return set_yylval(EXPORT); }
"extern"                              { return set_yylval(EXTERN); }
"false"                               { return set_yylval(FALSE); }
"float"                               { return set_yylval(FLOAT); }
"for"                                 { return set_yylval(FOR); }
"friend"                              { return set_yylval(FRIEND); }
"goto"                                { return set_yylval(GOTO); }
"if"                                  { return set_yylval(IF); }
"inline"                              { return set_yylval(INLINE); }
"int"                                 { return set_yylval(INT); }
"long"                                { return set_yylval(LONG); }
"mutable"                             { return set_yylval(MUTABLE); }
"namespace"                           { return set_yylval(NAMESPACE); }
"new"                                 { return set_yylval(NEW); }
"operator"                            { return set_yylval(OPERATOR); }
"private"                             { return set_yylval(PRIVATE); }
"protected"                           { return set_yylval(PROTECTED); }
"public"                              { return set_yylval(PUBLIC); }
"register"                            { return set_yylval(REGISTER); }
"reinterpret_cast"                    { return set_yylval(REINTERPRET_CAST); }
"return"                              { return set_yylval(RETURN); }
"short"                               { return set_yylval(SHORT); }
"signed"                              { return set_yylval(SIGNED); }
"sizeof"                              { return set_yylval(SIZEOF); }
"static"                              { return set_yylval(STATIC); }
"static_cast"                         { return set_yylval(STATIC_CAST); }
"struct"                              { return set_yylval(STRUCT); }
"switch"                              { return set_yylval(SWITCH); }
"template"                            { return set_yylval(TEMPLATE); }
"this"                                { return set_yylval(THIS); }
"throw"                               { return set_yylval(THROW); }
"true"                                { return set_yylval(TRUE); }
"try"                                 { return set_yylval(TRY); }
"typedef"                             { return set_yylval(TYPEDEF); }
"typeid"                              { return set_yylval(TYPEID); }
"typename"                            { return set_yylval(TYPENAME); }
"union"                               { return set_yylval(UNION); }
"unsigned"                            { return set_yylval(UNSIGNED); }
"using"                               { return set_yylval(USING); }
"virtual"                             { return set_yylval(VIRTUAL); }
"void"                                { return set_yylval(VOID); }
"volatile"                            { return set_yylval(VOLATILE); }
"wchar_t"                             { return set_yylval(WCHAR_T); }
"while"                               { return set_yylval(WHILE); }
[a-zA-Z_][a-zA-Z_0-9]*                { return set_yylval(check_identifier(yytext)); }
"0"[xX][0-9a-fA-F]+{intsuffix}?       { return set_yylval(INTEGER); }
"0"[0-7]+{intsuffix}?                 { return set_yylval(INTEGER); }
[0-9]+{intsuffix}?                    { return set_yylval(INTEGER); }
{fracconst}{exppart}?{floatsuffix}?   { return set_yylval(FLOATING); }
[0-9]+{exppart}{floatsuffix}?         { return set_yylval(FLOATING); }
"'"{chartext}*"'"                     { return set_yylval(CHARACTER); }
"L'"{chartext}*"'"                    { return set_yylval(CHARACTER); }
"\""{stringtext}*"\""                 { return set_yylval(STRING); }
"L\""{stringtext}*"\""                { return set_yylval(STRING); }
"#include"[ ]*\"[^\\"]+\"             { handle_include(); }
"#include <iostream>"                 { saw_iostream = 1; }
"#include <string>"                   { saw_string = 1; }
"using namespace std;"                { 
                                        saw_std = 1;
                                        if (saw_string) typenametable_insert("string", CLASS_NAME);
                                        if (saw_iostream) {
                                           typenametable_insert("ifstream", CLASS_NAME);
                                           typenametable_insert("ofstream", CLASS_NAME);
                                        }
                                      }
<<EOF>>                               {
                                        free(filename);
                                        //get the filename from the stack
                                        if(filenameStack){
                                          if(filenameStack->filename)
                                            filename = strdup(filenameStack->filename);
                                          if(filenameStack->lineno)
                                            lineno = filenameStack->lineno;
                                          else
                                            lineno = 1;
                                        }
                                        yypop_buffer_state();
                                        if (!YY_CURRENT_BUFFER){
                                          return(0);
                                        }
                                      }
.                                     { lexerr("unexpected character"); }

%%

/*
 * We use this routine instead a lex pattern because we don't need
 * to save the matched comment in the `yytext' buffer.
 */
static void skip_comment(void)
{
   int c1, c2;

   c1 = input();
   c2 = input();

   while (c2 != EOF && !(c1 == '*' && c2 == '/')) {
      if (c1 == '\n')
         ++lineno;
      c1 = c2;
      c2 = input();
   }
}

/*
 * See the previous comment for an explanation.
 */
static void skip_until_eol(void)
{
   int c;

   while ((c = input()) != EOF && c != '\n')
      ;
   ++lineno;
}

/*
 * Type name table.
 * Replace this with a hash table, when you get a chance.
 */
struct typenametable_entry {
   char *name;
   int category;
   struct typenametable_entry *next;
   } *head;

int typenametable_lookup(char *s)
{
   struct typenametable_entry *tmp = head;
   while (tmp) {
      if (!strcmp(s, tmp->name)) return tmp->category;
      tmp = tmp->next;
   }
   return IDENTIFIER;
}

int typenametable_insert(char *s, int cat)
{
   struct typenametable_entry *tmp = head;
   while (tmp) {
      if (!strcmp(s, tmp->name)) {
         fprintf(stderr,
       "warning, %s is already in the typename table as a %d\n",
       s, tmp->category );
         return tmp->category;
         }
      tmp = tmp->next;
   }
   tmp = calloc(1, sizeof (struct typenametable_entry));
   if(!tmp) { fprintf(stderr, "tnti out of memory\n"); return -1; }
   tmp->name = strdup(s);
   tmp->category = cat;
   tmp->next = head;
   head = tmp;
}

static int
check_identifier(char *s)
{
   return typenametable_lookup(s);
}

static int set_yylval(int category) {
  // Create TreeNode leaf node.
  yylval.treenode = (TreeNode*)calloc(1,sizeof(TreeNode));
  yylval.treenode->leaf = true;
  yylval.treenode->nonterm = 0;
  yylval.treenode->cnum = 0;

  // Create the Token to put in the leaf node.
  yylval.treenode->token = (Token*)malloc(sizeof(Token));
  yylval.treenode->token->filename = strdup(filename);
  yylval.treenode->token->category = category;
  if(yytext)
    yylval.treenode->token->text = strdup(yytext);
  else
    yylval.treenode->token->text = strdup("");
  yylval.treenode->token->lineno = lineno;
  if(category == INT)
    yylval.treenode->token->ival = atoi(yytext);
  else
    yylval.treenode->token->ival = -1;
  if(category == DOUBLE)
    yylval.treenode->token->dval = atof(yytext);
  else
    yylval.treenode->token->dval = -1;
  if(category == STRING)
    yylval.treenode->token->sval = strdup(yytext);
  else
    yylval.treenode->token->sval = strdup("");
  return category;
}


/*
 * Handle lexical errors.
 */
static void lexerr(char *s)
{
  errors++;
  fprintf(stderr, "LEXICAL ERROR: In file (%s) line number (%d) %s",yylval.treenode->token->filename, lineno, s);
  fprintf(stderr, ", token = \"%s\"\n", yytext);
  exit(1);
}

/*
 * Parse all include files before parsing current file.
 */
static void handle_include()
{
   char *fname = strchr(yytext, '\"')+1;
   fname[strlen(fname)-1] = '\0';
   FILE *file = fopen(fname,"r");
   YY_BUFFER_STATE buffer;
   if (file == NULL) {
     lexerr("cannot open include file");
   } else{
      // Push the original filename onto a stack.
      FileNameStack *tempNode = (FileNameStack*)malloc(sizeof(FileNameStack));
      tempNode->filename = strdup(filename);
      tempNode->lineno = lineno;
      tempNode->next = filenameStack;
      filenameStack = tempNode;
      // Set filename and yyin to the new name
      free(filename);
      filename = strdup(fname);
      yypush_buffer_state(yy_create_buffer(file, YY_BUF_SIZE));
      // Reset lineno
      lineno = 1;
   }
}