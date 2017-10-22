#include "../header/type.h"

#include <stdlib.h>

BaseType type_from_terminal(enum yytokentype terminal) {
  switch (terminal) {
    case CHAR:
      return CHAR_T;
      break;
    case DOUBLE:
      return DOUBLE_T;
      break;
    case FLOAT:
      return FLOAT_T;
      break;
    case INT:
      return INT_T;
      break;
    case LONG:
      return LONG_T;
      break;
    case SHORT:
      return SHORT_T;
      break;
    case UNSIGNED:
      return UNSIGNED_T;
      break;
    case VOID:
      return VOID_T;
      break;
    case IDENTIFIER:
      return CLASS_T;
    default:
      return UNKNOWN_T;
      break;
  }
}

Type *type_new(enum BaseType basetype) {
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = basetype;
  switch (basetype) { /* initialize type specific info union */
    case FUNCTION_T:
      newtype->info.function.status = FUNC_NEW;
      newtype->info.function.returntype = NULL;
      newtype->info.function.nparams = 0;
      newtype->info.function.parameters = (Parameter **)malloc(sizeof(Parameter *) * 256);
      newtype->info.function.symtab = NULL;
      break;
    case CLASS_T:
      newtype->info.class.name = NULL;
      newtype->info.class.nFields = 0;
      newtype->info.class.status = CLASS_NEW;
      newtype->info.class.public = NULL;
      newtype->info.class.private = NULL;
      break;
    case CLASS_INSTANCE_T:
      newtype->info.classinstance.classtype = NULL;
      break;
    case ARRAY_T:
      newtype->info.array.size = 0;
      newtype->info.array.type = NULL;
      break;
  }
  return newtype;
}

Parameter *type_new_parameter(Type *type) {
  Parameter *newparam = (Parameter *)malloc(sizeof(Parameter));
  newparam->type = type;
  return newparam;
}

TypeCompareResults type_compare(Type *type1, Type *type2) {
  if (!type1 || !type2)
    return TYPE_NULL_PARAMETERS;
  else {
    switch (type1->basetype) {
      // case CLASS_T:
      // case CLASS_INSTANCE_T:
      // case FUNCTION_T:
      // case UNKNOWN_T:
      default:
        if (type1->basetype == type2->basetype)
          return TYPE_EQUAL;
        else
          return TYPE_NOT_EQUAL;
        break;
    }
  }
}