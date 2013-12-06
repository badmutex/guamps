#ifndef __GUAMPS_OUTPUT_H__
#define __GUAMPS_OUTPUT_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct {
  enum { FILETYPE_PATH, FILETYPE_FILE } type;
  union { char *path; FILE *handle; } file;
  bool should_close;
} args_file_t;

void guamps_error(const char *str, ...);

FILE * output_file_fopen(args_file_t *f, char *mode);
void output_file_close(args_file_t *f);

void fprint_enum(const char *names[], const size_t count, const int indent, FILE *stream);

#endif
