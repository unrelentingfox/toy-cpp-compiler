#ifndef PWD_H
#define PWD_H

#include <stdio.h>  /* defines FILENAME_MAX */
#include <string.h>
#include "logger.h"
#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#elif __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#else
log_error(INTERNAL_ERROR, "Operating systems besides windows and linux are not supported by 120++\n");
#endif

char *PWD(void) {
  char *buff;
  buff = (char *)malloc(sizeof(char) * FILENAME_MAX);
  GetCurrentDir(buff, FILENAME_MAX);
  return buff;
}

#endif