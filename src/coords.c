#include "io.h"

#include "stdio.h"

#include "gromacs/types/state.h"
#include "gromacs/tpxio.h"
#include "gromacs/checkpoint.h"




int read_cpt_coords(const char *path, t_state *state) {
  int sim_part;
  gmx_large_int_t step;
  double time;

  init_state(state, -1, -1, -1, -1);
  read_checkpoint_state(path, &sim_part, &step, &time, state);

  return 0;

}

int main(int argc, char *argv[]){

  const char *cpt_path = "../state0.cpt";
  const char *x_path = "../coords.txt";

  t_state state;

  read_cpt_coords(cpt_path, &state);

  FILE *fh = fopen(x_path, "w");
  guamps_write_rvec(fh, state.x, state.natoms);

  return 0;

}
