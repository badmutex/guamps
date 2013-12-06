#include "stdbool.h"
#include "gmxutil.h"

int guamps_init_gromacs(const gmx_init_params_t *params) {
  if (params == NULL) {
    return true;
  }

  if (params->program_name != NULL) {
    guamps_gmx_set_progname(params->program_name);
  }

  return true;
}

int guamps_gmx_set_progname(const char *name) {
  set_program_name(name);
  return true;
}
