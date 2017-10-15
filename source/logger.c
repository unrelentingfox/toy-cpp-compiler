#include "../header/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void log_assert(bool val, char *name) {
  if (val) {
    return;
  } else {
    fprintf(stderr, "ERROR: Unexpected NULL value for variable: \"%s\".", name);
    exit(NULL_ERROR);
  }
}

void log_error(ErrorTypes errortype, const char *format, ...) {
  va_list args;
  va_start(args, format);
  fprintf(stderr, format, args);
  va_end(args);
  exit(errortype);
}

void log_lex_error(const char *format, ...) {
  va_list args;
  log_error(LEX_ERROR, format, args);
}

void log_syn_error(char* filename, int lineno, char* message, char* token) {
  log_error(SYN_ERROR, "SYNTAX ERROR: In file (%s) line number (%d) %s, Token: \"%s\"\n",filename, lineno, message, token);
}

void log_sup_error(char* filename, int lineno, char* message, char* token){
  log_error(SUP_ERROR, "NOT SUPPORTED ERROR: In file (%s) line number (%d) %s, Token: \"%s\"\n",filename, lineno, message, token);
}

void log_sem_error(const char *format, ...) {
  va_list args;
  log_error(SEM_ERROR, format, args);
}