O        [0-7]
D        [0-9]
L        [a-zA-Z_]
H        [a-fA-F0-9]
E        [Ee][+-]?{D}+
FS       (f|F|l|L)
IS       (u|U|l|L)
W        [ \t\f]*
LIT      \"(\\.|[^\\"])*\"

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ytab.h"
#include "120++.h"

struct Token yytoken;

void lexerr(char *s);
void handle_include();
int set_yytoken(int category);
void initialize_yytoken();

/* #define DEBUG */
char *file_name = NULL;
int line_num = 1;
int errors;
int included_iostream = 0;
FILE *saved_yyin;
%}

%%

 /* WHITE SPACE */
\n                      { line_num++; }
[ \t\f]+                { }

 /*TODO: Add the extra reserved words.*/
 /* RESERVED WORDS */
"alignas"               { return set_yytoken(ALIGNAS); }
"alignof"               { return set_yytoken(ALIGNOF); }
"asm"                   { return set_yytoken(ASM); }
"auto"                  { return set_yytoken(AUTO); }
"bool"                  { return set_yytoken(BOOL); }
"break"                 { return set_yytoken(BREAK); }
"case"                  { return set_yytoken(CASE); }
"catch"                 { return set_yytoken(CATCH); }
"char"                  { return set_yytoken(CHAR); }
"char16_t"              { return set_yytoken(CHAR16_T); }
"char32_t"              { return set_yytoken(CHAR32_T); }
"class"                 { return set_yytoken(CLASS); }
"const"                 { return set_yytoken(CONST); }
"const_cast"            { return set_yytoken(CONST_CAST); }
"constexpr"             { return set_yytoken(CONSTEXPR); }
"continue"              { return set_yytoken(CONTINUE); }
"decltype"              { return set_yytoken(DECLTYPE); }
"default"               { return set_yytoken(DEFAULT); }
"delete"                { return set_yytoken(DELETE); }
"do"                    { return set_yytoken(DO); }
"double"                { return set_yytoken(DOUBLE); }
"dynamic_cast"          { return set_yytoken(DYNAMIC_CAST); }
"else"                  { return set_yytoken(ELSE); }
"enum"                  { return set_yytoken(ENUM); }
"explicit"              { return set_yytoken(EXPLICIT); }
"export"                { return set_yytoken(EXPORT); }
"extern"                { return set_yytoken(EXTERN); }
"false"                 { return set_yytoken(FALSE); }
"float"                 { return set_yytoken(FLOAT); }
"for"                   { return set_yytoken(FOR); }
"friend"                { return set_yytoken(FRIEND); }
"goto"                  { return set_yytoken(GOTO); }
"if"                    { return set_yytoken(IF); }
"inline"                { return set_yytoken(INLINE); }
"int"                   { return set_yytoken(INT); }
"long"                  { return set_yytoken(LONG); }
"mutable"               { return set_yytoken(MUTABLE); }
"namespace"             { return set_yytoken(NAMESPACE); }
"new"                   { return set_yytoken(NEW); }
"noexcept"              { return set_yytoken(NOEXCEPT); }
"nullptr"               { return set_yytoken(NULLPTR); }
"operator"              { return set_yytoken(OPERATOR); }
"private"               { return set_yytoken(PRIVATE); }
"protected"             { return set_yytoken(PROTECTED); }
"public"                { return set_yytoken(PUBLIC); }
"register"              { return set_yytoken(REGISTER); }
"reinterpret_cast"      { return set_yytoken(REINTERPRET_CAST); }
"return"                { return set_yytoken(RETURN); }
"short"                 { return set_yytoken(SHORT); }
"signed"                { return set_yytoken(SIGNED); }
"sizeof"                { return set_yytoken(SIZEOF); }
"static"                { return set_yytoken(STATIC); }
"static_assert"         { return set_yytoken(STATIC_ASSERT); }
"static_cast"           { return set_yytoken(STATIC_CAST); }
"struct"                { return set_yytoken(STRUCT); }
"switch"                { return set_yytoken(SWITCH); }
"template"              { return set_yytoken(TEMPLATE); }
"this"                  { return set_yytoken(THIS); }
"thread_local"          { return set_yytoken(THREAD_LOCAL); }
"throw"                 { return set_yytoken(THROW); }
"true"                  { return set_yytoken(TRUE); }
"try"                   { return set_yytoken(TRY); }
"typedef"               { return set_yytoken(TYPEDEF); }
"typeid"                { return set_yytoken(TYPEID); }
"typename"              { return set_yytoken(TYPENAME); }
"union"                 { return set_yytoken(UNION); }
"unsigned"              { return set_yytoken(UNSIGNED); }
"using"                 { return set_yytoken(USING); }
"virtual"               { return set_yytoken(VIRTUAL); }
"void"                  { return set_yytoken(VOID); }
"volatile"              { return set_yytoken(VOLATILE); }
"wchar_t"               { return set_yytoken(WCHAR_T); }
"while"                 { return set_yytoken(WHILE); }

{L}({L}|{D})*           { return set_yytoken(IDENTIFIER); }


0[xX]{H}+{IS}?          { lexerr("Hex not supported\n"); }

0{O}+{IS}?              { lexerr("Octal not supported\n"); }

{D}+{IS}?               { return set_yytoken(ICON); }

'(\\.|[^\\'])+'         { return set_yytoken(CCON); }

{D}+{E}{FS}?            { return set_yytoken(FCON); }
{D}*"."{D}+({E})?{FS}?  { return set_yytoken(FCON); }
{D}+"."{D}*({E})?{FS}?  { return set_yytoken(FCON); }

{LIT}                   { return set_yytoken(STRING); }

"#include"[ ]*\"[^\\"]+\"  { handle_include(); }

"#include <iostream>"   { included_iostream = 1; }

">>="                   { return set_yytoken(SRASN); }
"<<="                   { return set_yytoken(SLASN); }
"+="                    { return set_yytoken(PLASN); }
"-="                    { return set_yytoken(MIASN); }
"*="                    { return set_yytoken(MUASN); }
"/="                    { return set_yytoken(DIASN); }
"%="                    { return set_yytoken(MOASN); }
"&="                    { return set_yytoken(ANASN); }
"^="                    { return set_yytoken(ERASN); }
"|="                    { return set_yytoken(ORASN); }
">>"                    { return set_yytoken(SHR); }
"<<"                    { return set_yytoken(SHL); }
"++"                    { return set_yytoken(INCOP); }
"--"                    { return set_yytoken(DECOP); }
"->"                    { return set_yytoken(FOLLOW); }
"&&"                    { return set_yytoken(ANDAND); }
"||"                    { return set_yytoken(OROR); }
"<="                    { return set_yytoken(LE); }
">="                    { return set_yytoken(GE); }
"=="                    { return set_yytoken(EQ); }
"!="                    { return set_yytoken(NE); }
";"                     { return set_yytoken(SM); }
"{"                     { return set_yytoken(LC); }
"}"                     { return set_yytoken(RC); }
","                     { return set_yytoken(CM); }
":"                     { return set_yytoken(COLON); }
"="                     { return set_yytoken(ASN); }
"("                     { return set_yytoken(LP); }
")"                     { return set_yytoken(RP); }
"["                     { return set_yytoken(LB); }
"]"                     { return set_yytoken(RB); }
"."                     { return set_yytoken(DOT); }
"&"                     { return set_yytoken(AND); }
"!"                     { return set_yytoken(BANG); }
"~"                     { return set_yytoken(NOT); }
"-"                     { return set_yytoken(MINUS); }
"+"                     { return set_yytoken(PLUS); }
"*"                     { return set_yytoken(MUL); }
"/"                     { return set_yytoken(DIV); }
"%"                     { return set_yytoken(MOD); }
"<"                     { return set_yytoken(LT); }
">"                     { return set_yytoken(GT); }
"^"                     { return set_yytoken(ER); }
"|"                     { return set_yytoken(OR); }
"?"                     { return set_yytoken(QUEST); }
%%

void initialize_yytoken(){
  yytoken.category = -1;
  yytoken.text = "";
  yytoken.lineno = -1;
  free(yytoken.filename);
  yytoken.ival = -1;
  yytoken.dval = -1.0;
  free(yytoken.sval = NULL);
}

int set_yytoken(int category) {
  initialize_yytoken();
  yytoken.category = category;
  yytoken.text = yytext;
  yytoken.lineno = line_num;
  yytoken.filename = file_name;
  yytoken.ival = 0;
  yytoken.dval = 0.0;
  yytoken.sval = NULL;
  return category;
}

void lexerr(char *s)
{
	errors++;

  fprintf(stderr, "%s: lexical error", s);

	/* to do: add mechanism for reporting file name and line number */

  fprintf(stderr, ", token = \"%s\"\n", yytext);
}

/*
 * Return 1 if done, 0 if yyin points at more input
 */
int yywrap()
{
   return 1;
}

void handle_include()
{
   char *newfilename = malloc(strlen(yytext)+1-strlen("#include \"\""));
   saved_yyin = yyin;
   char *fname = strchr(yytext, '\"')+1;
   fname[strlen(fname)-1] = '\0';
   // fprintf(stdout, "return incluset_yytoken(n); }
   yyin = fopen(fname,"r");
   if (yyin == NULL) {
     lexerr("cannot open include file");
     exit(1);
   }
}
