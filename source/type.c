#include "../header/type.h"

#include <stdlib.h>

int type_from_terminal(int terminal) {
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
    return CLASS_INSTANCE_T;
  default:
    return UNKNOWN_T;
    break;
  }
}


Type *type_new(enum BaseType basetype) {
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = basetype;
  return newtype;
}

Parameter *type_new_parameter(Type *type) {
  Parameter *newparam = (Parameter *)malloc(sizeof(Parameter));
  newparam->type = type;
  newparam->next = NULL;
  return newparam;
}