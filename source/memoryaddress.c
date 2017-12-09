#include "../header/memoryaddress.h"
#include "../header/logger.h"

static int GLOBALS_INITIALIZED;
static void memory_address_init() {
  GLOBAL_R_COUNT = 0;
  LOCAL_R_COUNT = 0;
  PARAM_R_COUNT = 0;
  CLASS_R_COUNT = 0;
  LABEL_R_COUNT = 0;
  CONST_R_COUNT = 0;
  GLOBALS_INITIALIZED = 1;
}

MemoryAddress *memory_address_new(enum MemoryRegion region, int size) {
  if (!GLOBALS_INITIALIZED) {
    memory_address_init();
  }
  MemoryAddress *new_address = (MemoryAddress *)malloc(sizeof(MemoryAddress));
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
