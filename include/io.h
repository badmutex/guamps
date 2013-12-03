
#ifndef __GUAMPS_WRITE_H__
#define __GUAMPS_WRITE_H__

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "types.h"


int guamps_pick_selector(const char *selstr, selector_t *sel);
int guamps_read_checkpoint_X(const char *path, const selector_t sel, guamps_data_t *result);
int guamps_get_state_X(const t_state *state, const selector_t sel, guamps_data_t *result);
int guamps_write(FILE *fh, const guamps_data_t *data);
int guamps_write_int(FILE *fh, const int);
int guamps_write_rvec(FILE *fh, const rvec *vec, const int length);
int guamps_pick_filetype(const char *path, filetype_t *type);
int guamps_read_tpr_X(const char *path, const selector_t sel, guamps_data_t *result);
void guamps_error(const char *str, ...);
int guamps_read_rvec(FILE *fh, guamps_data_t *data);
int guamps_selector_type(const selector_t sel, type_t *type);
int guamps_read_int(FILE *fh, guamps_data_t *data);
int guamps_read(FILE *fh, const selector_t sel, guamps_data_t *data);
tpr_t * guamps_load_tpr(const char *path);

void guamps_save_tpr(const char *path, tpr_t *tpr);

int guamps_read_trr_X(const char *path, const unsigned int frame, const selector_t sel, guamps_data_t *result);



#endif
