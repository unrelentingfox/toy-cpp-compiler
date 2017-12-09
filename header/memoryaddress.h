#ifndef MEMORYADDRESS_H
#define MEMORYADDRESS_H

enum MemoryRegion {
  GLOBAL_R = 2001,
  LOCAL_R = 2002,
  PARAM_R = 2003,
  CLASS_R = 2004,
  LABEL_R = 2005,
  CONST_R = 2006,
};

typedef struct MemoryAddress {
  enum MemoryRegion region;
  int offset;
} Address;

int GLOBAL_R_COUNT;
int LOCAL_R_COUNT;
int PARAM_R_COUNT;
int CLASS_R_COUNT;
int LABEL_R_COUNT;
int CONST_R_COUNT;

struct MemoryAddress *memory_address_new(enum MemoryRegion region, int size);

#endif