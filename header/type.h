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
  struct Type *classtype;
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

struct Type *void_t;
struct Type *int_t;
struct Type *short_t;
struct Type *long_t;
struct Type *float_t;
struct Type *double_t;
struct Type *long_double_t;
struct Type *char_t;
struct Type *unsigned_t;
struct Type *unknown_t;

static int BASETYPES_INITIALIZED;
static void type_initialize_basetypes();

struct Type *type_from_terminal(enum yytokentype terminal);
struct Type *type_new(BaseType basetype);
Type *type_new_function(Type *returntype);
Type *type_new_class(char* name);
Type *type_new_class_instance(Type *classtype);
Type *type_new_array(Type *type, int size);
struct Parameter *type_new_parameter(Type *type);
enum TypeCompareResults type_compare(Type *type1, Type *type2);

#endif