#include "output.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include <ctype.h>

args_file_t* new_args_file_t(){
  args_file_t *ft;
  ft = (args_file_t*)calloc(1, sizeof(args_file_t));
  return ft;
}

static void vguamps_msg_output(const char *name, const char *str, va_list args) {
  const char *extra = "[GUAMPS]";
  char *msg = (char *) calloc (strlen(extra) + strlen(name) + 3 +strlen(str), sizeof(char));
  strcat(msg, extra);
  strcat(msg, " ");
  strcat(msg, name);
  strcat(msg, ": ");
  strcat(msg, str);

  vfprintf(stderr, msg, args);
  free(msg);
}

void guamps_error(const char *str, ...) {
  va_list args;
  va_start(args, str);
  vguamps_msg_output("ERROR", str, args);
  va_end(args);
}

void guamps_warn(const char *str, ...) {
  va_list args;
  va_start(args, str);
  vguamps_msg_output("WARNING", str, args);
  va_end(args);
}

FILE * args_file_fopen(args_file_t *f, char *mode) {
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
