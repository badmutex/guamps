#include "guampsio.h"
#include "gromacs/statutil.h"

void test0() {

  tpr_t *tpr = guamps_load_tpr("topol.tpr");
  data_t res;
  guamps_select_tpr(tpr, LAMBDA, &res);
  guamps_write_data(stdout, &res);
}

int main(int argc, char *argv[]) {
  set_program_name(argv[0]);
  test0();
  return 0;
}
