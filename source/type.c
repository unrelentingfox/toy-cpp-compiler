#include "../header/type.h"
#include "../header/logger.h"
#include <stdlib.h>
#include <string.h>

#define PARAM_LIMIT 22

static int BASETYPES_INITIALIZED;

static void type_initialize_basetypes() {
  if (!BASETYPES_INITIALIZED) {
    void_t = type_initialize_basetype(VOID_T, 0);
    int_t = type_initialize_basetype(INT_T, 8);
    short_t = type_initialize_basetype(SHORT_T, 8);
    long_t = type_initialize_basetype(LONG_T, 8);
    float_t = type_initialize_basetype(FLOAT_T, 8);
    double_t = type_initialize_basetype(DOUBLE_T, 8);
    char_t = type_initialize_basetype(CHAR_T, 8);
    unsigned_t = type_initialize_basetype(UNSIGNED_T, 8);
    // Set flag to initialied
    BASETYPES_INITIALIZED = 1;
  } else {
    return;
  }
}

static Type *type_initialize_basetype(enum BaseType basetype, int size) {
  Type *temp;
  temp = (Type *)malloc(sizeof(Type));
  temp->basetype = basetype;
  temp->size = size;
  return temp;
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

char *type_to_string(Type *type) {
  if (!type) {
    return strdup("(NULL)");
  }
  switch (type->basetype) {
    case VOID_T:
      return strdup("Void");
      break;
    case INT_T:
      return strdup("Int");
      break;
    case SHORT_T:
      return strdup("Short");
      break;
    case LONG_T:
      return strdup("Long");
      break;
    case FLOAT_T:
      return strdup("Float");
      break;
    case DOUBLE_T:
      return strdup("Double");
      break;
    case CHAR_T:
      return strdup("Char");
      break;
    case UNSIGNED_T:
      return strdup("Unsigned");
      break;
    case CLASS_T:
      return strdup("Class");;
      break;
    case CLASS_INSTANCE_T: {
      LOG_ASSERT(type->info.classinstance.classtype);
      LOG_ASSERT(type->info.classinstance.classtype->info.class.name);
      return strdup(type->info.classinstance.classtype->info.class.name);
    }
    break;
    case FUNCTION_T:
      return strdup("Function");
      break;
    case ARRAY_T:
      return strdup("Array");
      break;
    case UNKNOWN_T:
    default:
      return strdup("Unknown");
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

Type *type_new_function(Type *returntype) {
  if (!returntype)
    returntype = type_get_basetype(VOID_T);
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = (Type *)malloc(sizeof(Type));
  newtype->basetype = FUNCTION_T;
  newtype->info.function.status = FUNC_NEW;
  newtype->info.function.returntype = returntype;
  newtype->info.function.nparams = 0;
  // TODO: dynamically increase size of param_limit instead of fixed variable;
  newtype->info.function.parameters = (Parameter **)malloc(sizeof(Parameter *) * PARAM_LIMIT);
  for (int i = 0; i < PARAM_LIMIT; i++) {
    newtype->info.function.parameters[i] = NULL;
  }
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
  newtype->info.array.size = size;
  newtype->info.array.type = type;
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
  if (type1 == unknown_t || type2 == unknown_t)
    return TYPE_NOT_EQUAL;
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
        else {
          return TYPE_NOT_EQUAL;
        }
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