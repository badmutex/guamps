
#ifndef __GUAMPS_WRITE_H__
#define __GUAMPS_WRITE_H__

#include "stdio.h"
#include "string.h"

#include "gromacs/types/state.h"
#include "gromacs/tpxio.h"
#include "gromacs/checkpoint.h"

typedef struct gmx_data_s {
    enum type_e { RVEC, INT } type;
    union data_u {
        rvec *rvec;
        int number;
    } data;
} gmx_data_t;

typedef union selection_result_u {
    rvec *vec;
    int  count;
} selection_result_t;


int guamps_read_checkpoint_X(const char *path, const char *selector, gmx_data_t *result);
int guamps_get_state_X(const t_state *state, const char *selector, gmx_data_t *result);
int guamps_write_rvec(FILE *fh, const rvec *vec, const int length);

int guamps_write(FILE *fh, const gmx_data_t *data);

int guamps_read_tpr_X(const char *path, const char *selector, selection_result_t *result);
int guamps_read_checkpoint(const char *path, t_state *state);
int guamps_read_tpr(const char *path, t_inputrec *params, t_state *state);



#endif
