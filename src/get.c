#include "stdio.h"

#include "io.h"
#include "gmxutil.h"

int main(int argc, char *argv[]){
  const char *cpt_path = argv[1];
  const char *sel     = argv[2];
  gmx_data_t r;

  gmx_init_params_t gmx_params;
  gmx_params.program_name = "guamps_get";
  guamps_init_gromacs(&gmx_params);

  guamps_read_checkpoint_X(cpt_path, sel, &r);
  guamps_write(stdout, &r);
    
  

  return 0;
}
