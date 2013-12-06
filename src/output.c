#include "output.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include <ctype.h>

void guamps_error(const char *str, ...) {
  const char *extra = "[GUAMPS] ";
  char *msg = (char *) calloc (strlen(extra) + strlen(str), sizeof(char));
  strcat(msg, extra);
  strcat(msg, str);

  va_list args;
  va_start(args, str);
  vfprintf(stderr, msg, args);
  va_end(args);
  free(msg);
}

FILE * output_file_fopen(args_file_t *f, char *mode) {
  FILE *fh;
  switch(f->type) {
  case FILETYPE_PATH:
    fh = (FILE *)malloc(sizeof(FILE));
    fh = fopen(f->file.path, mode);
    f->should_close = true;
    break;
  case FILETYPE_FILE:
    f->should_close = false;
    fh = f->file.handle;
  }
  return fh;
}

void output_file_close(args_file_t *f) {
  if(f->should_close && f->file.handle)
    fclose(f->file.handle);
}

void fprint_enum(const char *names[], const size_t count, const int indent, FILE *stream) {

    for (int enum_ix=0; enum_ix<count; enum_ix++) {
      for (int ind=0; ind<indent; ind++) {
	fputc(' ', stream);
      }

      int char_ix = 0;
      while(names[enum_ix][char_ix]) {
	fputc(tolower(names[enum_ix][char_ix]), stream);
	char_ix++;
      }
      fputc('\n', stream);
    }


}
