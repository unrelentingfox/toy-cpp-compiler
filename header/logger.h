#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdio.h>

enum errortypes {
  LEX_ERROR,
  SYN_ERROR,
  SEM_ERROR,
  NULL_ERROR
}

void log_assert(bool val);
void log_error(const char * format, ...);

#endif