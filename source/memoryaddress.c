#include "../header/memoryaddress.h"
#include "../header/logger.h"
#include <stdlib.h>
#include <string.h>

int GLOBAL_R_COUNT;
int LOCAL_R_COUNT;
int PARAM_R_COUNT;
int CLASS_R_COUNT;
int LABEL_R_COUNT;
int CONST_R_COUNT;
int TEMP_R_COUNT;
int GLOBALS_INITIALIZED;

void mem_init() {
  GLOBAL_R_COUNT = 0;
  LOCAL_R_COUNT = 0;
  PARAM_R_COUNT = 0;
  CLASS_R_COUNT = 0;
  LABEL_R_COUNT = 1;
  CONST_R_COUNT = 0;
  TEMP_R_COUNT = 0;
  GLOBALS_INITIALIZED = 1;
}

MemoryAddress *mem_new_address(enum MemoryRegion region, int size) {
  if (!GLOBALS_INITIALIZED) {
    mem_init();
  }
  MemoryAddress *new_address = (MemoryAddress *)malloc(sizeof(MemoryAddress));
  LOG_ASSERT(new_address);
  switch (region) {
    case GLOBAL_R:
      new_address->offset = GLOBAL_R_COUNT;
      GLOBAL_R_COUNT += size;
      break;
    case LOCAL_R:
      new_address->offset = LOCAL_R_COUNT;
      LOCAL_R_COUNT += size;
      break;
    case PARAM_R:
      new_address->offset = PARAM_R_COUNT;
      PARAM_R_COUNT += size;
      break;
    case CLASS_R:
      new_address->offset = CLASS_R_COUNT;
      CLASS_R_COUNT += size;
      break;
    case LABEL_R:
      new_address->offset = LABEL_R_COUNT;
      LABEL_R_COUNT += size;
      break;
    case CONST_R:
      new_address->offset = CONST_R_COUNT;
      CONST_R_COUNT += size;
      break;
    default:
      log_error(INTERNAL_ERROR, "Invalid MemoryRegion in MemoryAddress creation.\n");
      exit(INTERNAL_ERROR);
      break;
  }
  new_address->region = region;
  return new_address;
}

char *mem_address_to_str(struct MemoryAddress *address) {
  if (!address)
    return strdup("");
  char *temp = malloc(sizeof(char) * 20);
  switch (address->region) {
    case GLOBAL_R:
      snprintf(temp, 20, "<GLOBAL, %d>", address->offset);
      break;
    case LOCAL_R:
      snprintf(temp, 20, "<LOCAL, %d>", address->offset);
      break;
    case PARAM_R:
      snprintf(temp, 20, "<PARAM, %d>", address->offset);
      break;
    case CLASS_R:
      snprintf(temp, 20, "<CLASS, %d>", address->offset);
      break;
    case LABEL_R:
      snprintf(temp, 20, "<LABEL, %d>", address->offset);
      break;
    case CONST_R:
      switch (address->consttype) {
        case SVAL:
          snprintf(temp, 20, "<CONST, %s>", address->constval.sval);
          break;
        case IVAL:
          snprintf(temp, 20, "<CONST, %d>", address->constval.ival);
          break;
        case DVAL:
          snprintf(temp, 20, "<CONST, %f>", address->constval.dval);
          break;
      }
      break;
    default:
      return strdup("(INVALID)");
      break;
  }
  return temp;
}
