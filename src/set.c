#include "guampsio.h"
#include "gmxutil.h"

#include "stdio.h"
#include "stdbool.h"


int main(int argc, char *argv[]) {

  const char *progname = argv[0];

  if (!guamps_gmx_set_progname(progname)) {
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
  if(!guamps_fread(fh, RVEC_T, &data)){
    guamps_error("Cannot read data file %s\n", datpath);
    return 1;
  }
  fclose(fh);

  if(!guamps_update(obj, sel, &data)){
    guamps_error("Cannot update %s with new values\n", GUAMPS_FILETYPE_NAMES[obj->kind]);
    return 1;
  }

  if(!guamps_write(path, obj)){
    guamps_error("Cannot write %s\n", path);
    return 1;
  }

}
