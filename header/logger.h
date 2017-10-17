#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

int log_total_errors;
int log_first_error;

typedef enum ErrorTypes {
  NO_ERROR,
  LEX_ERROR,
  SYN_ERROR,
  SEM_ERROR,
  SUP_ERROR,
  NULL_ERROR
} ErrorTypes;

void log_init_global();
void log_assert(bool val, char *name);
void log_error(ErrorTypes errortype, const char *format, ...);
void log_lex_error(char* filename, int lineno, char* message, char* token);
void log_syn_error(char* filename, int lineno, char* message, char* token);
void log_sem_error(char* filename, int lineno, char* message, char* token);
void log_sup_error(char* filename, int lineno, char* message, char* token);
void log_final_status();

#endif