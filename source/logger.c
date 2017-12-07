#include "../header/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void log_init_global() {
  log_total_errors = 0;
  log_first_error = NO_ERROR;
}

void log_error(ErrorTypes errortype, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  if (log_first_error == 0)
    log_first_error = errortype;
  log_total_errors++;
}

void log_lex_error(char *filename, int lineno, char *message, char *token) {
  log_error(LEX_ERROR, "%s:%d:LEXICAL ERROR: Near the token '%s' %s. \n", filename, lineno, token, message);
}

void log_syn_error(char *filename, int lineno, char *message, char *token) {
  log_error(SYN_ERROR, "%s:%d:SYNTAX ERROR: Near the token '%s' %s. \n", filename, lineno, token, message);
}

void log_sup_error(char *filename, int lineno, char *message, char *token) {
  log_error(SUP_ERROR, "%s:%d:NOT SUPPORTED ERROR: Near the token '%s' %s is not supported int 120++. \n", filename, lineno, token, message);
}

void log_sem_error(char *filename, int lineno, char *message, char *token) {
  log_error(SEM_ERROR, "%s:%d:SEMANTIC ERROR: Near the token '%s' %s. \n", filename, lineno, token, message);
}

void log_final_status() {
  fprintf(stderr, "TOTAL ERRORS: %d\n", log_total_errors);
}

// TODO: create a helper function for better type checking error message