#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define LOG_ASSERT(val) \
  if (!val) { \
    fprintf(stderr, "120++ INTERNAL ERROR: In file (%s) line number (%d) assertion failed.\n", __FILE__, __LINE__); \
    exit(INTERNAL_ERROR); \
  }

int log_total_errors;
int log_first_error;

typedef enum ErrorTypes {
  NO_ERROR,
  LEX_ERROR,
  SYN_ERROR,
  SEM_ERROR,
  SUP_ERROR,
  INTERNAL_ERROR
} ErrorTypes;

void log_init_global();

void log_error(ErrorTypes errortype, const char *format, ...);
void log_lex_error(char *filename, int lineno, char *message, char *token);
void log_syn_error(char *filename, int lineno, char *message, char *token);
void log_sem_error(char *filename, int lineno, char *message, char *token);
void log_sup_error(char *filename, int lineno, char *message, char *token);
void log_final_status();

#endif