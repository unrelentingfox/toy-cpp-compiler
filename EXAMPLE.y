%{
   
#include <stdio.h>
#include <string.h>

/* include lots of your include files here */

# define YYDEBUG 1
/* #define DEBUG */

extern int yyerror(char *);
extern int yylex(void);

/* declare externs for global variables and helper function prototypes */

%}

%union {
struct token *tokptr;
struct tree *treeptr;
}

%token BAD_TOKEN
%token ICON CCON FCON 
%token ENUMERATION_CONSTANT IDENTIFIER STRING
%token SIZEOF
%token INCOP DECOP SHL SHR LE GE EQ NE
%token ANDAND OROR MUASN DIASN MOASN PLASN ASN
%token MIASN SLASN SRASN ANASN
%token ERASN ORASN TYPEDEF_NAME
%token CM SM LT GT PLUS MINUS MUL DIV MOD LP RP LB RB LC RC COLON
%token QUEST AND OR ER NOT FOLLOW BANG DOT
%token SP DTAB ITAB EOL TAB ECNT

%token TYPEDEF EXTERN STATIC AUTO REGISTER 
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELIPSIS

%token CASE DEFAULT IF SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

/* Lowest Precedence */
%left  THEN
%left  ELSE

%left CM
%right ASN PLASN MIASN MUASN DIASN MOASN SRASN SLASN ANASN ERASN ORASN
%left OROR
%left ANDAND
%left OR
%left ER
%left AND
%left EQ NE
%left GT GE LT LE
%left SHL SHR
%left PLUS MINUS
%left MUL DIV MOD
%left BANG NOT INCOP DECOP SIZEOF
/* Highest Precedence */

%start file

%type <treeptr>    identifier
%type <treeptr>        file
%type <treeptr>    translation_unit
%type <treeptr>    external_declaration
%type <treeptr>    function_definition
%type <treeptr>    declaration
%type <treeptr>    untyped_declaration
%type <treeptr>    declaration_list
%type <treeptr>    declaration_specifiers
%type <treeptr>    storage_class_specifier
%type <treeptr>    type_specifier
%type <treeptr>    actual_type_specifier
%type <treeptr>    type_adjective
%type <treeptr>    type_qualifier
%type <treeptr>    struct_or_union_specifier
%type <treeptr>    struct_or_union
%type <treeptr>    struct_declaration_list
%type <treeptr>    init_declarator_list
%type <treeptr>    init_declarator
%type <treeptr>    struct_declaration
%type <treeptr>    specifier_qualifier_list
%type <treeptr>    struct_declarator_list
%type <treeptr>    struct_declarator
%type <treeptr>    enum_specifier
%type <treeptr>    enumerator_list
%type <treeptr>    enumerator
%type <treeptr>    declarator
%type <treeptr>    direct_declarator
%type <treeptr>    function_declarator
%type <treeptr>    direct_function_declarator
%type <treeptr>    pointer
%type <treeptr>    type_qualifier_list
%type <treeptr>    parameter_type_list
%type <treeptr>    parameter_list
%type <treeptr>    parameter_declaration
%type <treeptr>    identifier_list
%type <treeptr>    initializer
%type <treeptr>    initializer_list
%type <treeptr>    type_name
%type <treeptr>    abstract_declarator
%type <treeptr>    direct_abstract_declarator
%type <treeptr>    statement
%type <treeptr>    labeled_statement
%type <treeptr>    expression_statement
%type <treeptr>    compound_statement
%type <treeptr>    compound_statement_opt
%type <treeptr>    statement_list
%type <treeptr>    selection_statement
%type <treeptr>    iteration_statement
%type <treeptr>    forcntrl
%type <treeptr>    jump_statement
%type <treeptr>    expression
%type <treeptr>    assignment_expression
%type <treeptr>    assignment_operator
%type <treeptr>    conditional_expression
%type <treeptr>    constant_expression
%type <treeptr>    logical_or_expression
%type <treeptr>    logical_and_expression
%type <treeptr>    inclusive_or_expression
%type <treeptr>    exclusive_or_expression
%type <treeptr>    and_expression
%type <treeptr>    equality_expression
%type <treeptr>    relational_expression
%type <treeptr>    shift_expression
%type <treeptr>    additive_expression
%type <treeptr>    multiplicative_expression
%type <treeptr>    cast_expression
%type <treeptr>    unary_expression
%type <treeptr>    unary_operator
%type <treeptr>    postfix_expression
%type <treeptr>    primary_expression
%type <treeptr>    argument_expression_list
%type <treeptr>    constant


%%

identifier      /* 1000 */
    : IDENTIFIER
      {
      }
    | ENUMERATION_CONSTANT
      {
      }
    ;

file
    : translation_unit
      {
      }
    ;

translation_unit: 
    external_declaration { }
    | translation_unit external_declaration
    { }
    ;

/* "global" things. Does not imply that "extern" is present. */
external_declaration:  
   function_definition
   { 
   }
   |  declaration 
   { }
   |  untyped_declaration
   { }
   ;

function_definition:
      function_declarator 
      { /* old school pre-ANSI, no return type */
      } 
      compound_statement
      {
      }

    | function_declarator declaration_list 
      { /* old school pre ANSI */
      } 
      compound_statement
      {
      }

    | declaration_specifiers function_declarator 
      { /* proper ANSI C function definition */
      } 
      compound_statement 
      {
      }

    | declaration_specifiers function_declarator  declaration_list
      { /* declaration_list! This must be pre-ANSI  */
      } 
      compound_statement
      {
      }
;

declaration     /* 1080 */
	: declaration_specifiers SM
          {
	  }
	| declaration_specifiers  
          init_declarator_list SM
	  {
	  }
	;

untyped_declaration
	 : init_declarator_list SM
	    {
            }
	 ;

declaration_list
	: declaration { }
	| declaration_list declaration
	  { 
	  }
	;

declaration_specifiers
	: storage_class_specifier 
          /* defines TYPEDEF,EXTERN,STATIC,AUTO,REGISTER*/
	| storage_class_specifier declaration_specifiers
          {
          }
	| type_specifier 
	| type_specifier 
          declaration_specifiers
	  { 
          }
	| type_qualifier  
	| type_qualifier declaration_specifiers
          {  
          }
	;


storage_class_specifier
	/* here we will know the storage specifier */

	:  TYPEDEF  /* indicate typedef is seen */
	   {
           }
	 | EXTERN
  	   {
           }
	 | STATIC
	   {
           }
	 | AUTO
           {
           }
	 | REGISTER
	   {
           }
	;

type_specifier
           :  actual_type_specifier /* build ctype */
	   | type_adjective 
	   ;

actual_type_specifier
	: VOID
	{
        }	
        | CHAR
        {
        }
	| INT
	{
        }
	| FLOAT
        {
        }
        | DOUBLE
        {
        }
        | TYPEDEF_NAME
	{
        }
	| struct_or_union_specifier
	| enum_specifier
	;

type_adjective    /* Build ctype for all */
	 : SHORT
           {
           }
	 | LONG
           {
           }
	 | SIGNED
           {
           }
	 | UNSIGNED
           {
           }
	 ;

type_qualifier
	: CONST
          {
          } 
	 | VOLATILE
	  {
          } 
	;

struct_or_union_specifier
    : struct_or_union
    LC { } struct_declaration_list RC
    {
    }

    | struct_or_union identifier LC { } struct_declaration_list RC
    {
    }
    | struct_or_union identifier
    {
    }
    ;

struct_or_union
    : STRUCT
      {  } /* build ctype */
    | UNION
      {  }  /* build ctype */
    ;

struct_declaration_list
	: struct_declaration 
        {  }
	| struct_declaration_list struct_declaration
		{  }
	;

init_declarator_list    /* 1320 */

	: init_declarator 
        /* an identifier has been found and added to the symbol table*/
        /* so reset ctype to be base type of declaration */

	| init_declarator_list CM init_declarator
        { 
        }
	;

init_declarator         /* 1340 */
	: declarator { $$ = maketree(INIT_DECLARATOR_RULE_1, 1, $1); }
	| declarator ASN initializer
          { $$ = maketree(INIT_DECLARATOR_RULE_2, 3, $1, $2, $3); }
	;

struct_declaration
	: specifier_qualifier_list SM
	  {
	  }
	|  specifier_qualifier_list struct_declarator_list  SM
	  {
          }
	;

specifier_qualifier_list
	: type_specifier /* ctype build */
	| type_specifier specifier_qualifier_list  /* ctype build */
		{
		}
	| type_qualifier /* ctype build */
	| type_qualifier specifier_qualifier_list  /* ctype build */
		{
		}
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list CM
	  struct_declarator
		{
		}
	;

struct_declarator
	: declarator
	| COLON constant_expression
		{
		}
	| declarator COLON constant_expression
		{
		}
	;

enum_specifier
	: ENUM LC enumerator_list RC
		{
		}
	| ENUM identifier LC enumerator_list RC
		{
		}
	| ENUM identifier
		{
		}
	;

enumerator_list
	: enumerator
	| enumerator_list CM enumerator
		{
		}
	;

enumerator
	: IDENTIFIER
		{
		}
	| IDENTIFIER
		{
		}
		ASN constant_expression
		{
		}
	;

declarator              /* 1500 */
	: direct_declarator
        {
	}
	| pointer direct_declarator
        {
	}
	;

direct_declarator   /* 1520 */
	: identifier  /* this could be a TRUE identifier or a TYPEDEF identifier*/
	{
        }
	| LP declarator RP
        {
	}
	| direct_declarator LB RB
        {
        }
	| direct_declarator LB constant_expression RB
        {
        }
        | direct_declarator LP parameter_type_list RP
        {
	}
	| direct_declarator  LP RP
        {
	}
	| direct_declarator  LP identifier_list RP
        {
	}
        ;

function_declarator : 
    direct_function_declarator 
    | pointer direct_function_declarator
    {
    }
    ;

direct_function_declarator
    : direct_declarator LP parameter_type_list RP
      {
      }
    | direct_declarator LP RP
      {
      }
    | direct_declarator LP identifier_list RP
      { /* pre-ANSI, error case */
      }
    ;

pointer
    : MUL
      {
      }
    | MUL 
    type_qualifier_list
      {
      }
    | MUL pointer
      {
      }
    | MUL type_qualifier_list pointer
      {
      }
    ;

type_qualifier_list
    : type_qualifier
    | type_qualifier_list type_qualifier
    {
    }
    ;

parameter_type_list
    : parameter_list { }
    | parameter_list CM ELIPSIS
    {
    }
    ;

parameter_list
    : parameter_declaration  { }
    | parameter_list CM parameter_declaration
      {
      }
    ;

parameter_declaration
    : declaration_specifiers declarator
      { }
    |    declaration_specifiers
      { }
    |    declaration_specifiers abstract_declarator
      { }
    ;

identifier_list
	: IDENTIFIER
		{ }
	| identifier_list CM IDENTIFIER
		{ }
	;

initializer
	: assignment_expression
	| LC initializer_list RC
		{ }
	| LC initializer_list CM RC
		{ }
	;

initializer_list
	: initializer
	| initializer_list CM initializer
		{ }
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list   abstract_declarator
          { }
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
		{
		}
	;

direct_abstract_declarator
	: LP abstract_declarator RP
		{
		}
	| LB RB
		{
		}
	| LB constant_expression RB
		{
		}
	| direct_abstract_declarator LB RB
		{
		}
	| direct_abstract_declarator LB constant_expression RB
		{
		}
	| LP RP
		{
		}
	| LP parameter_type_list RP
		{
		}
	| direct_abstract_declarator LP RP
		{
		}
	| direct_abstract_declarator LP parameter_type_list RP
		{
		}
	;

statement: 
        labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement: 
    identifier COLON statement
    {
    }
    | CASE constant_expression COLON statement
      { 
      }
    | DEFAULT COLON statement
      {
      }
    ;

expression_statement
	: SM
		{
		}
	| expression SM
		{
		}
	;

compound_statement:
    LC  RC
    {
    }
    | LC 
      {
      }
      compound_statement_opt RC 
      {
      } 
    ;

compound_statement_opt:
    statement_list 
    {
    }

    | declaration_list
    {
    }

    | declaration_list statement_list 
    { 
     
    }

;

statement_list
	: statement
	| statement_list statement
		{
		}
	;

selection_statement: 
   IF LP expression RP statement %prec THEN
      {   

      }
   | IF LP expression RP statement ELSE statement
     {
     }

   | SWITCH LP expression RP statement
     {
     }
   ;

iteration_statement: 
    WHILE LP expression RP statement
    {

    }
    | DO statement WHILE LP expression RP SM
    {
    }		

    | FOR LP forcntrl RP statement
      { 
      }
     ;

forcntrl
     : SM SM
     {
     }
     | SM SM expression
     {
     }
     | SM expression  SM
     {
     }
     | SM expression  SM expression
     {
     }
     | expression  SM SM
     {
     }
     | expression  SM SM expression
     {
     }
     | expression  SM expression  SM
     {
     }
     | expression  SM expression  SM expression
     {
     }
     ;



jump_statement:
  GOTO identifier SM
  {
  }
  | CONTINUE SM
  {
  }
  | BREAK SM
  {
  }
  | RETURN SM
  {
  }
  | RETURN expression SM
  {
  }
  ;

expression
	: assignment_expression
		{
		}
	| expression CM assignment_expression
		{
		}
	;


assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
          {
          }
	;

assignment_operator
	: ASN   { }
	| MUASN { }
	| DIASN { }
	| MOASN { }
	| PLASN { }
	| MIASN { }
	| SLASN { }
	| SRASN { }
	| ANASN { }
	| ERASN { }
	| ORASN { }
	;


conditional_expression
	: logical_or_expression
	| logical_or_expression QUEST expression COLON conditional_expression
		{
		}
	;


constant_expression
	: conditional_expression
	;


logical_or_expression
	: logical_and_expression
	| logical_or_expression OROR logical_and_expression
		{
                }
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression ANDAND inclusive_or_expression
		{ }
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression OR exclusive_or_expression
		{
		}
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression ER and_expression
		{
		}
	;

and_expression
	: equality_expression
	| and_expression AND equality_expression
		{
		}
	;

equality_expression
	: relational_expression
	| equality_expression EQ relational_expression
		{
	        }
	| equality_expression NE relational_expression
		{
                }			   
	;

relational_expression:
   shift_expression
   | relational_expression LT shift_expression
     {
     }
   | relational_expression GT shift_expression
     {
     }
   | relational_expression LE shift_expression
     {
     }
   | relational_expression GE shift_expression
     {
     }
   ;

shift_expression:
   additive_expression
   | shift_expression SHL additive_expression
     {
     }
   | shift_expression SHR additive_expression
     {
     }
     ;

additive_expression: 
   multiplicative_expression
   | additive_expression PLUS multiplicative_expression
     {
     }
   | additive_expression MINUS multiplicative_expression
     {
     }
   ;

multiplicative_expression:
   cast_expression
   | multiplicative_expression MUL cast_expression
     {
     }
   | multiplicative_expression DIV cast_expression
     {
     }
   | multiplicative_expression MOD cast_expression
     {
     }
   ;

cast_expression:
   unary_expression
   | LP type_name RP cast_expression
   {
   }
   ;


unary_expression:        
   postfix_expression
   | INCOP unary_expression
   {
   }
   | DECOP unary_expression
   {
   }
   | unary_operator cast_expression
   {
   }
   | SIZEOF unary_expression
   {
   }
   | SIZEOF LP type_name RP
   {  
   }
   ;

unary_operator:
   AND
   {
   }
   | MUL
   {
   }
   | PLUS
   {
   }
   | MINUS
   {
   }
   | NOT
   {
   }
   | BANG
   {
   }
   ;


postfix_expression:      
   primary_expression
   | postfix_expression LB expression RB
     {
     }
   | postfix_expression LP RP
     {
     }
   | postfix_expression LP  argument_expression_list RP
     {
     }
   | postfix_expression DOT identifier
     {
     }
   | postfix_expression FOLLOW identifier
     {
     }
   | postfix_expression INCOP
     {
     }
   | postfix_expression DECOP
     {
     }
   ;

primary_expression: 
   IDENTIFIER
   { 
   }
   | constant
   | STRING
     {
     }
   | LP expression RP
   { 
   }
   ;

argument_expression_list: 
   assignment_expression 
   {
   }
   | argument_expression_list CM assignment_expression
   {
   }
   ;

constant:
   ICON
   {
   }
   | CCON
   {
   }
   | FCON
   {
   }
   | ENUMERATION_CONSTANT
   {
   }
   ;

%%
