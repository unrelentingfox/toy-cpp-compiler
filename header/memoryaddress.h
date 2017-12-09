#ifndef MEMORYADDRESS_H
#define MEMORYADDRESS_H

enum MemoryRegion {
  GLOBAL_R = 2001,
  LOCAL_R = 2002,
  PARAM_R = 2003,
  CLASS_R = 2004,
  LABEL_R = 2005,
  CONST_R = 2006
};

enum ConstantType {
  IVAL,
  DVAL,
  SVAL
};

union ConstantValue {
  int ival;
  double dval;
  char *sval;
};

typedef struct MemoryAddress {
  enum MemoryRegion region;
  int offset;
  enum ConstantType consttype;
  union ConstantValue constval;
} MemoryAddress;

struct MemoryAddress *mem_new_address(enum MemoryRegion region, int size);
char *mem_address_to_str(struct MemoryAddress *address);

#endif