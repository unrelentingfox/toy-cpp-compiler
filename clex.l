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

extern int yychar;

void lexerr(char *s);
void handle_include();

/* #define DEBUG */

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
"alignas"               { printf("(%d)(%s)",ALIGNAS,yytext);}
"alignof"               { printf("(%d)(%s)",ALIGNOF,yytext);}
"asm"                   { printf("(%d)(%s)",ASM,yytext);}
"auto"                  { printf("(%d)(%s)",AUTO,yytext);}
"bool"                  { printf("(%d)(%s)",BOOL,yytext);}
"break"                 { printf("(%d)(%s)",BREAK,yytext);}
"case"                  { printf("(%d)(%s)",CASE,yytext);}
"catch"                 { printf("(%d)(%s)",CATCH,yytext);}
"char"                  { printf("(%d)(%s)",CHAR,yytext);}
"char16_t"              { printf("(%d)(%s)",CHAR16_T,yytext);}
"char32_t"              { printf("(%d)(%s)",CHAR32_T,yytext);}
"class"                 { printf("(%d)(%s)",CLASS,yytext);}
"const"                 { printf("(%d)(%s)",CONST,yytext);}
"const_cast"            { printf("(%d)(%s)",CONST_CAST,yytext);}
"constexpr"             { printf("(%d)(%s)",CONSTEXPR,yytext);}
"continue"              { printf("(%d)(%s)",CONTINUE,yytext);}
"decltype"              { printf("(%d)(%s)",DECLTYPE,yytext);}
"default"               { printf("(%d)(%s)",DEFAULT,yytext);}
"delete"                { printf("(%d)(%s)",DELETE,yytext);}
"do"                    { printf("(%d)(%s)",DO,yytext);}
"double"                { printf("(%d)(%s)",DOUBLE,yytext);}
"dynamic_cast"          { printf("(%d)(%s)",DYNAMIC_CAST,yytext);}
"else"                  { printf("(%d)(%s)",ELSE,yytext);}
"enum"                  { printf("(%d)(%s)",ENUM,yytext);}
"explicit"              { printf("(%d)(%s)",EXPLICIT,yytext);}
"export"                { printf("(%d)(%s)",EXPORT,yytext);}
"extern"                { printf("(%d)(%s)",EXTERN,yytext);}
"false"                 { printf("(%d)(%s)",FALSE,yytext);}
"float"                 { printf("(%d)(%s)",FLOAT,yytext);}
"for"                   { printf("(%d)(%s)",FOR,yytext);}
"friend"                { printf("(%d)(%s)",FRIEND,yytext);}
"goto"                  { printf("(%d)(%s)",GOTO,yytext);}
"if"                    { printf("(%d)(%s)",IF,yytext);}
"inline"                { printf("(%d)(%s)",INLINE,yytext);}
"int"                   { printf("(%d)(%s)",INT,yytext);}
"long"                  { printf("(%d)(%s)",LONG,yytext);}
"mutable"               { printf("(%d)(%s)",MUTABLE,yytext);}
"namespace"             { printf("(%d)(%s)",NAMESPACE,yytext);}
"new"                   { printf("(%d)(%s)",NEW,yytext);}
"noexcept"              { printf("(%d)(%s)",NOEXCEPT,yytext);}
"nullptr"               { printf("(%d)(%s)",NULLPTR,yytext);}
"operator"              { printf("(%d)(%s)",OPERATOR,yytext);}
"private"               { printf("(%d)(%s)",PRIVATE,yytext);}
"protected"             { printf("(%d)(%s)",PROTECTED,yytext);}
"public"                { printf("(%d)(%s)",PUBLIC,yytext);}
"register"              { printf("(%d)(%s)",REGISTER,yytext);}
"reinterpret_cast"      { printf("(%d)(%s)",REINTERPRET_CAST,yytext);}
"return"                { printf("(%d)(%s)",RETURN,yytext);}
"short"                 { printf("(%d)(%s)",SHORT,yytext);}
"signed"                { printf("(%d)(%s)",SIGNED,yytext);}
"sizeof"                { printf("(%d)(%s)",SIZEOF,yytext);}
"static"                { printf("(%d)(%s)",STATIC,yytext);}
"static_assert"         { printf("(%d)(%s)",STATIC_ASSERT,yytext);}
"static_cast"           { printf("(%d)(%s)",STATIC_CAST,yytext);}
"struct"                { printf("(%d)(%s)",STRUCT,yytext);}
"switch"                { printf("(%d)(%s)",SWITCH,yytext);}
"template"              { printf("(%d)(%s)",TEMPLATE,yytext);}
"this"                  { printf("(%d)(%s)",THIS,yytext);}
"thread_local"          { printf("(%d)(%s)",THREAD_LOCAL,yytext);}
"throw"                 { printf("(%d)(%s)",THROW,yytext);}
"true"                  { printf("(%d)(%s)",TRUE,yytext);}
"try"                   { printf("(%d)(%s)",TRY,yytext);}
"typedef"               { printf("(%d)(%s)",TYPEDEF,yytext);}
"typeid"                { printf("(%d)(%s)",TYPEID,yytext);}
"typename"              { printf("(%d)(%s)",TYPENAME,yytext);}
"union"                 { printf("(%d)(%s)",UNION,yytext);}
"unsigned"              { printf("(%d)(%s)",UNSIGNED,yytext);}
"using"                 { printf("(%d)(%s)",USING,yytext);}
"virtual"               { printf("(%d)(%s)",VIRTUAL,yytext);}
"void"                  { printf("(%d)(%s)",VOID,yytext);}
"volatile"              { printf("(%d)(%s)",VOLATILE,yytext);}
"wchar_t"               { printf("(%d)(%s)",WCHAR_T,yytext);}
"while"                 { printf("(%d)(%s)",WHILE,yytext);}

{L}({L}|{D})*           { printf("(%d)(%s)",IDENTIFIER,yytext); }


0[xX]{H}+{IS}?          { lexerr("Hex not supported\n"); }

0{O}+{IS}?              { lexerr("Octal not supported\n"); }

{D}+{IS}?               { printf("(%d)(%s)",ICON,yytext); }

'(\\.|[^\\'])+'         { printf("(%d)(%s)",CCON,yytext); }

{D}+{E}{FS}?            { printf("(%d)(%s)",FCON,yytext); }
{D}*"."{D}+({E})?{FS}?  { printf("(%d)(%s)",FCON,yytext); }
{D}+"."{D}*({E})?{FS}?  { printf("(%d)(%s)",FCON,yytext); }

{LIT}                   { printf("(%d)(%s)",STRING,yytext); }

"#include"[ ]*\"[^\\"]+\"  { handle_include(); }

"#include <iostream>"   { included_iostream = 1; }

">>="                   { printf("(%d)(%s)",SRASN,yytext); }
"<<="                   { printf("(%d)(%s)",SLASN,yytext); }
"+="                    { printf("(%d)(%s)",PLASN,yytext); }
"-="                    { printf("(%d)(%s)",MIASN,yytext); }
"*="                    { printf("(%d)(%s)",MUASN,yytext); }
"/="                    { printf("(%d)(%s)",DIASN,yytext); }
"%="                    { printf("(%d)(%s)",MOASN,yytext); }
"&="                    { printf("(%d)(%s)",ANASN,yytext); }
"^="                    { printf("(%d)(%s)",ERASN,yytext); }
"|="                    { printf("(%d)(%s)",ORASN,yytext); }
">>"                    { printf("(%d)(%s)",SHR,yytext); }
"<<"                    { printf("(%d)(%s)",SHL,yytext); }
"++"                    { printf("(%d)(%s)",INCOP,yytext); }
"--"                    { printf("(%d)(%s)",DECOP,yytext); }
"->"                    { printf("(%d)(%s)",FOLLOW,yytext); }
"&&"                    { printf("(%d)(%s)",ANDAND,yytext); }
"||"                    { printf("(%d)(%s)",OROR,yytext); }
"<="                    { printf("(%d)(%s)",LE,yytext); }
">="                    { printf("(%d)(%s)",GE,yytext); }
"=="                    { printf("(%d)(%s)",EQ,yytext); }
"!="                    { printf("(%d)(%s)",NE,yytext); }
";"                     { printf("(%d)(%s)",SM,yytext); }
"{"                     { printf("(%d)(%s)",LC,yytext); }
"}"                     { printf("(%d)(%s)",RC,yytext); }
","                     { printf("(%d)(%s)",CM,yytext); }
":"                     { printf("(%d)(%s)",COLON,yytext); }
"="                     { printf("(%d)(%s)",ASN,yytext); }
"("                     { printf("(%d)(%s)",LP,yytext); }
")"                     { printf("(%d)(%s)",RP,yytext); }
"["                     { printf("(%d)(%s)",LB,yytext); }
"]"                     { printf("(%d)(%s)",RB,yytext); }
"."                     { printf("(%d)(%s)",DOT,yytext); }
"&"                     { printf("(%d)(%s)",AND,yytext); }
"!"                     { printf("(%d)(%s)",BANG,yytext); }
"~"                     { printf("(%d)(%s)",NOT,yytext); }
"-"                     { printf("(%d)(%s)",MINUS,yytext); }
"+"                     { printf("(%d)(%s)",PLUS,yytext); }
"*"                     { printf("(%d)(%s)",MUL,yytext); }
"/"                     { printf("(%d)(%s)",DIV,yytext); }
"%"                     { printf("(%d)(%s)",MOD,yytext); }
"<"                     { printf("(%d)(%s)",LT,yytext); }
">"                     { printf("(%d)(%s)",GT,yytext); }
"^"                     { printf("(%d)(%s)",ER,yytext); }
"|"                     { printf("(%d)(%s)",OR,yytext); }
"?"                     { printf("(%d)(%s)",QUEST,yytext); }
%%

int main(int argc, char **argv)
{
    if (argc > 1)
        yyin = fopen(argv[1], "r");
    else
        yyin = stdin;
     
    yylex();

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
   // fprintf(stdout, "included filename '%s'\n", fname); fflush(stdout);
   yyin = fopen(fname,"r");
   if (yyin == NULL) {
     lexerr("cannot open include file");
     exit(1);
   }
}
