#ifndef __GUAMPS_GMXWRAP_H__
#define __GUAMPS_GMXWRAP_H__

#include "gromacs/statutil.h"

typedef struct {
  char *program_name;
} gmx_init_params_t;

int guamps_init_gromacs(const gmx_init_params_t *params);
int guamps_gmx_set_progname(const char *name);


#endif
