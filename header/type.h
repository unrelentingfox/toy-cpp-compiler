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

typedef enum FunctionStatus {
  FUNC_NEW,
  FUNC_DECLARED,
  FUNC_DEFINED
} FunctionStatus;

typedef enum ClassStatus {
  CLASS_NEW,
  CLASS_DECLARED,
  CLASS_DEFINED
} ClassStatus;

typedef enum TypeCompareResults {
  TYPE_NOT_EQUAL,
  TYPE_EQUAL,
  TYPE_NULL_PARAMETERS
} TypeCompareResults;

typedef struct ArrayInfo {
  int size; /* allow for missing size, e.g. -1 */
  struct Type *type; /* pointer to c_type for elements in array,
          follow it to find its base type, etc.*/
} ArrayInfo;

typedef struct ClassInfo {    /* structs */
  char *name;
  int nFields;
  enum ClassStatus status;
  struct SymbolTable *public;
  struct SymbolTable *private;
} ClassInfo;

typedef struct ClassInstanceInfo {
  struct Type* classtype;
} ClassInstanceInfo;

typedef struct Parameter {
  struct Type *type;
  struct SymtabNode *symtabnode;
} Parameter;

typedef struct FunctionInfo {
  enum FunctionStatus status; /* either FUNC_DECLARED or FUNC_DEFINED */
  struct Type *returntype;
  int nparams;
  struct Parameter **parameters;
  struct SymbolTable *symtab;
} FunctionInfo;

typedef struct Type {
  BaseType basetype;
  union info {
    ArrayInfo array;
    ClassInfo class;
    FunctionInfo function;
    ClassInstanceInfo classinstance;
  } info;
} Type;

BaseType type_from_terminal(enum yytokentype terminal);

Type *type_new(BaseType basetype);
Parameter *type_new_parameter(Type *type);

TypeCompareResults type_compare(Type *type1, Type *type2);

#endif