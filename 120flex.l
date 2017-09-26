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
#include "120.h"
#include "ytab.h"

/*
 * Debug info 
 */
int errors;
int yylineno;
int saw_iostream, saw_string, saw_std;

StackNode *filenameStack;

Token yytoken;

static void lexerr(char *s);
static void handle_include();
static int set_yytoken(int category);
static void initialize_yytoken();
static void record_comment();
static void single_line_comment();

static void skip_until_eol(void);
static void skip_comment(void);
static int check_identifier(char *);
%}

intsuffix            ([uU][lL]?)|([lL][uU]?)
fracconst            ([0-9]*\.[0-9]+)|([0-9]+\.)
exppart              ([eE][-+]?[0-9]+)
floatsuffix          ([fFlL])
chartext             ([^'])|(\\.)
stringtext           ([^"])|(\\.)

%%

"\n"                 { ++yylineno; }
[\t\f\v\r ]+         { /* Ignore whitespace. */ }

"/*"                 { skip_comment(); }
"//"                 { skip_until_eol(); }

"{"                  { return set_yytoken('{'); }
"<%"                 { return set_yytoken('{'); }
"}"                  { return set_yytoken('}'); }
"%>"                 { return set_yytoken('}'); }
"["                  { return set_yytoken('['); }
"<:"                 { return set_yytoken('['); }
"]"                  { return set_yytoken(']'); }
":>"                 { return set_yytoken(']'); }
"("                  { return set_yytoken('('); }
")"                  { return set_yytoken(')'); }
";"                  { return set_yytoken(';'); }
":"                  { return set_yytoken(':'); }
"..."                { return set_yytoken(ELLIPSIS); }
"?"                  { return set_yytoken('?'); }
"::"                 { return set_yytoken(COLONCOLON); }
"."                  { return set_yytoken('.'); }
".*"                 { return set_yytoken(DOTSTAR); }
"+"                  { return set_yytoken('+'); }
"-"                  { return set_yytoken('-'); }
"*"                  { return set_yytoken('*'); }
"/"                  { return set_yytoken('/'); }
"%"                  { return set_yytoken('%'); }
"^"                  { return set_yytoken('^'); }
"xor"                { return set_yytoken('^'); }
"&"                  { return set_yytoken('&'); }
"bitand"             { return set_yytoken('&'); }
"|"                  { return set_yytoken('|'); }
"bitor"              { return set_yytoken('|'); }
"~"                  { return set_yytoken('~'); }
"compl"              { return set_yytoken('~'); }
"!"                  { return set_yytoken('!'); }
"not"                { return set_yytoken('!'); }
"="                  { return set_yytoken('='); }
"<"                  { return set_yytoken('<'); }
">"                  { return set_yytoken('>'); }
"+="                 { return set_yytoken(ADDEQ); }
"-="                 { return set_yytoken(SUBEQ); }
"*="                 { return set_yytoken(MULEQ); }
"/="                 { return set_yytoken(DIVEQ); }
"%="                 { return set_yytoken(MODEQ); }
"^="                 { return set_yytoken(XOREQ); }
"xor_eq"             { return set_yytoken(XOREQ); }
"&="                 { return set_yytoken(ANDEQ); }
"and_eq"             { return set_yytoken(ANDEQ); }
"|="                 { return set_yytoken(OREQ); }
"or_eq"              { return set_yytoken(OREQ); }
"<<"                 { return set_yytoken(SL); }
">>"                 { return set_yytoken(SR); }
"<<="                { return set_yytoken(SLEQ); }
">>="                { return set_yytoken(SREQ); }
"=="                 { return set_yytoken(EQ); }
"!="                 { return set_yytoken(NOTEQ); }
"not_eq"             { return set_yytoken(NOTEQ); }
"<="                 { return set_yytoken(LTEQ); }
">="                 { return set_yytoken(GTEQ); }
"&&"                 { return set_yytoken(ANDAND); }
"and"                { return set_yytoken(ANDAND); }
"||"                 { return set_yytoken(OROR); }
"or"                 { return set_yytoken(OROR); }
"++"                 { return set_yytoken(PLUSPLUS); }
"--"                 { return set_yytoken(MINUSMINUS); }
","                  { return set_yytoken(','); }
"->*"                { return set_yytoken(ARROWSTAR); }
"->"                 { return set_yytoken(ARROW); }

"asm"                { return set_yytoken(ASM); }
"auto"               { return set_yytoken(AUTO); }
"bool"               { return set_yytoken(BOOL); }
"break"              { return set_yytoken(BREAK); }
"case"               { return set_yytoken(CASE); }
"catch"              { return set_yytoken(CATCH); }
"char"               { return set_yytoken(CHAR); }
"class"              { return set_yytoken(CLASS); }
"const"              { return set_yytoken(CONST); }
"const_cast"         { return set_yytoken(CONST_CAST); }
"continue"           { return set_yytoken(CONTINUE); }
"default"            { return set_yytoken(DEFAULT); }
"delete"             { return set_yytoken(DELETE); }
"do"                 { return set_yytoken(DO); }
"double"             { return set_yytoken(DOUBLE); }
"dynamic_cast"       { return set_yytoken(DYNAMIC_CAST); }
"else"               { return set_yytoken(ELSE); }
"enum"               { return set_yytoken(ENUM); }
"explicit"           { return set_yytoken(EXPLICIT); }
"export"             { return set_yytoken(EXPORT); }
"extern"             { return set_yytoken(EXTERN); }
"false"              { return set_yytoken(FALSE); }
"float"              { return set_yytoken(FLOAT); }
"for"                { return set_yytoken(FOR); }
"friend"             { return set_yytoken(FRIEND); }
"goto"               { return set_yytoken(GOTO); }
"if"                 { return set_yytoken(IF); }
"inline"             { return set_yytoken(INLINE); }
"int"                { return set_yytoken(INT); }
"long"               { return set_yytoken(LONG); }
"mutable"            { return set_yytoken(MUTABLE); }
"namespace"          { return set_yytoken(NAMESPACE); }
"new"                { return set_yytoken(NEW); }
"operator"           { return set_yytoken(OPERATOR); }
"private"            { return set_yytoken(PRIVATE); }
"protected"          { return set_yytoken(PROTECTED); }
"public"             { return set_yytoken(PUBLIC); }
"register"           { return set_yytoken(REGISTER); }
"reinterpret_cast"   { return set_yytoken(REINTERPRET_CAST); }
"return"             { return set_yytoken(RETURN); }
"short"              { return set_yytoken(SHORT); }
"signed"             { return set_yytoken(SIGNED); }
"sizeof"             { return set_yytoken(SIZEOF); }
"static"             { return set_yytoken(STATIC); }
"static_cast"        { return set_yytoken(STATIC_CAST); }
"struct"             { return set_yytoken(STRUCT); }
"switch"             { return set_yytoken(SWITCH); }
"template"           { return set_yytoken(TEMPLATE); }
"this"               { return set_yytoken(THIS); }
"throw"              { return set_yytoken(THROW); }
"true"               { return set_yytoken(TRUE); }
"try"                { return set_yytoken(TRY); }
"typedef"            { return set_yytoken(TYPEDEF); }
"typeid"             { return set_yytoken(TYPEID); }
"typename"           { return set_yytoken(TYPENAME); }
"union"              { return set_yytoken(UNION); }
"unsigned"           { return set_yytoken(UNSIGNED); }
"using"              { return set_yytoken(USING); }
"virtual"            { return set_yytoken(VIRTUAL); }
"void"               { return set_yytoken(VOID); }
"volatile"           { return set_yytoken(VOLATILE); }
"wchar_t"            { return set_yytoken(WCHAR_T); }
"while"              { return set_yytoken(WHILE); }

[a-zA-Z_][a-zA-Z_0-9]*                 { return check_identifier(yytext); }

"0"[xX][0-9a-fA-F]+{intsuffix}?        { return set_yytoken(INTEGER); }
"0"[0-7]+{intsuffix}?                  { return set_yytoken(INTEGER); }
[0-9]+{intsuffix}?                     { return set_yytoken(INTEGER); }

{fracconst}{exppart}?{floatsuffix}?    { return set_yytoken(FLOATING); }
[0-9]+{exppart}{floatsuffix}?          { return set_yytoken(FLOATING); }

"'"{chartext}*"'"                      { return set_yytoken(CHARACTER); }
"L'"{chartext}*"'"                     { return set_yytoken(CHARACTER); }

"\""{stringtext}*"\""                  { return set_yytoken(STRING); }
"L\""{stringtext}*"\""                 { return set_yytoken(STRING); }
"#include"[ ]*\"[^\\"]+\"              { handle_include(); }
"#include <iostream>"                  { saw_iostream = 1; }
"#include <string>"                    { saw_string = 1; }
"using namespace std;"                 { 
                                          saw_std = 1;
                                          if (saw_string) typenametable_insert("string", CLASS_NAME);
                                          if (saw_iostream) {
                                             typenametable_insert("ifstream", CLASS_NAME);
                                             typenametable_insert("ofstream", CLASS_NAME);
                                             }
                                       }
<<EOF>>                 {
                          free(yytoken.filename);
                          //get the filename from the stack
                          if(filenameStack){
                            if(filenameStack->filename)
                              yytoken.filename = strdup(filenameStack->filename);
                            if(filenameStack->lineno)
                              yylineno = filenameStack->lineno;
                            else
                              yylineno = 1;
                          }
                          yypop_buffer_state();
                          if (!YY_CURRENT_BUFFER){
                            return(0);
                          }
}

. { fprintf(stderr, "%d: unexpected character `%c'\n", yylineno, yytext[0]); }

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
         ++yylineno;
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
   ++yylineno;
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


/* 
 * Token setup.
 * This is where all the info about a token is stored.
 */
static void initialize_yytoken(){
  yytoken.category = -1;
  free(yytoken.text);
  yytoken.text = NULL;
  yytoken.lineno = -1;
  yytoken.ival = -1;
  yytoken.dval = -1.0;
  free(yytoken.sval);
  yytoken.sval = NULL;
}

static int set_yytoken(int category) {
  initialize_yytoken();
  yytoken.category = category;
  if(yytext)
    yytoken.text = strdup(yytext);
  yytoken.lineno = yylineno;
  if(category == INT)
    yytoken.ival = atoi(yytext);
  if(category == DOUBLE)
    yytoken.dval = atof(yytext);
  if(category == STRING)
    yytoken.sval = strdup(yytext);
  return category;
}


/*
 * Handle lexical errors.
 */
static void lexerr(char *s)
{
   errors++;

  fprintf(stderr, "In file (%s) line number (%d) %s: lexical error",yytoken.filename, yylineno, s);

  fprintf(stderr, ", token = \"%s\"\n", yytext);
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
     exit(1);
   } else{
      // Push the original filename onto a stack.
      StackNode *tempNode = (StackNode*)malloc(sizeof(StackNode));
      tempNode->filename = strdup(yytoken.filename);
      tempNode->lineno = yylineno;
      tempNode->next = filenameStack;
      filenameStack = tempNode;
      // Set filename and yyin to the new name
      free(yytoken.filename);
      yytoken.filename = strdup(fname);
      yypush_buffer_state(yy_create_buffer(file, YY_BUF_SIZE));
      // Reset yylineno
      yylineno = 1;
   }
}