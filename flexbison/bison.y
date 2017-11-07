/*
 * Grammar for 120++, a subset of C++ used in CS 120 at University of Idaho
 *
 * Adaptation by Clinton Jeffery, with help from Matthew Brown, Ranger
 * Adams, and Shea Newton.
 *
 * Based on Sandro Sigala's transcription of the ISO C++ 1996 draft standard.
 * 
 */

/*  $Id: parser.y,v 1.3 1997/11/19 15:13:16 sandro Exp $  */

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
 * ISO C++ parser.
 *
 * Based on the ISO C++ draft standard of December '96.
 */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/logger.h"
#include "../header/nonterm.h"
#include "../header/token.h"
#include "../header/tree.h"

TreeNode *astRoot;

extern int typenametable_insert(char *s, int cat);
extern int yylex(void);

extern int lineno;
extern char *yytext;

/* Debug Options */
//#define YYDEBUG 1
//int yydebug=1;

static void
yyerror(char *message);

static void
sup_error(char *message);

%}
%union {
  struct TreeNode *treenode;
  }

//%type < treenode > identifier
//%type < treenode > direct_declarator parameter_declaration_clause
//%type < treenode > parameter_declaration_list parameter_declaration
//%type < treenode > decl_specifier_seq declarator decl_specifier
//%type < treenode > storage_class_specifier type_specifier simple_type_specifier
//%type < treenode > ptr_operator

%type < treenode > typedef_name
%type < treenode > class_name
%type < treenode > enum_name
%type < treenode > template_name
%type < treenode > identifier
%type < treenode > literal
%type < treenode > integer_literal
%type < treenode > character_literal
%type < treenode > floating_literal
%type < treenode > string_literal
%type < treenode > boolean_literal
%type < treenode > translation_unit
%type < treenode > primary_expression
%type < treenode > id_expression
%type < treenode > unqualified_id
%type < treenode > qualified_id
%type < treenode > nested_name_specifier
%type < treenode > postfix_expression
%type < treenode > expression_list
%type < treenode > unary_expression
%type < treenode > unary_operator
%type < treenode > new_expression
%type < treenode > new_placement
%type < treenode > new_type_id
%type < treenode > new_declarator
%type < treenode > direct_new_declarator
%type < treenode > new_initializer
%type < treenode > delete_expression
%type < treenode > cast_expression
%type < treenode > pm_expression
%type < treenode > multiplicative_expression
%type < treenode > additive_expression
%type < treenode > shift_expression
%type < treenode > relational_expression
%type < treenode > equality_expression
%type < treenode > and_expression
%type < treenode > exclusive_or_expression
%type < treenode > inclusive_or_expression
%type < treenode > logical_and_expression
%type < treenode > logical_or_expression
%type < treenode > conditional_expression
%type < treenode > assignment_expression
%type < treenode > assignment_operator
%type < treenode > expression
%type < treenode > constant_expression
%type < treenode > statement
%type < treenode > labeled_statement
%type < treenode > expression_statement
%type < treenode > compound_statement
%type < treenode > statement_seq
%type < treenode > selection_statement
%type < treenode > condition
%type < treenode > iteration_statement
%type < treenode > for_init_statement
%type < treenode > jump_statement
%type < treenode > declaration_statement
%type < treenode > declaration_seq
%type < treenode > declaration
%type < treenode > block_declaration
%type < treenode > simple_declaration
%type < treenode > decl_specifier
%type < treenode > decl_specifier_seq
%type < treenode > storage_class_specifier
%type < treenode > function_specifier
%type < treenode > type_specifier
%type < treenode > simple_type_specifier
%type < treenode > type_name
%type < treenode > elaborated_type_specifier
%type < treenode > enum_specifier
%type < treenode > enumerator_list
%type < treenode > enumerator_definition
%type < treenode > enumerator
%type < treenode > namespace_name
%type < treenode > original_namespace_name
%type < treenode > namespace_definition
%type < treenode > named_namespace_definition
%type < treenode > original_namespace_definition
%type < treenode > extension_namespace_definition
%type < treenode > unnamed_namespace_definition
%type < treenode > namespace_body
//%type < treenode > namespace_alias
%type < treenode > namespace_alias_definition
%type < treenode > qualified_namespace_specifier
%type < treenode > using_declaration
%type < treenode > using_directive
%type < treenode > asm_definition
%type < treenode > linkage_specification
%type < treenode > init_declarator_list
%type < treenode > init_declarator
%type < treenode > declarator
%type < treenode > direct_declarator
%type < treenode > ptr_operator
%type < treenode > cv_qualifier_seq
%type < treenode > cv_qualifier
%type < treenode > declarator_id
%type < treenode > type_id
%type < treenode > type_specifier_seq
%type < treenode > abstract_declarator
%type < treenode > direct_abstract_declarator
%type < treenode > parameter_declaration_clause
%type < treenode > parameter_declaration_list
%type < treenode > parameter_declaration
%type < treenode > function_definition
%type < treenode > function_body
%type < treenode > initializer
%type < treenode > initializer_clause
%type < treenode > initializer_list
%type < treenode > class_specifier
%type < treenode > class_head
%type < treenode > class_key
%type < treenode > member_specification
%type < treenode > member_declaration
%type < treenode > member_declarator_list
%type < treenode > member_declarator
//%type < treenode > pure_specifier
%type < treenode > constant_initializer
%type < treenode > base_clause
%type < treenode > base_specifier_list
%type < treenode > base_specifier
%type < treenode > access_specifier
%type < treenode > conversion_function_id
%type < treenode > conversion_type_id
%type < treenode > conversion_declarator
%type < treenode > ctor_initializer
%type < treenode > mem_initializer_list
%type < treenode > mem_initializer
%type < treenode > mem_initializer_id
%type < treenode > operator_function_id
%type < treenode > operator
%type < treenode > template_declaration
%type < treenode > template_parameter_list
%type < treenode > template_parameter
%type < treenode > type_parameter
%type < treenode > template_id
%type < treenode > template_argument_list
%type < treenode > template_argument
%type < treenode > explicit_instantiation
%type < treenode > explicit_specialization
%type < treenode > try_block
%type < treenode > function_try_block
%type < treenode > handler_seq
%type < treenode > handler
%type < treenode > exception_declaration
%type < treenode > throw_expression
%type < treenode > exception_specification
%type < treenode > type_id_list
%type < treenode > declaration_seq_opt
%type < treenode > nested_name_specifier_opt
%type < treenode > expression_list_opt
%type < treenode > COLONCOLON_opt
%type < treenode > new_placement_opt
%type < treenode > new_initializer_opt
%type < treenode > new_declarator_opt
%type < treenode > expression_opt
%type < treenode > statement_seq_opt
%type < treenode > condition_opt
%type < treenode > enumerator_list_opt
//%type < treenode > initializer_opt
%type < treenode > constant_expression_opt
%type < treenode > abstract_declarator_opt
%type < treenode > type_specifier_seq_opt
%type < treenode > direct_abstract_declarator_opt
%type < treenode > ctor_initializer_opt
%type < treenode > COMMA_opt
%type < treenode > member_specification_opt
%type < treenode > SEMICOLON_opt
%type < treenode > conversion_declarator_opt
%type < treenode > EXPORT_opt
%type < treenode > handler_seq_opt
%type < treenode > assignment_expression_opt
%type < treenode > type_id_list_opt

%start translation_unit

// nonterms not yet implemented in lexor
%token < treenode > CLASS_NAME
%token < treenode > ENUM_NAME
%token < treenode > IDENTIFIER
%token < treenode > NAMESPACE_NAME
%token < treenode > TEMPLATE_NAME
%token < treenode > TYPEDEF_NAME

%token < treenode > '{'
%token < treenode > '}'
%token < treenode > '['
%token < treenode > ']'
%token < treenode > '('
%token < treenode > ')'
%token < treenode > ';'
%token < treenode > ':'
%token < treenode > ELLIPSIS
%token < treenode > '?'
%token < treenode > COLONCOLON
%token < treenode > '.'
%token < treenode > DOTSTAR
%token < treenode > '+'
%token < treenode > '-'
%token < treenode > '*'
%token < treenode > '/'
%token < treenode > '%'
%token < treenode > '^'
%token < treenode > '&'
%token < treenode > '|'
%token < treenode > '~'
%token < treenode > '!'
%token < treenode > '='
%token < treenode > '<'
%token < treenode > '>'
%token < treenode > ADDEQ
%token < treenode > SUBEQ
%token < treenode > MULEQ
%token < treenode > DIVEQ
%token < treenode > MODEQ
%token < treenode > XOREQ
%token < treenode > ANDEQ
%token < treenode > OREQ
%token < treenode > SL
%token < treenode > SR
%token < treenode > SLEQ
%token < treenode > SREQ
%token < treenode > EQ
%token < treenode > NOTEQ
%token < treenode > LTEQ
%token < treenode > GTEQ
%token < treenode > ANDAND
%token < treenode > OROR
%token < treenode > PLUSPLUS
%token < treenode > MINUSMINUS
%token < treenode > ','
%token < treenode > ARROWSTAR
%token < treenode > ARROW
%token < treenode > ASM
%token < treenode > AUTO
%token < treenode > BOOL
%token < treenode > BREAK
%token < treenode > CASE
%token < treenode > CATCH
%token < treenode > CHAR
%token < treenode > CLASS
%token < treenode > CONST
%token < treenode > CONST_CAST
%token < treenode > CONTINUE
%token < treenode > DEFAULT
%token < treenode > DELETE
%token < treenode > DO
%token < treenode > DOUBLE
%token < treenode > DYNAMIC_CAST
%token < treenode > ELSE
%token < treenode > ENUM
%token < treenode > EXPLICIT
%token < treenode > EXPORT
%token < treenode > EXTERN
%token < treenode > FALSE
%token < treenode > FLOAT
%token < treenode > FOR
%token < treenode > FRIEND
%token < treenode > GOTO
%token < treenode > IF
%token < treenode > INLINE
%token < treenode > INT
%token < treenode > LONG
%token < treenode > MUTABLE
%token < treenode > NAMESPACE
%token < treenode > NEW
%token < treenode > OPERATOR
%token < treenode > PRIVATE
%token < treenode > PROTECTED
%token < treenode > PUBLIC
%token < treenode > REGISTER
%token < treenode > REINTERPRET_CAST
%token < treenode > RETURN
%token < treenode > SHORT
%token < treenode > SIGNED
%token < treenode > SIZEOF
%token < treenode > STATIC
%token < treenode > STATIC_CAST
%token < treenode > STRUCT
%token < treenode > SWITCH
%token < treenode > TEMPLATE
%token < treenode > THIS
%token < treenode > THROW
%token < treenode > TRUE
%token < treenode > TRY
%token < treenode > TYPEDEF
%token < treenode > TYPEID
%token < treenode > TYPENAME
%token < treenode > UNION
%token < treenode > UNSIGNED
%token < treenode > USING
%token < treenode > VIRTUAL
%token < treenode > VOID
%token < treenode > VOLATILE
%token < treenode > WCHAR_T
%token < treenode > WHILE
%token < treenode > INTEGER
%token < treenode > FLOATING
%token < treenode > CHARACTER
%token < treenode > STRING

%%

/*----------------------------------------------------------------------
 * Context-dependent identifiers.
 *----------------------------------------------------------------------*/

typedef_name:
     TYPEDEF_NAME           { $$ = tree_new(typedef_name, 1, $1); }
   ;


namespace_name:
     original_namespace_name           { $$ = tree_new(namespace_name, 1, $1); }
   ;


original_namespace_name:
     NAMESPACE_NAME           { $$ = tree_new(original_namespace_name, 1, $1); }
   ;


class_name:
     CLASS_NAME           { $$ = tree_new(class_name, 1, $1); }
   | template_id           { yyerror("template functions"); $$ = NULL; }
   ;


enum_name:
     ENUM_NAME           { $$ = tree_new(enum_name, 1, $1); }
   ;


template_name:
     TEMPLATE_NAME           { $$ = tree_new(template_name, 1, $1); }
   ;

/*----------------------------------------------------------------------
 * Lexical elements.
 *----------------------------------------------------------------------*/

identifier:
     IDENTIFIER           { $$ = tree_new(identifier, 1, $1); }
   ;


literal:
     integer_literal           { $$ = tree_new(literal, 1, $1); }
   | character_literal           { $$ = tree_new(literal, 1, $1); }
   | floating_literal           { $$ = tree_new(literal, 1, $1); }
   | string_literal           { $$ = tree_new(literal, 1, $1); }
   | boolean_literal           { $$ = tree_new(literal, 1, $1); }
   ;


integer_literal:
     INTEGER           { $$ = tree_new(integer_literal, 1, $1); }
   ;


character_literal:
     CHARACTER           { $$ = tree_new(character_literal, 1, $1); }
   ;


floating_literal:
     FLOATING           { $$ = tree_new(floating_literal, 1, $1); }
   ;


string_literal:
     STRING           { $$ = tree_new(string_literal, 1, $1); }
   ;


boolean_literal:
     TRUE           { $$ = tree_new(boolean_literal, 1, $1); }
   | FALSE           { $$ = tree_new(boolean_literal, 1, $1); }
   ;

/*----------------------------------------------------------------------
 * Translation unit.
 *----------------------------------------------------------------------*/

translation_unit:
     declaration_seq_opt           { astRoot = tree_new(translation_unit, 1, $1); $$ = NULL;}
   ;

/*----------------------------------------------------------------------
 * Expressions.
 *----------------------------------------------------------------------*/

primary_expression:
     literal           { $$ = tree_new(primary_expression, 1, $1); }
   | THIS           { $$ = tree_new(primary_expression, 1, $1); }
   | '(' expression ')'           { $$ = tree_new(primary_expression, 3, $1, $2, $3); }
   | id_expression           { $$ = tree_new(primary_expression, 1, $1); }
   ;


id_expression:
     unqualified_id           { $$ = tree_new(id_expression, 1, $1); }
   | qualified_id           { $$ = tree_new(id_expression, 1, $1); }
   ;


unqualified_id:
     identifier           { $$ = tree_new(unqualified_id, 1, $1); }
   | operator_function_id           { $$ = tree_new(unqualified_id, 1, $1); }
   | conversion_function_id           { $$ = tree_new(unqualified_id, 1, $1); }
   | '~' class_name           { $$ = tree_new(unqualified_id, 2, $1, $2); }
   ;


qualified_id:
     nested_name_specifier unqualified_id           { $$ = tree_new(qualified_id, 2, $1, $2); }
   | nested_name_specifier TEMPLATE unqualified_id           { $$ = tree_new(qualified_id, 3, $1, $2, $3); }
   ;


nested_name_specifier:
     class_name COLONCOLON nested_name_specifier           { $$ = tree_new(nested_name_specifier, 3, $1, $2, $3); }
   | namespace_name COLONCOLON nested_name_specifier           { $$ = tree_new(nested_name_specifier, 3, $1, $2, $3); }
   | class_name COLONCOLON           { $$ = tree_new(nested_name_specifier, 2, $1, $2); }
   | namespace_name COLONCOLON           { $$ = tree_new(nested_name_specifier, 2, $1, $2); }
   ;


postfix_expression:
     primary_expression           { $$ = tree_new(postfix_expression, 1, $1); }
   | postfix_expression '[' expression ']'           { $$ = tree_new(postfix_expression-1, 4, $1, $2, $3, $4); }
   | postfix_expression '(' ')'                       { $$ = tree_new(postfix_expression-2, 3, $1, $2, $3); }
   | postfix_expression '(' expression_list ')'           { $$ = tree_new(postfix_expression-3, 4, $1, $2, $3, $4); }
   | DOUBLE '(' expression_list_opt ')'           { $$ = NULL; sup_error("this"); }
   | INT '(' expression_list_opt ')'           { $$ = NULL; sup_error("this"); }
   | CHAR '(' expression_list_opt ')'           { $$ = NULL; sup_error("this"); }
   | BOOL '(' expression_list_opt ')'           { $$ = NULL; sup_error("this"); }
   | postfix_expression '.' TEMPLATE COLONCOLON id_expression           { $$ = NULL; sup_error("template"); }
   | postfix_expression '.' TEMPLATE id_expression           { $$ = NULL; sup_error("template"); }
   | postfix_expression '.' COLONCOLON id_expression           { $$ = tree_new(postfix_expression-4, 4, $1, $2, $3, $4); }
   | postfix_expression '.' id_expression           { $$ = tree_new(postfix_expression-5, 3, $1, $2, $3); }
   | postfix_expression ARROW TEMPLATE COLONCOLON id_expression           { $$ = NULL; sup_error("template"); }
   | postfix_expression ARROW TEMPLATE id_expression           { $$ = NULL; sup_error("template"); }
   | postfix_expression ARROW COLONCOLON id_expression           { $$ = tree_new(postfix_expression-6, 4, $1, $2, $3, $4); }
   | postfix_expression ARROW id_expression           { $$ = tree_new(postfix_expression-7, 3, $1, $2, $3); }
   | postfix_expression PLUSPLUS           { $$ = tree_new(postfix_expression-8, 2, $1, $2); }
   | postfix_expression MINUSMINUS           { $$ = tree_new(postfix_expression-9, 2, $1, $2); }
   | DYNAMIC_CAST '<' type_id '>' '(' expression ')'           { $$ = NULL; sup_error("cast"); }
   | STATIC_CAST '<' type_id '>' '(' expression ')'           { $$ = NULL; sup_error("cast"); }
   | REINTERPRET_CAST '<' type_id '>' '(' expression ')'           { $$ = NULL; sup_error("cast"); }
   | CONST_CAST '<' type_id '>' '(' expression ')'           { $$ = NULL; sup_error("cast"); }
   | TYPEID '(' expression ')'           { $$ = NULL; sup_error("typeid"); }
   | TYPEID '(' type_id ')'           { $$ = NULL; sup_error("typeid"); }
   ;


expression_list:
     assignment_expression           { $$ = tree_new(expression_list, 1, $1); }
   | expression_list ',' assignment_expression           { $$ = tree_new(expression_list, 3, $1, $2, $3); }
   ;


unary_expression:
     postfix_expression           { $$ = tree_new(unary_expression-1, 1, $1); }
   | PLUSPLUS cast_expression           { $$ = tree_new(unary_expression-2, 2, $1, $2); }
   | MINUSMINUS cast_expression           { $$ = tree_new(unary_expression-3, 2, $1, $2); }
   | '*' cast_expression           { $$ = tree_new(unary_expression-4, 2, $1, $2); }
   | '&' cast_expression           { $$ = tree_new(unary_expression-5, 2, $1, $2); }
   | unary_operator cast_expression           { $$ = tree_new(unary_expression-6, 2, $1, $2); }
   | SIZEOF unary_expression           { $$ = tree_new(unary_expression-7, 2, $1, $2); }
   | SIZEOF '(' type_id ')'           { $$ = tree_new(unary_expression-8, 4, $1, $2, $3, $4); }
   | new_expression           { $$ = tree_new(unary_expression-9, 1, $1); }
   | delete_expression           { $$ = tree_new(unary_expression-10, 1, $1); }
   ;


unary_operator:
     '+'           { $$ = tree_new(unary_operator, 1, $1); }
   | '-'           { $$ = tree_new(unary_operator, 1, $1); }
   | '!'           { $$ = tree_new(unary_operator, 1, $1); }
   | '~'           { $$ = tree_new(unary_operator, 1, $1); }
   ;


new_expression:
     NEW new_placement_opt new_type_id new_initializer_opt           { $$ = tree_new(new_expression, 4, $1, $2, $3, $4); }
   | COLONCOLON NEW new_placement_opt new_type_id new_initializer_opt           { $$ = tree_new(new_expression, 5, $1, $2, $3, $4, $5); }
   | NEW new_placement_opt '(' type_id ')' new_initializer_opt           { $$ = tree_new(new_expression, 6, $1, $2, $3, $4, $5, $6); }
   | COLONCOLON NEW new_placement_opt '(' type_id ')' new_initializer_opt           { $$ = tree_new(new_expression, 7, $1, $2, $3, $4, $5, $6, $7); }
   ;


new_placement:
     '(' expression_list ')'           { $$ = tree_new(new_placement, 3, $1, $2, $3); }
   ;


new_type_id:
     type_specifier_seq new_declarator_opt           { $$ = tree_new(new_type_id, 2, $1, $2); }
   ;


new_declarator:
     ptr_operator new_declarator_opt           { $$ = tree_new(new_declarator, 2, $1, $2); }
   | direct_new_declarator           { $$ = tree_new(new_declarator, 1, $1); }
   ;


direct_new_declarator:
     '[' expression ']'           { $$ = tree_new(direct_new_declarator, 3, $1, $2, $3); }
   | direct_new_declarator '[' constant_expression ']'           { $$ = tree_new(direct_new_declarator, 4, $1, $2, $3, $4); }
   ;


new_initializer:
     '(' expression_list_opt ')'           { $$ = tree_new(new_initializer, 3, $1, $2, $3); }
   ;


delete_expression:
     DELETE cast_expression           { $$ = tree_new(delete_expression, 2, $1, $2); }
   | COLONCOLON DELETE cast_expression           { $$ = tree_new(delete_expression, 3, $1, $2, $3); }
   | DELETE '[' ']' cast_expression           { $$ = tree_new(delete_expression, 4, $1, $2, $3, $4); }
   | COLONCOLON DELETE '[' ']' cast_expression           { $$ = tree_new(delete_expression, 5, $1, $2, $3, $4, $5); }
   ;


cast_expression:
     unary_expression           { $$ = tree_new(cast_expression, 1, $1); }
   | '(' type_id ')' cast_expression           { $$ = tree_new(cast_expression, 4, $1, $2, $3, $4); }
   ;


pm_expression:
     cast_expression           { $$ = tree_new(pm_expression, 1, $1); }
   | pm_expression DOTSTAR cast_expression           { $$ = tree_new(pm_expression, 3, $1, $2, $3); }
   | pm_expression ARROWSTAR cast_expression           { $$ = tree_new(pm_expression, 3, $1, $2, $3); }
   ;


multiplicative_expression:
     pm_expression           { $$ = tree_new(multiplicative_expression, 1, $1); }
   | multiplicative_expression '*' pm_expression           { $$ = tree_new(multiplicative_expression, 3, $1, $2, $3); }
   | multiplicative_expression '/' pm_expression           { $$ = tree_new(multiplicative_expression, 3, $1, $2, $3); }
   | multiplicative_expression '%' pm_expression           { $$ = tree_new(multiplicative_expression, 3, $1, $2, $3); }
   ;


additive_expression:
     multiplicative_expression           { $$ = tree_new(additive_expression, 1, $1); }
   | additive_expression '+' multiplicative_expression           { $$ = tree_new(additive_expression, 3, $1, $2, $3); }
   | additive_expression '-' multiplicative_expression           { $$ = tree_new(additive_expression, 3, $1, $2, $3); }
   ;


shift_expression:
     additive_expression           { $$ = tree_new(shift_expression, 1, $1); }
   | shift_expression SL additive_expression           { $$ = tree_new(shift_expression, 3, $1, $2, $3); }
   | shift_expression SR additive_expression           { $$ = tree_new(shift_expression, 3, $1, $2, $3); }
   ;


relational_expression:
     shift_expression           { $$ = tree_new(relational_expression, 1, $1); }
   | relational_expression '<' shift_expression           { $$ = tree_new(relational_expression, 3, $1, $2, $3); }
   | relational_expression '>' shift_expression           { $$ = tree_new(relational_expression, 3, $1, $2, $3); }
   | relational_expression LTEQ shift_expression           { $$ = tree_new(relational_expression, 3, $1, $2, $3); }
   | relational_expression GTEQ shift_expression           { $$ = tree_new(relational_expression, 3, $1, $2, $3); }
   ;


equality_expression:
     relational_expression           { $$ = tree_new(equality_expression, 1, $1); }
   | equality_expression EQ relational_expression           { $$ = tree_new(equality_expression, 3, $1, $2, $3); }
   | equality_expression NOTEQ relational_expression           { $$ = tree_new(equality_expression, 3, $1, $2, $3); }
   ;


and_expression:
     equality_expression           { $$ = tree_new(and_expression, 1, $1); }
   | and_expression '&' equality_expression           { $$ = NULL; sup_error("single & expression"); }
   ;


exclusive_or_expression:
     and_expression           { $$ = tree_new(exclusive_or_expression, 1, $1); }
   | exclusive_or_expression '^' and_expression           { $$ = NULL; sup_error("exclusive or expression"); }
   ;


inclusive_or_expression:
     exclusive_or_expression           { $$ = tree_new(inclusive_or_expression, 1, $1); }
   | inclusive_or_expression '|' exclusive_or_expression           { $$ = NULL; sup_error("inclusive or expression"); }
   ;


logical_and_expression:
     inclusive_or_expression           { $$ = tree_new(logical_and_expression-1, 1, $1); }
   | logical_and_expression ANDAND inclusive_or_expression           { $$ = tree_new(logical_and_expression-2, 3, $1, $2, $3); }
   ;


logical_or_expression:
     logical_and_expression           { $$ = tree_new(logical_or_expression-1, 1, $1); }
   | logical_or_expression OROR logical_and_expression           { $$ = tree_new(logical_or_expression-2, 3, $1, $2, $3); }
   ;


conditional_expression:
     logical_or_expression           { $$ = tree_new(conditional_expression, 1, $1); }
   | logical_or_expression '?' expression ':' assignment_expression           { $$ = NULL; sup_error("'?:' conditional"); }
   ;


assignment_expression:
     conditional_expression           { $$ = tree_new(assignment_expression, 1, $1); }
   | logical_or_expression assignment_operator assignment_expression           { $$ = tree_new(assignment_expression, 3, $1, $2, $3); }
   | throw_expression           { $$ = tree_new(assignment_expression, 1, $1); }
   ;


assignment_operator:
     '='           { $$ = tree_new(assignment_operator, 1, $1); }
   | MULEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | DIVEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | MODEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | ADDEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | SUBEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | SREQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | SLEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | ANDEQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | XOREQ           { $$ = tree_new(assignment_operator, 1, $1); }
   | OREQ           { $$ = tree_new(assignment_operator, 1, $1); }
   ;


expression:
     assignment_expression           { $$ = tree_new(expression, 1, $1); }
   | expression ',' assignment_expression           { $$ = tree_new(expression, 3, $1, $2, $3); }
   ;


constant_expression:
     conditional_expression           { $$ = tree_new(constant_expression, 1, $1); }
   ;

/*----------------------------------------------------------------------
 * Statements.
 *----------------------------------------------------------------------*/

statement:
     labeled_statement           { $$ = tree_new(statement, 1, $1); }
   | expression_statement           { $$ = tree_new(statement, 1, $1); }
   | compound_statement           { $$ = tree_new(statement, 1, $1); }
   | selection_statement           { $$ = tree_new(statement, 1, $1); }
   | iteration_statement           { $$ = tree_new(statement, 1, $1); }
   | jump_statement           { $$ = tree_new(statement, 1, $1); }
   | declaration_statement           { $$ = tree_new(statement, 1, $1); }
   | try_block           { $$ = tree_new(statement, 1, $1); }
   ;


labeled_statement:
     identifier ':' statement           { $$ = tree_new(labeled_statement, 3, $1, $2, $3); }
   | CASE constant_expression ':' statement           { $$ = tree_new(labeled_statement, 4, $1, $2, $3, $4); }
   | DEFAULT ':' statement           { $$ = tree_new(labeled_statement, 3, $1, $2, $3); }
   ;


expression_statement:
     expression_opt ';'           { $$ = tree_new(expression_statement, 2, $1, $2); }
   ;


compound_statement:
     '{' statement_seq_opt '}'           { $$ = tree_new(compound_statement, 3, $1, $2, $3); }
   ;


statement_seq:
     statement           { $$ = tree_new(statement_seq-1, 1, $1); }
   | statement_seq statement           { $$ = tree_new(statement_seq-2, 2, $1, $2); }
   ;


selection_statement:
     IF '(' condition ')' statement           { $$ = tree_new(selection_statement-1, 5, $1, $2, $3, $4, $5); }
   | IF '(' condition ')' statement ELSE statement           { $$ = tree_new(selection_statement-2, 7, $1, $2, $3, $4, $5, $6, $7); }
   | SWITCH '(' condition ')' statement           { $$ = tree_new(selection_statement-3, 5, $1, $2, $3, $4, $5); }
   ;


condition:
     expression           { $$ = tree_new(condition, 1, $1); }
   | type_specifier_seq declarator '=' assignment_expression           { $$ = tree_new(condition, 4, $1, $2, $3, $4); }
   ;


iteration_statement:
     WHILE '(' condition ')' statement           { $$ = tree_new(iteration_statement, 5, $1, $2, $3, $4, $5); }
   | DO statement WHILE '(' expression ')' ';'           { $$ = tree_new(iteration_statement, 7, $1, $2, $3, $4, $5, $6, $7); }
   | FOR '(' for_init_statement condition_opt ';' expression_opt ')' statement           { $$ = tree_new(iteration_statement, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
   ;


for_init_statement:
     expression_statement           { $$ = tree_new(for_init_statement, 1, $1); }
   | simple_declaration           { $$ = tree_new(for_init_statement, 1, $1); }
   ;


jump_statement:
     BREAK ';'           { $$ = tree_new(jump_statement, 2, $1, $2); }
   | CONTINUE ';'           { $$ = tree_new(jump_statement, 2, $1, $2); }
   | RETURN expression_opt ';'           { $$ = tree_new(jump_statement, 3, $1, $2, $3); }
   | GOTO identifier ';'           { $$ = tree_new(jump_statement, 3, $1, $2, $3); }
   ;


declaration_statement:
     block_declaration           { $$ = tree_new(declaration_statement, 1, $1); }
   ;

/*----------------------------------------------------------------------
 * Declarations.
 *----------------------------------------------------------------------*/

declaration_seq:
     declaration           { $$ = tree_new(declaration_seq-1, 1, $1); }
   | declaration_seq declaration           { $$ = tree_new(declaration_seq-2, 2, $1, $2); }
   ;


declaration:
     block_declaration           { $$ = tree_new(declaration, 1, $1); }
   | function_definition           { $$ = tree_new(declaration, 1, $1); }
   | template_declaration           { $$ = tree_new(declaration, 1, $1); }
   | explicit_instantiation           { $$ = tree_new(declaration, 1, $1); }
   | explicit_specialization           { $$ = tree_new(declaration, 1, $1); }
   | linkage_specification           { $$ = tree_new(declaration, 1, $1); }
   | namespace_definition           { $$ = tree_new(declaration, 1, $1); }
   ;


block_declaration:
     simple_declaration           { $$ = tree_new(block_declaration, 1, $1); }
   | asm_definition           { $$ = tree_new(block_declaration, 1, $1); }
   | namespace_alias_definition           { $$ = tree_new(block_declaration, 1, $1); }
   | using_declaration           { $$ = tree_new(block_declaration, 1, $1); }
   | using_directive           { $$ = tree_new(block_declaration, 1, $1); }
   ;


simple_declaration:
     decl_specifier_seq init_declarator_list ';'           { $$ = tree_new(simple_declaration-1, 3, $1, $2, $3); }
   | decl_specifier_seq ';'           { $$ = tree_new(simple_declaration-2, 2, $1, $2); }
   ;


decl_specifier:
     storage_class_specifier           { $$ = tree_new(decl_specifier, 1, $1); }
   | type_specifier           { $$ = tree_new(decl_specifier, 1, $1); }
   | function_specifier           { $$ = tree_new(decl_specifier, 1, $1); }
   | FRIEND           { $$ = tree_new(decl_specifier, 1, $1); }
   | TYPEDEF           { $$ = tree_new(decl_specifier, 1, $1); }
   ;


decl_specifier_seq:
     decl_specifier           { $$ = tree_new(decl_specifier_seq, 1, $1); }
   | decl_specifier_seq decl_specifier           { $$ = tree_new(decl_specifier_seq, 2, $1, $2); }
   ;


storage_class_specifier:
     AUTO           { $$ = tree_new(storage_class_specifier, 1, $1); }
   | REGISTER           { $$ = tree_new(storage_class_specifier, 1, $1); }
   | STATIC           { $$ = tree_new(storage_class_specifier, 1, $1); }
   | EXTERN           { $$ = tree_new(storage_class_specifier, 1, $1); }
   | MUTABLE           { $$ = tree_new(storage_class_specifier, 1, $1); }
   ;


function_specifier:
     INLINE           { $$ = tree_new(function_specifier, 1, $1); }
   | VIRTUAL           { $$ = tree_new(function_specifier, 1, $1); }
   | EXPLICIT           { $$ = tree_new(function_specifier, 1, $1); }
   ;


type_specifier:
     simple_type_specifier           { $$ = tree_new(type_specifier, 1, $1); }
   | class_specifier           { $$ = tree_new(type_specifier, 1, $1); }
   | enum_specifier           { $$ = tree_new(type_specifier, 1, $1); }
   | elaborated_type_specifier           { $$ = tree_new(type_specifier, 1, $1); }
   | cv_qualifier           { $$ = tree_new(type_specifier, 1, $1); }
   ;


simple_type_specifier:
     type_name           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | nested_name_specifier type_name           { $$ = tree_new(simple_type_specifier, 2, $1, $2); }
   | COLONCOLON nested_name_specifier_opt type_name           { $$ = tree_new(simple_type_specifier, 3, $1, $2, $3); }
   | CHAR           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | WCHAR_T           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | BOOL           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | SHORT           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | INT           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | LONG           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | SIGNED           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | UNSIGNED           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | FLOAT           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | DOUBLE           { $$ = tree_new(simple_type_specifier, 1, $1); }
   | VOID           { $$ = tree_new(simple_type_specifier, 1, $1); }
   ;


type_name:
     class_name           { $$ = tree_new(type_name, 1, $1); }
   | enum_name           { $$ = tree_new(type_name, 1, $1); }
   | typedef_name           { $$ = tree_new(type_name, 1, $1); }
   ;


elaborated_type_specifier:
     class_key COLONCOLON nested_name_specifier identifier           { $$ = tree_new(elaborated_type_specifier, 4, $1, $2, $3, $4); }
   | class_key COLONCOLON identifier           { $$ = tree_new(elaborated_type_specifier, 3, $1, $2, $3); }
   | ENUM COLONCOLON nested_name_specifier identifier           { $$ = tree_new(elaborated_type_specifier, 4, $1, $2, $3, $4); }
   | ENUM COLONCOLON identifier           { $$ = tree_new(elaborated_type_specifier, 3, $1, $2, $3); }
   | ENUM nested_name_specifier identifier           { $$ = tree_new(elaborated_type_specifier, 3, $1, $2, $3); }
   | TYPENAME COLONCOLON_opt nested_name_specifier identifier           { $$ = tree_new(elaborated_type_specifier, 4, $1, $2, $3, $4); }
   | TYPENAME COLONCOLON_opt nested_name_specifier identifier '<' template_argument_list '>'           { $$ = tree_new(elaborated_type_specifier, 7, $1, $2, $3, $4, $5, $6, $7); }
   ;

/*
enum_name:
  identifier
  ;
*/

enum_specifier:
     ENUM identifier '{' enumerator_list_opt '}'           { $$ = tree_new(enum_specifier, 5, $1, $2, $3, $4, $5); }
   ;


enumerator_list:
     enumerator_definition           { $$ = tree_new(enumerator_list, 1, $1); }
   | enumerator_list ',' enumerator_definition           { $$ = tree_new(enumerator_list, 3, $1, $2, $3); }
   ;


enumerator_definition:
     enumerator           { $$ = tree_new(enumerator_definition, 1, $1); }
   | enumerator '=' constant_expression           { $$ = tree_new(enumerator_definition, 3, $1, $2, $3); }
   ;


enumerator:
     identifier           { $$ = tree_new(enumerator, 1, $1); }
   ;

/*
namespace_name:
  original_namespace_name
  | namespace_alias
  ;

original_namespace_name:
  identifier
  ;
*/

namespace_definition:
     named_namespace_definition           { $$ = tree_new(namespace_definition, 1, $1); }
   | unnamed_namespace_definition           { $$ = tree_new(namespace_definition, 1, $1); }
   ;


named_namespace_definition:
     original_namespace_definition           { $$ = tree_new(named_namespace_definition, 1, $1); }
   | extension_namespace_definition           { $$ = tree_new(named_namespace_definition, 1, $1); }
   ;


original_namespace_definition:
     NAMESPACE identifier '{' namespace_body '}'           { $$ = tree_new(original_namespace_definition, 5, $1, $2, $3, $4, $5); }
   ;


extension_namespace_definition:
     NAMESPACE original_namespace_name '{' namespace_body '}'           { $$ = tree_new(extension_namespace_definition, 5, $1, $2, $3, $4, $5); }
   ;


unnamed_namespace_definition:
     NAMESPACE '{' namespace_body '}'           { $$ = tree_new(unnamed_namespace_definition, 4, $1, $2, $3, $4); }
   ;


namespace_body:
     declaration_seq_opt           { $$ = tree_new(namespace_body, 1, $1); }
   ;

/*
namespace_alias:
  identifier
  ;
*/

namespace_alias_definition:
     NAMESPACE identifier '=' qualified_namespace_specifier ';'           { $$ = tree_new(namespace_alias_definition, 5, $1, $2, $3, $4, $5); }
   ;


qualified_namespace_specifier:
     COLONCOLON nested_name_specifier namespace_name           { $$ = tree_new(qualified_namespace_specifier, 3, $1, $2, $3); }
   | COLONCOLON namespace_name           { $$ = tree_new(qualified_namespace_specifier, 2, $1, $2); }
   | nested_name_specifier namespace_name           { $$ = tree_new(qualified_namespace_specifier, 2, $1, $2); }
   | namespace_name           { $$ = tree_new(qualified_namespace_specifier, 1, $1); }
   ;


using_declaration:
     USING TYPENAME COLONCOLON nested_name_specifier unqualified_id ';'           { $$ = tree_new(using_declaration, 6, $1, $2, $3, $4, $5, $6); }
   | USING TYPENAME nested_name_specifier unqualified_id ';'           { $$ = tree_new(using_declaration, 5, $1, $2, $3, $4, $5); }
   | USING COLONCOLON nested_name_specifier unqualified_id ';'           { $$ = tree_new(using_declaration, 5, $1, $2, $3, $4, $5); }
   | USING nested_name_specifier unqualified_id ';'           { $$ = tree_new(using_declaration, 4, $1, $2, $3, $4); }
   | USING COLONCOLON unqualified_id ';'           { $$ = tree_new(using_declaration, 4, $1, $2, $3, $4); }
   ;


using_directive:
     USING NAMESPACE COLONCOLON nested_name_specifier namespace_name ';'           { $$ = tree_new(using_directive, 6, $1, $2, $3, $4, $5, $6); }
   | USING NAMESPACE COLONCOLON namespace_name ';'           { $$ = tree_new(using_directive, 5, $1, $2, $3, $4, $5); }
   | USING NAMESPACE nested_name_specifier namespace_name ';'           { $$ = tree_new(using_directive, 5, $1, $2, $3, $4, $5); }
   | USING NAMESPACE namespace_name ';'           { $$ = tree_new(using_directive, 4, $1, $2, $3, $4); }
   ;


asm_definition:
     ASM '(' string_literal ')' ';'           { $$ = tree_new(asm_definition, 5, $1, $2, $3, $4, $5); }
   ;


linkage_specification:
     EXTERN string_literal '{' declaration_seq_opt '}'           { $$ = tree_new(linkage_specification, 5, $1, $2, $3, $4, $5); }
   | EXTERN string_literal declaration           { $$ = tree_new(linkage_specification, 3, $1, $2, $3); }
   ;

/*----------------------------------------------------------------------
 * Declarators.
 *----------------------------------------------------------------------*/

init_declarator_list:
     init_declarator           { $$ = tree_new(init_declarator_list, 1, $1); }
   | init_declarator_list ',' init_declarator           { $$ = tree_new(init_declarator_list, 3, $1, $2, $3); }
   ;


init_declarator:
     declarator                          { $$ = tree_new(init_declarator-1, 1, $1); }
   | declarator initializer              { $$ = tree_new(init_declarator-2, 2, $1, $2); }
   ;


declarator:
     direct_declarator           { $$ = tree_new(declarator, 1, $1); }
   | ptr_operator declarator           { $$ = tree_new(declarator, 2, $1, $2); }
   ;


direct_declarator:
     declarator_id           { $$ = tree_new(direct_declarator, 1, $1); }
   | direct_declarator '(' parameter_declaration_clause ')' cv_qualifier_seq exception_specification           { $$ = NULL; sup_error("cv_qualifier");}
   | direct_declarator '(' parameter_declaration_clause ')' cv_qualifier_seq           { $$ = NULL; sup_error("cv_qualifier");}
   | direct_declarator '(' parameter_declaration_clause ')' exception_specification            { $$ = NULL; sup_error("exception_specification");}
   | direct_declarator '(' parameter_declaration_clause ')'           { $$ = tree_new(direct_declarator-1, 4, $1, $2, $3, $4); }
   | CLASS_NAME '(' parameter_declaration_clause ')'           { $$ = tree_new(direct_declarator-2, 4, $1, $2, $3, $4); }
   | CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')'           { $$ = tree_new(direct_declarator-3, 6, $1, $2, $3, $4, $5, $6); }
   | CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')'           { $$ = tree_new(direct_declarator-4, 6, $1, $2, $3, $4, $5, $6); }
   | direct_declarator '[' constant_expression ']'           { $$ = tree_new(direct_declarator-5, 4, $1, $2, $3, $4); }
   | direct_declarator '[' ']'           { $$ = NULL; yyerror("size of array is unknown"); }        
   | '(' declarator ')'           { $$ = tree_new(direct_declarator-6, 3, $1, $2, $3); }
   ;


ptr_operator:
     '*'           { $$ = tree_new(ptr_operator, 1, $1); }
   | '*' cv_qualifier_seq           { $$ = tree_new(ptr_operator, 2, $1, $2); }
   | '&'           { $$ = tree_new(ptr_operator, 1, $1); }
   | nested_name_specifier '*'           { $$ = tree_new(ptr_operator, 2, $1, $2); }
   | nested_name_specifier '*' cv_qualifier_seq           { $$ = tree_new(ptr_operator, 3, $1, $2, $3); }
   | COLONCOLON nested_name_specifier '*'           { $$ = tree_new(ptr_operator, 3, $1, $2, $3); }
   | COLONCOLON nested_name_specifier '*' cv_qualifier_seq           { $$ = tree_new(ptr_operator, 4, $1, $2, $3, $4); }
   ;


cv_qualifier_seq:
     cv_qualifier           { $$ = tree_new(cv_qualifier_seq, 1, $1); }
   | cv_qualifier cv_qualifier_seq           { $$ = tree_new(cv_qualifier_seq, 2, $1, $2); }
   ;


cv_qualifier:
     CONST           { $$ = tree_new(cv_qualifier, 1, $1); }
   | VOLATILE           { $$ = tree_new(cv_qualifier, 1, $1); }
   ;


declarator_id:
     id_expression           { $$ = tree_new(declarator_id, 1, $1); }
   | COLONCOLON id_expression           { $$ = tree_new(declarator_id, 2, $1, $2); }
   | COLONCOLON nested_name_specifier type_name           { $$ = tree_new(declarator_id, 3, $1, $2, $3); }
   | COLONCOLON type_name           { $$ = tree_new(declarator_id, 2, $1, $2); }
   ;


type_id:
     type_specifier_seq abstract_declarator_opt           { $$ = tree_new(type_id, 2, $1, $2); }
   ;


type_specifier_seq:
     type_specifier type_specifier_seq_opt           { $$ = tree_new(type_specifier_seq, 2, $1, $2); }
   ;


abstract_declarator:
     ptr_operator abstract_declarator_opt           { $$ = tree_new(abstract_declarator, 2, $1, $2); }
   | direct_abstract_declarator           { $$ = tree_new(abstract_declarator, 1, $1); }
   ;


direct_abstract_declarator:
     direct_abstract_declarator_opt '(' parameter_declaration_clause ')' cv_qualifier_seq exception_specification           { $$ = NULL; sup_error("abstract declarators");}
   | direct_abstract_declarator_opt '(' parameter_declaration_clause ')' cv_qualifier_seq           { $$ = NULL; sup_error("abstract declarators");}
   | direct_abstract_declarator_opt '(' parameter_declaration_clause ')' exception_specification           { $$ = NULL; sup_error("abstract declarators");}
   | direct_abstract_declarator_opt '(' parameter_declaration_clause ')'           { $$ = NULL; sup_error("abstract declarators");}
   | direct_abstract_declarator_opt '[' constant_expression_opt ']'           { $$ = NULL; sup_error("abstract declarators");}
   | '(' abstract_declarator ')'           { $$ = NULL; sup_error("abstract declarators"); }
   ;


parameter_declaration_clause:
     parameter_declaration_list ELLIPSIS           { $$ = tree_new(parameter_declaration_clause, 2, $1, $2); }
   | parameter_declaration_list           { $$ = tree_new(parameter_declaration_clause, 1, $1); }
   | ELLIPSIS           { $$ = tree_new(parameter_declaration_clause, 1, $1); }
   |     /* epsilon */          { $$ = NULL; }
   | parameter_declaration_list ',' ELLIPSIS           { $$ = tree_new(parameter_declaration_clause, 3, $1, $2, $3); }
   ;


parameter_declaration_list:
     parameter_declaration           { $$ = tree_new(parameter_declaration_list, 1, $1); }
   | parameter_declaration_list ',' parameter_declaration           { $$ = tree_new(parameter_declaration_list, 3, $1, $2, $3); }
   ;


parameter_declaration:
     decl_specifier_seq declarator           { $$ = tree_new(parameter_declaration, 2, $1, $2); }
   | decl_specifier_seq declarator '=' assignment_expression           { $$ = tree_new(parameter_declaration, 4, $1, $2, $3, $4); }
   | decl_specifier_seq abstract_declarator_opt           { $$ = tree_new(parameter_declaration, 2, $1, $2); }
   | decl_specifier_seq abstract_declarator_opt '=' assignment_expression           { $$ = tree_new(parameter_declaration, 4, $1, $2, $3, $4); }
   ;


function_definition:
     declarator ctor_initializer_opt function_body           { $$ = tree_new(function_definition, 3, $1, $2, $3); }
   | decl_specifier_seq declarator ctor_initializer_opt function_body           { $$ = tree_new(function_definition-1, 4, $1, $2, $3, $4); }
   | declarator function_try_block           { $$ = tree_new(function_definition-2, 2, $1, $2); }
   | decl_specifier_seq declarator function_try_block           { $$ = tree_new(function_definition-3, 3, $1, $2, $3); }
   ;


function_body:
     compound_statement           { $$ = tree_new(function_body, 1, $1); }
   ;


initializer:
     '=' initializer_clause           { $$ = tree_new(initializer, 2, $1, $2); }
   | '(' expression_list ')'           { $$ = tree_new(initializer, 3, $1, $2, $3); }
   ;


initializer_clause:
     assignment_expression           { $$ = tree_new(initializer_clause, 1, $1); }
   | '{' initializer_list COMMA_opt '}'           { $$ = tree_new(initializer_clause, 4, $1, $2, $3, $4); }
   | '{' '}'           { $$ = tree_new(initializer_clause, 2, $1, $2); }
   ;


initializer_list:
     initializer_clause           { $$ = tree_new(initializer_list, 1, $1); }
   | initializer_list ',' initializer_clause           { $$ = tree_new(initializer_list, 3, $1, $2, $3); }
   ;

/*----------------------------------------------------------------------
 * Classes.
 *----------------------------------------------------------------------*/

class_specifier:
     class_head '{' member_specification_opt '}'           { $$ = tree_new(class_specifier, 4, $1, $2, $3, $4); }
   ;


class_head:
     class_key identifier           {
      if($2->token->text){
        typenametable_insert($2->token->text, CLASS_NAME);
        $$ = tree_new(class_head, 2, $1, $2); 
      } else {
        yyerror("class identifier could not be added to the typenametable");
      }
    }
   | class_key identifier base_clause           { $$ = tree_new(class_head, 3, $1, $2, $3); }
   | class_key nested_name_specifier identifier           { $$ = tree_new(class_head, 3, $1, $2, $3); }
   | class_key nested_name_specifier identifier base_clause           { $$ = tree_new(class_head, 4, $1, $2, $3, $4); }
   ;


class_key:
     CLASS           { $$ = tree_new(class_key, 1, $1); }
   | STRUCT           { $$ = tree_new(class_key, 1, $1); }
   | UNION           { $$ = tree_new(class_key, 1, $1); }
   ;


member_specification:
     member_declaration member_specification_opt           { $$ = tree_new(member_specification-1, 2, $1, $2); }
   | access_specifier ':' member_specification_opt           { $$ = tree_new(member_specification-2, 3, $1, $2, $3); }
   ;


member_declaration:
     decl_specifier_seq member_declarator_list ';'           { $$ = tree_new(member_declaration, 3, $1, $2, $3); }
   | decl_specifier_seq ';'           { $$ = tree_new(member_declaration, 2, $1, $2); }
   | member_declarator_list ';'           { $$ = tree_new(member_declaration, 2, $1, $2); }
   | ';'           { $$ = tree_new(member_declaration, 1, $1); }
   | function_definition SEMICOLON_opt           { $$ = tree_new(member_declaration, 2, $1, $2); }
   | qualified_id ';'           { $$ = tree_new(member_declaration, 2, $1, $2); }
   | using_declaration           { $$ = tree_new(member_declaration, 1, $1); }
   | template_declaration           { $$ = tree_new(member_declaration, 1, $1); }
   ;


member_declarator_list:
     member_declarator           { $$ = tree_new(member_declarator_list, 1, $1); }
   | member_declarator_list ',' member_declarator           { $$ = tree_new(member_declarator_list, 3, $1, $2, $3); }
   ;


member_declarator:
     declarator           { $$ = tree_new(member_declarator, 1, $1); }
//   | declarator pure_specifier           { $$ = tree_new(member_declarator, 2, $1, $2); }
   | declarator constant_initializer           { $$ = tree_new(member_declarator, 2, $1, $2); }
   | identifier ':' constant_expression           { $$ = tree_new(member_declarator, 3, $1, $2, $3); }
   ;

/*
 * This rule need a hack for working around the ``= 0'' pure specifier.
 * 0 is returned as an ``INTEGER'' by the lexical analyzer but in this
 * context is different.
 
pure_specifier:
     '=' '0'           { $$ = tree_new(pure_specifier, 2, $1, $2); }
   ; */


constant_initializer:
     '=' constant_expression           { $$ = tree_new(constant_initializer, 2, $1, $2); }
   ;
/*----------------------------------------------------------------------
 * Derived classes.
 *----------------------------------------------------------------------*/

base_clause:
     ':' base_specifier_list           { $$ = tree_new(base_clause, 2, $1, $2); }
   ;


base_specifier_list:
     base_specifier           { $$ = tree_new(base_specifier_list, 1, $1); }
   | base_specifier_list ',' base_specifier           { $$ = tree_new(base_specifier_list, 3, $1, $2, $3); }
   ;


base_specifier:
     COLONCOLON nested_name_specifier class_name           { $$ = tree_new(base_specifier, 3, $1, $2, $3); }
   | COLONCOLON class_name           { $$ = tree_new(base_specifier, 2, $1, $2); }
   | nested_name_specifier class_name           { $$ = tree_new(base_specifier, 2, $1, $2); }
   | class_name           { $$ = tree_new(base_specifier, 1, $1); }
   | VIRTUAL access_specifier COLONCOLON nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 5, $1, $2, $3, $4, $5); }
   | VIRTUAL access_specifier nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 4, $1, $2, $3, $4); }
   | VIRTUAL COLONCOLON nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 4, $1, $2, $3, $4); }
   | VIRTUAL nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 3, $1, $2, $3); }
   | access_specifier VIRTUAL COLONCOLON nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 5, $1, $2, $3, $4, $5); }
   | access_specifier VIRTUAL nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 4, $1, $2, $3, $4); }
   | access_specifier COLONCOLON nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 4, $1, $2, $3, $4); }
   | access_specifier nested_name_specifier_opt class_name           { $$ = tree_new(base_specifier, 3, $1, $2, $3); }
   ;


access_specifier:
     PRIVATE           { $$ = tree_new(access_specifier, 1, $1); }
   | PROTECTED           { $$ = tree_new(access_specifier, 1, $1); }
   | PUBLIC           { $$ = tree_new(access_specifier, 1, $1); }
   ;

/*----------------------------------------------------------------------
 * Special member functions.
 *----------------------------------------------------------------------*/

conversion_function_id:
     OPERATOR conversion_type_id           { $$ = tree_new(conversion_function_id, 2, $1, $2); }
   ;


conversion_type_id:
     type_specifier_seq conversion_declarator_opt           { $$ = tree_new(conversion_type_id, 2, $1, $2); }
   ;


conversion_declarator:
     ptr_operator conversion_declarator_opt           { $$ = tree_new(conversion_declarator, 2, $1, $2); }
   ;


ctor_initializer:
     ':' mem_initializer_list           { $$ = tree_new(ctor_initializer, 2, $1, $2); }
   ;


mem_initializer_list:
     mem_initializer           { $$ = tree_new(mem_initializer_list, 1, $1); }
   | mem_initializer ',' mem_initializer_list           { $$ = tree_new(mem_initializer_list, 3, $1, $2, $3); }
   ;


mem_initializer:
     mem_initializer_id '(' expression_list_opt ')'           { $$ = tree_new(mem_initializer, 4, $1, $2, $3, $4); }
   ;


mem_initializer_id:
     COLONCOLON nested_name_specifier class_name           { $$ = tree_new(mem_initializer_id, 3, $1, $2, $3); }
   | COLONCOLON class_name           { $$ = tree_new(mem_initializer_id, 2, $1, $2); }
   | nested_name_specifier class_name           { $$ = tree_new(mem_initializer_id, 2, $1, $2); }
   | class_name           { $$ = tree_new(mem_initializer_id, 1, $1); }
   | identifier           { $$ = tree_new(mem_initializer_id, 1, $1); }
   ;


/*----------------------------------------------------------------------
 * Overloading.
 *----------------------------------------------------------------------*/
operator_function_id:
     OPERATOR operator           { $$ = tree_new(operator_function_id, 2, $1, $2); }
   ;


operator:
     NEW           { $$ = tree_new(operator, 1, $1); }
   | DELETE           { $$ = tree_new(operator, 1, $1); }
   | NEW '[' ']'           { $$ = tree_new(operator, 3, $1, $2, $3); }
   | DELETE '[' ']'           { $$ = tree_new(operator, 3, $1, $2, $3); }
   | '+'           { $$ = tree_new(operator, 1, $1); }
   | '*'           { $$ = tree_new(operator, 1, $1); }
   | '/'           { $$ = tree_new(operator, 1, $1); }
   | '%'           { $$ = tree_new(operator, 1, $1); }
   | '^'           { $$ = tree_new(operator, 1, $1); }
   | '&'           { $$ = tree_new(operator, 1, $1); }
   | '|'           { $$ = tree_new(operator, 1, $1); }
   | '~'           { $$ = tree_new(operator, 1, $1); }
   | '!'           { $$ = tree_new(operator, 1, $1); }
   | '='           { $$ = tree_new(operator, 1, $1); }
   | '<'           { $$ = tree_new(operator, 1, $1); }
   | '>'           { $$ = tree_new(operator, 1, $1); }
   | ADDEQ           { $$ = tree_new(operator, 1, $1); }
   | SUBEQ           { $$ = tree_new(operator, 1, $1); }
   | MULEQ           { $$ = tree_new(operator, 1, $1); }
   | DIVEQ           { $$ = tree_new(operator, 1, $1); }
   | MODEQ           { $$ = tree_new(operator, 1, $1); }
   | XOREQ           { $$ = tree_new(operator, 1, $1); }
   | ANDEQ           { $$ = tree_new(operator, 1, $1); }
   | OREQ           { $$ = tree_new(operator, 1, $1); }
   | SL           { $$ = tree_new(operator, 1, $1); }
   | SR           { $$ = tree_new(operator, 1, $1); }
   | SREQ           { $$ = tree_new(operator, 1, $1); }
   | SLEQ           { $$ = tree_new(operator, 1, $1); }
   | EQ           { $$ = tree_new(operator, 1, $1); }
   | NOTEQ           { $$ = tree_new(operator, 1, $1); }
   | LTEQ           { $$ = tree_new(operator, 1, $1); }
   | GTEQ           { $$ = tree_new(operator, 1, $1); }
   | ANDAND           { $$ = tree_new(operator, 1, $1); }
   | OROR           { $$ = tree_new(operator, 1, $1); }
   | PLUSPLUS           { $$ = tree_new(operator, 1, $1); }
   | MINUSMINUS           { $$ = tree_new(operator, 1, $1); }
   | ','           { $$ = tree_new(operator, 1, $1); }
   | ARROWSTAR           { $$ = tree_new(operator, 1, $1); }
   | ARROW           { $$ = tree_new(operator, 1, $1); }
   | '(' ')'           { $$ = tree_new(operator, 2, $1, $2); }
   | '[' ']'           { $$ = tree_new(operator, 2, $1, $2); }
   ;

/*----------------------------------------------------------------------
 * Templates.
 *----------------------------------------------------------------------*/

template_declaration:
     EXPORT_opt TEMPLATE '<' template_parameter_list '>' declaration           { $$ = tree_new(template_declaration, 6, $1, $2, $3, $4, $5, $6); }
   ;


template_parameter_list:
     template_parameter           { $$ = tree_new(template_parameter_list, 1, $1); }
   | template_parameter_list ',' template_parameter           { $$ = tree_new(template_parameter_list, 3, $1, $2, $3); }
   ;


template_parameter:
     type_parameter           { $$ = tree_new(template_parameter, 1, $1); }
   | parameter_declaration           { $$ = tree_new(template_parameter, 1, $1); }
   ;


type_parameter:
     CLASS identifier           { $$ = tree_new(type_parameter, 2, $1, $2); }
   | CLASS identifier '=' type_id           { $$ = tree_new(type_parameter, 4, $1, $2, $3, $4); }
   | TYPENAME identifier           { $$ = tree_new(type_parameter, 2, $1, $2); }
   | TYPENAME identifier '=' type_id           { $$ = tree_new(type_parameter, 4, $1, $2, $3, $4); }
   | TEMPLATE '<' template_parameter_list '>' CLASS identifier           { $$ = tree_new(type_parameter, 6, $1, $2, $3, $4, $5, $6); }
   | TEMPLATE '<' template_parameter_list '>' CLASS identifier '=' template_name           { $$ = tree_new(type_parameter, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
   ;


template_id:
     template_name '<' template_argument_list '>'           { $$ = tree_new(template_id, 4, $1, $2, $3, $4); }
   ;


template_argument_list:
     template_argument           { $$ = tree_new(template_argument_list, 1, $1); }
   | template_argument_list ',' template_argument           { $$ = tree_new(template_argument_list, 3, $1, $2, $3); }
   ;


template_argument:
     assignment_expression           { $$ = tree_new(template_argument, 1, $1); }
   | type_id           { $$ = tree_new(template_argument, 1, $1); }
   | template_name           { $$ = tree_new(template_argument, 1, $1); }
   ;


explicit_instantiation:
     TEMPLATE declaration           { $$ = tree_new(explicit_instantiation, 2, $1, $2); }
   ;


explicit_specialization:
     TEMPLATE '<' '>' declaration           { $$ = tree_new(explicit_specialization, 4, $1, $2, $3, $4); }
   ;

/*----------------------------------------------------------------------
 * Exception handling.
 *----------------------------------------------------------------------*/

try_block:
     TRY compound_statement handler_seq           { $$ = tree_new(try_block, 3, $1, $2, $3); }
   ;


function_try_block:
     TRY ctor_initializer_opt function_body handler_seq           { sup_error("try_block"); }
   ;


handler_seq:
     handler handler_seq_opt           { $$ = tree_new(handler_seq, 2, $1, $2); }
   ;


handler:
     CATCH '(' exception_declaration ')' compound_statement           { $$ = tree_new(handler, 5, $1, $2, $3, $4, $5); }
   ;


exception_declaration:
     type_specifier_seq declarator           { $$ = tree_new(exception_declaration, 2, $1, $2); }
   | type_specifier_seq abstract_declarator           { $$ = tree_new(exception_declaration, 2, $1, $2); }
   | type_specifier_seq           { $$ = tree_new(exception_declaration, 1, $1); }
   | ELLIPSIS           { $$ = tree_new(exception_declaration, 1, $1); }
   ;


throw_expression:
     THROW assignment_expression_opt           { $$ = tree_new(throw_expression, 2, $1, $2); }
   ;


exception_specification:
     THROW '(' type_id_list_opt ')'           { $$ = tree_new(exception_specification, 4, $1, $2, $3, $4); }
   ;


type_id_list:
     type_id           { $$ = tree_new(type_id_list, 1, $1); }
   | type_id_list ',' type_id           { $$ = tree_new(type_id_list, 3, $1, $2, $3); }
   ;

/*----------------------------------------------------------------------
 * Epsilon (optional) definitions.
 *----------------------------------------------------------------------*/

declaration_seq_opt:
     /* epsilon */          { $$ = NULL; }
   | declaration_seq           { $$ = tree_new(declaration_seq_opt, 1, $1); }
   ;


nested_name_specifier_opt:
     /* epsilon */          { $$ = NULL; }
   | nested_name_specifier           { $$ = tree_new(nested_name_specifier_opt, 1, $1); }
   ;


expression_list_opt:
     /* epsilon */          { $$ = NULL; }
   | expression_list           { $$ = tree_new(expression_list_opt, 1, $1); }
   ;


COLONCOLON_opt:
     /* epsilon */          { $$ = NULL; }
   | COLONCOLON           { $$ = tree_new(COLONCOLON_opt, 1, $1); }
   ;


new_placement_opt:
     /* epsilon */          { $$ = NULL; }
   | new_placement           { $$ = tree_new(new_placement_opt, 1, $1); }
   ;


new_initializer_opt:
     /* epsilon */          { $$ = NULL; }
   | new_initializer           { $$ = tree_new(new_initializer_opt, 1, $1); }
   ;


new_declarator_opt:
     /* epsilon */          { $$ = NULL; }
   | new_declarator           { $$ = tree_new(new_declarator_opt, 1, $1); }
   ;


expression_opt:
     /* epsilon */          { $$ = NULL; }
   | expression           { $$ = tree_new(expression_opt, 1, $1); }
   ;


statement_seq_opt:
     /* epsilon */          { $$ = NULL; }
   | statement_seq           { $$ = tree_new(statement_seq_opt, 1, $1); }
   ;


condition_opt:
     /* epsilon */          { $$ = NULL; }
   | condition           { $$ = tree_new(condition_opt, 1, $1); }
   ;


enumerator_list_opt:
     /* epsilon */          { $$ = NULL; }
   | enumerator_list           { $$ = tree_new(enumerator_list_opt, 1, $1); }
   ;


//initializer_opt:
//     /* epsilon */          { $$ = NULL; }
//   | initializer           { $$ = tree_new(initializer_opt, 1, $1); }
//   ;


constant_expression_opt:
     /* epsilon */          { $$ = NULL; }
   | constant_expression           { $$ = tree_new(constant_expression_opt, 1, $1); }
   ;


abstract_declarator_opt:
     /* epsilon */          { $$ = NULL; }
   | abstract_declarator           { $$ = tree_new(abstract_declarator_opt, 1, $1); }
   ;


type_specifier_seq_opt:
     /* epsilon */          { $$ = NULL; }
   | type_specifier_seq           { $$ = tree_new(type_specifier_seq_opt, 1, $1); }
   ;


direct_abstract_declarator_opt:
     /* epsilon */          { $$ = NULL; }
   | direct_abstract_declarator           { $$ = tree_new(direct_abstract_declarator_opt, 1, $1); }
   ;


ctor_initializer_opt:
     /* epsilon */          { $$ = NULL; }
   | ctor_initializer           { $$ = tree_new(ctor_initializer_opt, 1, $1); }
   ;


COMMA_opt:
     /* epsilon */          { $$ = NULL; }
   | ','           { $$ = tree_new(COMMA_opt, 1, $1); }
   ;


member_specification_opt:
     /* epsilon */          { $$ = NULL; }
   | member_specification           { $$ = tree_new(member_specification_opt, 1, $1); }
   ;


SEMICOLON_opt:
     /* epsilon */          { $$ = NULL; }
   | ';'           { $$ = tree_new(SEMICOLON_opt, 1, $1); }
   ;


conversion_declarator_opt:
     /* epsilon */          { $$ = NULL; }
   | conversion_declarator           { $$ = tree_new(conversion_declarator_opt, 1, $1); }
   ;


EXPORT_opt:
     /* epsilon */          { $$ = NULL; }
   | EXPORT           { $$ = tree_new(EXPORT_opt, 1, $1); }
   ;


handler_seq_opt:
     /* epsilon */          { $$ = NULL; }
   | handler_seq           { $$ = tree_new(handler_seq_opt, 1, $1); }
   ;


assignment_expression_opt:
     /* epsilon */          { $$ = NULL; }
   | assignment_expression           { $$ = tree_new(assignment_expression_opt, 1, $1); }
   ;


type_id_list_opt:
     /* epsilon */          { $$ = NULL; }
   | type_id_list           { $$ = tree_new(type_id_list_opt, 1, $1); }
   ;

%%

static void
yyerror(char *message)
{
  log_syn_error(yylval.treenode->token->filename,yylval.treenode->token->lineno, message,  yylval.treenode->token->text);
}

static void
sup_error(char *message)
{
  log_sup_error(yylval.treenode->token->filename,yylval.treenode->token->lineno, message,  yylval.treenode->token->text);
}