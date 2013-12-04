#include "guampsio.h"
#include "gmxutil.h"

#include "stdio.h"
#include "stdbool.h"


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

  const char *path   = argv[1];
  const char *selstr = argv[2];

  data_t r;
  selector_t selector;
  if (!guamps_pick_selector(selstr, &selector)) {
    fprintf(stderr, "Unknown selection: %s\n", selstr);
    return 1;
  }

  data_t data;
  selectable_t *sel;

  if(!(sel = guamps_load(path))) {
    guamps_error("%s: failed to load %s\n", argv[0], path);
    return 1;
  }
  if(!guamps_select(sel, selector, &data)){
    guamps_error("%s: failed to select %s\n", argv[0], GUAMPS_SELECTOR_NAMES);
    return 1;
  }
  guamps_write(stdout, &data);

}
