#include "output.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

void guamps_error(const char *str, ...) {
  const char *extra = "[GUAMPS] ";
  char *msg = (char *) calloc (strlen(extra) + strlen(str), sizeof(char));
  strcat(msg, extra);
  strcat(msg, str);

  va_list args;
  va_start(args, str);
  vfprintf(stderr, msg, args);
  va_end(args);
}
