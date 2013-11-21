#include "gmxutil.h"

int guamps_init_gromacs(const gmx_init_params_t *params) {
  if (params == NULL) {
    return 0;
  }

  if (params->program_name != NULL) {
    set_program_name(params->program_name);
  }

  return 0;
}
