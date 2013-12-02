#include "stdio.h"
#include "stdbool.h"

#include "io.h"
#include "gmxutil.h"

int init_gmx(){
  gmx_init_params_t gmx_params;
  gmx_params.program_name = "guamps_has";
  return guamps_init_gromacs(&gmx_params);
}


int main(int argc, char *argv[]) {

  if (!init_gmx()) {
    fprintf(stderr, "Unable to initialize GROMACS\n");
    return 1;
  }

  const char
    *path    = argv[1],
    *selstr  = argv[2],
    *datpath = argv[3];

  guamps_data_t r;
  selector_t selector;
  if (!guamps_pick_selector(selstr, &selector)) {
    fprintf(stderr, "Unknown selection: %s\n", selstr);
    return 1;
  }

  // read and select
  filetype_t ftype;
  guamps_pick_filetype(path, &ftype);

  FILE *fh = fopen(datpath, "r");
  guamps_read_rvec(fh, &r);

}
