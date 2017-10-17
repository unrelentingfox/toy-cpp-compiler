#ifndef TYPES_H
#define TYPES_H

#include "symtab.h"
#include "bison.h"

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
  CLASS_INSTANCE_T,
  FUNCTION_T,
  UNKNOWN_T
} BaseType;

typedef struct ArrayInfo {
  int size; /* allow for missing size, e.g. -1 */
  struct Type *type; /* pointer to c_type for elements in array,
          follow it to find its base type, etc.*/
} ArrayInfo;

typedef struct ClassInfo {    /* structs */
  char *name;
  int nFields;
  struct SymbolTable *public;
  struct SymbolTable *private;
} ClassInfo;

typedef struct FunctionInfo {
  struct Type *returntype;
  struct Parameter *parameters;
  struct SymbolTable *symtab;
} FunctionInfo;

typedef struct Type {
  BaseType basetype;
  union info {
    ArrayInfo array;
    ClassInfo class;
    FunctionInfo function;
    struct Type *parentclass;
  } info;
} Type;

/* TODO: turn parameter list into a hash table */
typedef struct Parameter {
  Type *type;
  struct Parameter *next;
} Parameter;

int type_from_terminal(int terminal);

Type *type_new(enum BaseType basetype);
Parameter *type_new_parameter(Type *type);

#endif