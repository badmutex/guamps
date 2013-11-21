#include "stdio.h"

#include "io.h"
#include "gmxutil.h"

int main(int argc, char *argv[]){
  const char *cpt_path = "../state0.cpt";
  const char *sel     = "velocity";
  selection_result_t r;

  gmx_init_params_t gmx_params;
  gmx_params.program_name = "guamps_get";
  guamps_init_gromacs(&gmx_params);

  guamps_read_checkpoint_X(cpt_path, sel, &r);
  guamps_write_rvec(stdout, r.vec, 551);
    
  

  return 0;
}
