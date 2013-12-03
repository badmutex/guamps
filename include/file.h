#ifndef __GUAMPS_FILE_H__
#define __GUAMPS_FILE_H__

typedef enum {
  FILE_HANDLE_GMX, FILE_HANDLE_FILE
} file_t;


file_t *guamps_fopen(const char *path, const char *mode);
int     guamps_fclose(file_t *fh);

#endif
