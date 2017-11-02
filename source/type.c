#include "../header/type.h"

#include <stdlib.h>
#include <string.h>

static int BASETYPES_INITIALIZED;

static void type_initialize_basetypes() {
  if (!BASETYPES_INITIALIZED) {
    Type *temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = VOID_T;
    void_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = INT_T;
    int_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = SHORT_T;
    short_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = LONG_T;
    long_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = FLOAT_T;
    float_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = DOUBLE_T;
    double_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = CHAR_T;
    char_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = UNSIGNED_T;
    unsigned_t = temp;
    temp = (Type *)malloc(sizeof(Type));
    temp->basetype = UNKNOWN_T;
    unknown_t = temp;
    // Set flag to initialied
    BASETYPES_INITIALIZED = 1;
  } else {
    return;
  }
}

/**
 * @brief      Get a basetype by name
 *
 * @param[in]  basetype  The basetype
 *
 * @return     { description_of_the_return_value }
 */
Type *type_get_basetype(enum BaseType basetype) {
  switch (basetype) {
    case VOID_T:
      return void_t;
      break;
    case INT_T:
      return int_t;
      break;
    case SHORT_T:
      return short_t;
      break;
    case LONG_T:
      return long_t;
      break;
    case FLOAT_T:
      return float_t;
      break;
    case DOUBLE_T:
      return double_t;
      break;
    case CHAR_T:
      return char_t;
      break;
    case UNSIGNED_T:
      return unsigned_t;
      break;
    case UNKNOWN_T:
    default:
      return unknown_t;
      break;
  }
}

/**
 * @brief      Returns a type that corrilates with the yytokentype that is
 *             passed in. Only basetypes should be passed in. Arrays, Classes,
 *             and Functions should be created using the proper helper function.
 *
 * @param[in]  terminal  The yytokentype of the terminal
 *
 * @return   Returns a pointer to one of the basetypes.
 */
Type *type_from_terminal(enum yytokentype terminal) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  switch (terminal) {
    case VOID:
      return void_t;
    case INT:
      return int_t;
    case SHORT:
      return short_t;
    case LONG:
      return long_t;
    case FLOAT:
      return float_t;
    case DOUBLE:
      return double_t;
    case CHAR:
      return char_t;
    case UNSIGNED:
      return unsigned_t;
    default:
      return unknown_t;
      break;
  }
}

Type *type_new(enum BaseType basetype) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
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

Type *type_new_function(Type *returntype) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = FUNCTION_T;
  newtype->info.function.status = FUNC_NEW;
  newtype->info.function.returntype = returntype;
  newtype->info.function.nparams = 0;
  newtype->info.function.parameters = (Parameter **)malloc(sizeof(Parameter *) * 256);
  newtype->info.function.symtab = NULL;
  return newtype;
}

Type *type_new_class(char *name) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = CLASS_T;
  newtype->info.class.name = strdup(name);
  newtype->info.class.nFields = 0;
  newtype->info.class.status = CLASS_NEW;
  newtype->info.class.public = NULL;
  newtype->info.class.private = NULL;
  return newtype;
}

Type *type_new_class_instance(Type *classtype) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = CLASS_INSTANCE_T;
  newtype->info.classinstance.classtype = classtype;
  return newtype;
}

Type *type_new_array(Type *type, int size) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = ARRAY_T;
  newtype->info.array.size = 0;
  newtype->info.array.type = NULL;
  return newtype;
}

Parameter *type_new_parameter(Type *type) {
  Parameter *newparam = (Parameter *)malloc(sizeof(Parameter));
  newparam->type = type;
  return newparam;
}

TypeCompareResults type_compare(Type *type1, Type *type2) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  if (!type1 || !type2)
    return TYPE_NULL_PARAMETERS;
  else {
    switch (type1->basetype) {
      case CLASS_INSTANCE_T: {
        if (type1->info.classinstance.classtype == type2->info.classinstance.classtype)
          return TYPE_EQUAL;
        else
          return TYPE_NOT_EQUAL;
      }
      break;
      case ARRAY_T: {
        if (type1->info.array.type == type2->info.array.type
            && type1->info.array.size == type2->info.array.size)
          return TYPE_EQUAL;
        else
          return TYPE_NOT_EQUAL;
      }
      break;
      default: {
        if (type1 == type2)
          return TYPE_EQUAL;
        else
          return TYPE_NOT_EQUAL;
      }
      break;
    }
  }
}