#include "../header/type.h"
#include "../header/logger.h"
#include <stdlib.h>
#include <string.h>

#define PARAM_LIMIT 22

static int BASETYPES_INITIALIZED;
static void type_initialize_basetypes();
static Type *type_initialize_basetype(enum BaseType basetype, int size);

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

static char *type_concatinate_strings(char *a, char *b) {
  if (a == NULL || b == NULL)
    return strdup("");
  int alen;
  int blen;
  for (alen = 0; a[alen] != '\0'; alen++) {}
  for (blen = 0; b[blen] != '\0'; blen++) {}
  char *destination = calloc(alen + blen + 1, sizeof(char));
  strcat(destination, a);
  strcat(destination, b);
  return destination;
}

char *type_to_string(Type *type) {
  if (!type) {
    return strdup("(NULL)");
  }
  switch (type->basetype) {
    case VOID_T:
      return strdup("void");
      break;
    case INT_T:
      return strdup("int");
      break;
    case SHORT_T:
      return strdup("short");
      break;
    case LONG_T:
      return strdup("long");
      break;
    case FLOAT_T:
      return strdup("float");
      break;
    case DOUBLE_T:
      return strdup("double");
      break;
    case CHAR_T:
      return strdup("char");
      break;
    case UNSIGNED_T:
      return strdup("unsigned");
      break;
    case CLASS_T:
      return strdup("class");;
      break;
    case CLASS_INSTANCE_T: {
      LOG_ASSERT(type->info.classinstance.classtype);
      LOG_ASSERT(type->info.classinstance.classtype->info.class.name);
      return strdup(type->info.classinstance.classtype->info.class.name);
    }
    break;
    case FUNCTION_T:
      return strdup("function");
      break;
    case POINTER_T: {
      LOG_ASSERT(type->info.pointer.type);
      char *name = type_to_string(type->info.pointer.type);
      name = type_concatinate_strings(name, "*");
      return name;
    }
    break;
    case ARRAY_T: {
      LOG_ASSERT(type->info.array.type);
      char *temp1 = type_to_string(type->info.array.type);
      int size = type->info.array.size;
      char *sizestring = malloc(sizeof(char) * 16);
      snprintf(sizestring, 16, "%d", size);
      char *temp2 = type_concatinate_strings(temp1, "[");
      free(temp1);
      temp1 = type_concatinate_strings(temp2, sizestring);
      free(temp2);
      temp2 = type_concatinate_strings(temp1, "]");
      free(sizestring);
      free(temp1);
      return temp2;
    }
    break;
    case UNKNOWN_T:
    default:
      return strdup("unknown");
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

static Type *type_new() {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = (Type *)malloc(sizeof(Type));
  // by default just have every type size be 8 unless otherwise changed lated
  newtype->size = 8;
  return newtype;
}

Type *type_new_function(Type *returntype) {
  if (!returntype)
    returntype = type_get_basetype(VOID_T);
  Type *newtype = type_new();
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
  Type *newtype = type_new();
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
  Type *newtype = type_new();
  newtype->basetype = CLASS_INSTANCE_T;
  newtype->info.classinstance.classtype = classtype;
  return newtype;
}

Type *type_new_pointer(Type *type) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = type_new();
  newtype->basetype = POINTER_T;
  newtype->info.pointer.type = type;
  newtype->size = type->size;
  return newtype;
};

Type *type_new_array(Type *type, int size) {
  if (!BASETYPES_INITIALIZED)
    type_initialize_basetypes();
  Type *newtype = type_new();
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
  else if (type1 == unknown_t || type2 == unknown_t)
    return TYPE_NOT_EQUAL;
  else if (type1->basetype != type2->basetype) {
    return TYPE_NOT_EQUAL;
  } else {
    switch (type1->basetype) {
      case CLASS_INSTANCE_T: {
        if (type1->info.classinstance.classtype == type2->info.classinstance.classtype)
          return TYPE_EQUAL;
        else
          return TYPE_NOT_EQUAL;
      }
      break;
      case POINTER_T: {
        if (type_compare(type1->info.pointer.type, type2->info.pointer.type) == TYPE_EQUAL)
          return TYPE_EQUAL;
        else {
          return TYPE_NOT_EQUAL;
        }
      }
      break;
      case ARRAY_T: {
        if (type_compare(type1->info.array.type, type2->info.array.type) == TYPE_EQUAL
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