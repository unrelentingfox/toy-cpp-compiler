#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

typedef enum ErrorTypes {
  LEX_ERROR,
  SYN_ERROR,
  SEM_ERROR,
  NULL_ERROR
} ErrorTypes;

void log_assert(bool val, char *name);
void log_error(ErrorTypes errortype, const char *format, ...);
void log_lex_error(const char *format, ...);
void log_syn_error(const char *format, ...);
void log_sem_error(const char *format, ...);

#endif