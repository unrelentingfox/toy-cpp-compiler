#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

typedef enum ErrorTypes {
  NO_ERROR,
  LEX_ERROR,
  SYN_ERROR,
  SEM_ERROR,
  SUP_ERROR,
  NULL_ERROR
} ErrorTypes;

void log_assert(bool val, char *name);
void log_error(ErrorTypes errortype, const char *format, ...);
// void log_lex_error(const char *format, ...);
void log_syn_error(char* filename, int lineno, char* message, char* token);
// void log_sem_error(const char *format, ...);
void log_sup_error(char* filename, int lineno, char* message, char* token);

#endif