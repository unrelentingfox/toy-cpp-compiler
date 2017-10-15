#ifndef TYPES_H
#define TYPES_H

#include "symtab.h"

typedef enum BaseType {
  VOID_T,
  INT_T,
  SHORT_T,
  LONG_T,
  FLOAT_T,
  DOUBLE_T,
  LONG_DOUBLE_T,
  CHAR_T,
  UNSIGNED_T,
  ARRAY_T,
  CLASS_T,
  FUNCTION_T
} BaseType;

typedef struct Type {
  BaseType basetype;
  union info {
    struct ArrayInfo {
      int size; /* allow for missing size, e.g. -1 */
      struct Type *elemType; /* pointer to c_type for elements in array,
          follow it to find its base type, etc.*/
    } array;
    struct ClassInfo {    /* structs */
      char *name;
      int nFields;
      struct SymbolTable *symtab;
    } class;
    struct FunctionInfo {
      struct Type *returntype;
      // list *parameters;
      struct SymbolTable *symtab;
    } function;
  } info;
} Type;

#endif