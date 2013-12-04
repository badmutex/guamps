#include "guampsio.h"
#include "gmxutil.h"


#include "stdio.h"
#include "stdbool.h"


int init_gmx(){
  gmx_init_params_t gmx_params;
  gmx_params.program_name = "guamps_set";
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

  data_t data;
  selector_t sel;
  selectable_t *obj;

  obj = guamps_load(path);
  guamps_pick_selector(selstr, &sel);

  FILE *fh = fopen(datpath, "r");
  guamps_fread(fh, RVEC_T, &data);
  fclose(fh);
  guamps_update(obj, sel, &data);
  guamps_write(path, obj);

}
