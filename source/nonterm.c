#include "../header/nonterm.h"

char *humanReadable(int nonterm) {

  switch (nonterm) {
  // case 0:
  //   return strdup("Terminal");
  //   break;
  case typedef_name:
    return strdup("typedef_name");
    break;
  case class_name:
    return strdup("class_name");
    break;
  case template_name:
    return strdup("template_name");
    break;
  case identifier:
    return strdup("identifier");
    break;
  case literal:
    return strdup("literal");
    break;
  case integer_literal:
    return strdup("integer_literal");
    break;
  case character_literal:
    return strdup("character_literal");
    break;
  case floating_literal:
    return strdup("floating_literal");
    break;
  case string_literal:
    return strdup("string_literal");
    break;
  case boolean_literal:
    return strdup("boolean_literal");
    break;
  case translation_unit:
    return strdup("translation_unit");
    break;
  case primary_expression:
    return strdup("primary_expression");
    break;
  case id_expression:
    return strdup("id_expression");
    break;
  case unqualified_id:
    return strdup("unqualified_id");
    break;
  case qualified_id:
    return strdup("qualified_id");
    break;
  case nested_name_specifier:
    return strdup("nested_name_specifier");
    break;
  case postfix_expression:
    return strdup("postfix_expression");
    break;
  case expression_list:
    return strdup("expression_list");
    break;
  case unary_expression:
    return strdup("unary_expression");
    break;
  case unary_operator:
    return strdup("unary_operator");
    break;
  case new_expression:
    return strdup("new_expression");
    break;
  case new_placement:
    return strdup("new_placement");
    break;
  case new_type_id:
    return strdup("new_type_id");
    break;
  case new_declarator:
    return strdup("new_declarator");
    break;
  case direct_new_declarator:
    return strdup("direct_new_declarator");
    break;
  case new_initializer:
    return strdup("new_initializer");
    break;
  case delete_expression:
    return strdup("delete_expression");
    break;
  case cast_expression:
    return strdup("cast_expression");
    break;
  case pm_expression:
    return strdup("pm_expression");
    break;
  case multiplicative_expression:
    return strdup("multiplicative_expression");
    break;
  case additive_expression:
    return strdup("additive_expression");
    break;
  case shift_expression:
    return strdup("shift_expression");
    break;
  case relational_expression:
    return strdup("relational_expression");
    break;
  case equality_expression:
    return strdup("equality_expression");
    break;
  case and_expression:
    return strdup("and_expression");
    break;
  case exclusive_or_expression:
    return strdup("exclusive_or_expression");
    break;
  case inclusive_or_expression:
    return strdup("inclusive_or_expression");
    break;
  case logical_and_expression:
    return strdup("logical_and_expression");
    break;
  case logical_or_expression:
    return strdup("logical_or_expression");
    break;
  case conditional_expression:
    return strdup("conditional_expression");
    break;
  case assignment_expression:
    return strdup("assignment_expression");
    break;
  case assignment_operator:
    return strdup("assignment_operator");
    break;
  case expression:
    return strdup("expression");
    break;
  case constant_expression:
    return strdup("constant_expression");
    break;
  case statement:
    return strdup("statement");
    break;
  case labeled_statement:
    return strdup("labeled_statement");
    break;
  case expression_statement:
    return strdup("expression_statement");
    break;
  case compound_statement:
    return strdup("compound_statement");
    break;
  case statement_seq:
    return strdup("statement_seq");
    break;
  case selection_statement:
    return strdup("selection_statement");
    break;
  case condition:
    return strdup("condition");
    break;
  case iteration_statement:
    return strdup("iteration_statement");
    break;
  case for_init_statement:
    return strdup("for_init_statement");
    break;
  case jump_statement:
    return strdup("jump_statement");
    break;
  case declaration_statement:
    return strdup("declaration_statement");
    break;
  case declaration_seq:
    return strdup("declaration_seq");
    break;
  case declaration:
    return strdup("declaration");
    break;
  case block_declaration:
    return strdup("block_declaration");
    break;
  case simple_declaration:
    return strdup("simple_declaration");
    break;
  case decl_specifier:
    return strdup("decl_specifier");
    break;
  case decl_specifier_seq:
    return strdup("decl_specifier_seq");
    break;
  case storage_class_specifier:
    return strdup("storage_class_specifier");
    break;
  case function_specifier:
    return strdup("function_specifier");
    break;
  case type_specifier:
    return strdup("type_specifier");
    break;
  case simple_type_specifier:
    return strdup("simple_type_specifier");
    break;
  case type_name:
    return strdup("type_name");
    break;
  case elaborated_type_specifier:
    return strdup("elaborated_type_specifier");
    break;
  case enum_name:
    return strdup("enum_name");
    break;
  case enum_specifier:
    return strdup("enum_specifier");
    break;
  case enumerator_list:
    return strdup("enumerator_list");
    break;
  case enumerator_definition:
    return strdup("enumerator_definition");
    break;
  case enumerator:
    return strdup("enumerator");
    break;
  case namespace_name:
    return strdup("namespace_name");
    break;
  case original_namespace_name:
    return strdup("original_namespace_name");
    break;
  case namespace_definition:
    return strdup("namespace_definition");
    break;
  case named_namespace_definition:
    return strdup("named_namespace_definition");
    break;
  case original_namespace_definition:
    return strdup("original_namespace_definition");
    break;
  case extension_namespace_definition:
    return strdup("extension_namespace_definition");
    break;
  case unnamed_namespace_definition:
    return strdup("unnamed_namespace_definition");
    break;
  case namespace_body:
    return strdup("namespace_body");
    break;
  case namespace_alias:
    return strdup("namespace_alias");
    break;
  case namespace_alias_definition:
    return strdup("namespace_alias_definition");
    break;
  case qualified_namespace_specifier:
    return strdup("qualified_namespace_specifier");
    break;
  case using_declaration:
    return strdup("using_declaration");
    break;
  case using_directive:
    return strdup("using_directive");
    break;
  case asm_definition:
    return strdup("asm_definition");
    break;
  case linkage_specification:
    return strdup("linkage_specification");
    break;
  case init_declarator_list:
    return strdup("init_declarator_list");
    break;
  case init_declarator:
    return strdup("init_declarator");
    break;
  case declarator:
    return strdup("declarator");
    break;
  case direct_declarator:
    return strdup("direct_declarator");
    break;
  case ptr_operator:
    return strdup("ptr_operator");
    break;
  case cv_qualifier_seq:
    return strdup("cv_qualifier_seq");
    break;
  case cv_qualifier:
    return strdup("cv_qualifier");
    break;
  case declarator_id:
    return strdup("declarator_id");
    break;
  case type_id:
    return strdup("type_id");
    break;
  case type_specifier_seq:
    return strdup("type_specifier_seq");
    break;
  case abstract_declarator:
    return strdup("abstract_declarator");
    break;
  case direct_abstract_declarator:
    return strdup("direct_abstract_declarator");
    break;
  case parameter_declaration_clause:
    return strdup("parameter_declaration_clause");
    break;
  case parameter_declaration_list:
    return strdup("parameter_declaration_list");
    break;
  case parameter_declaration:
    return strdup("parameter_declaration");
    break;
  case function_definition:
    return strdup("function_definition");
    break;
  case function_body:
    return strdup("function_body");
    break;
  case initializer:
    return strdup("initializer");
    break;
  case initializer_clause:
    return strdup("initializer_clause");
    break;
  case initializer_list:
    return strdup("initializer_list");
    break;
  case class_specifier:
    return strdup("class_specifier");
    break;
  case class_head:
    return strdup("class_head");
    break;
  case class_key:
    return strdup("class_key");
    break;
  case member_specification:
    return strdup("member_specification");
    break;
  case member_declaration:
    return strdup("member_declaration");
    break;
  case member_declarator_list:
    return strdup("member_declarator_list");
    break;
  case member_declarator:
    return strdup("member_declarator");
    break;
  case pure_specifier:
    return strdup("pure_specifier");
    break;
  case constant_initializer:
    return strdup("constant_initializer");
    break;
  case base_clause:
    return strdup("base_clause");
    break;
  case base_specifier_list:
    return strdup("base_specifier_list");
    break;
  case base_specifier:
    return strdup("base_specifier");
    break;
  case access_specifier:
    return strdup("access_specifier");
    break;
  case conversion_function_id:
    return strdup("conversion_function_id");
    break;
  case conversion_type_id:
    return strdup("conversion_type_id");
    break;
  case conversion_declarator:
    return strdup("conversion_declarator");
    break;
  case ctor_initializer:
    return strdup("ctor_initializer");
    break;
  case mem_initializer_list:
    return strdup("mem_initializer_list");
    break;
  case mem_initializer:
    return strdup("mem_initializer");
    break;
  case mem_initializer_id:
    return strdup("mem_initializer_id");
    break;
  case operator_function_id:
    return strdup("operator_function_id");
    break;
  case operator:
    return strdup("operator");
    break;
  case template_declaration:
    return strdup("template_declaration");
    break;
  case template_parameter_list:
    return strdup("template_parameter_list");
    break;
  case template_parameter:
    return strdup("template_parameter");
    break;
  case type_parameter:
    return strdup("type_parameter");
    break;
  case template_id:
    return strdup("template_id");
    break;
  case template_argument_list:
    return strdup("template_argument_list");
    break;
  case template_argument:
    return strdup("template_argument");
    break;
  case explicit_instantiation:
    return strdup("explicit_instantiation");
    break;
  case explicit_specialization:
    return strdup("explicit_specialization");
    break;
  case try_block:
    return strdup("try_block");
    break;
  case function_try_block:
    return strdup("function_try_block");
    break;
  case handler_seq:
    return strdup("handler_seq");
    break;
  case handler:
    return strdup("handler");
    break;
  case exception_declaration:
    return strdup("exception_declaration");
    break;
  case throw_expression:
    return strdup("throw_expression");
    break;
  case exception_specification:
    return strdup("exception_specification");
    break;
  case type_id_list:
    return strdup("type_id_list");
    break;
  case declaration_seq_opt:
    return strdup("declaration_seq_opt");
    break;
  case nested_name_specifier_opt:
    return strdup("nested_name_specifier_opt");
    break;
  case expression_list_opt:
    return strdup("expression_list_opt");
    break;
  case COLONCOLON_opt:
    return strdup("COLONCOLON_opt");
    break;
  case new_placement_opt:
    return strdup("new_placement_opt");
    break;
  case new_initializer_opt:
    return strdup("new_initializer_opt");
    break;
  case new_declarator_opt:
    return strdup("new_declarator_opt");
    break;
  case expression_opt:
    return strdup("expression_opt");
    break;
  case statement_seq_opt:
    return strdup("statement_seq_opt");
    break;
  case condition_opt:
    return strdup("condition_opt");
    break;
  case enumerator_list_opt:
    return strdup("enumerator_list_opt");
    break;
  case initializer_opt:
    return strdup("initializer_opt");
    break;
  case constant_expression_opt:
    return strdup("constant_expression_opt");
    break;
  case abstract_declarator_opt:
    return strdup("abstract_declarator_opt");
    break;
  case type_specifier_seq_opt:
    return strdup("type_specifier_seq_opt");
    break;
  case direct_abstract_declarator_opt:
    return strdup("direct_abstract_declarator_opt");
    break;
  case ctor_initializer_opt:
    return strdup("ctor_initializer_opt");
    break;
  case COMMA_opt:
    return strdup("COMMA_opt");
    break;
  case member_specification_opt:
    return strdup("member_specification_opt");
    break;
  case SEMICOLON_opt:
    return strdup("SEMICOLON_opt");
    break;
  case conversion_declarator_opt:
    return strdup("conversion_declarator_opt");
    break;
  case EXPORT_opt:
    return strdup("EXPORT_opt");
    break;
  case handler_seq_opt:
    return strdup("handler_seq_opt");
    break;
  case assignment_expression_opt:
    return strdup("assignment_expression_opt");
    break;
  case type_id_list_opt:
    return strdup("type_id_list_opt");
    break;
  default:
    return strdup("ERROR: BAD INPUT TO HUMANREADABLE");
    break;
  }
}