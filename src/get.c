#include "stdio.h"
#include "stdbool.h"

#include "io.h"
#include "gmxutil.h"

int init_gmx(){
  gmx_init_params_t gmx_params;
  gmx_params.program_name = "guamps_get";
  return guamps_init_gromacs(&gmx_params);
}

int main(int argc, char *argv[]){
  if (!init_gmx()) {
    fprintf(stderr, "Unable to initialize GROMACS\n");
    return 1;
  }

  const char *cpt_path = argv[1];
  const char *selstr   = argv[2];

  gmx_data_t r;
  selector_t selector;
  if (!guamps_pick_selector(selstr, &selector)) {
    fprintf(stderr, "Unknown selection: %s\n", selstr);
    return 1;
  }

  if (guamps_read_checkpoint_X(cpt_path, selector, &r))
    guamps_write(stdout, &r);
    
  

  return 0;
}
