#ifndef TYPES_H
#define TYPES_H

enum BaseType {
  VOID_T,
  INT_T,
  SHORT_T,
  LONG_T,
  FLOAT_T,
  DOUBLE_T,
  LONG_DOUBLE_T,
  CHAR_T,
  UNSIGNED_T
  ARRAY_T,
  CLASS_T,
  FUNCTION_T
} BaseType;

struct Type {
  BaseType basetype;
  union {
    struct ArrayInfo {
      int size; /* allow for missing size, e.g. -1 */
      typeinfo *elemType; /* pointer to c_type for elements in array,
          follow it to find its base type, etc.*/
    } array;
    struct ClassInfo {    /* structs */
      char *name;
      int nFields;
      ClassField **fields;
    } class;
    struct FunctionInfo {
      typeinfo *returntype;
      list *parameters;
    } function;
  } info;
} Type;

#endif