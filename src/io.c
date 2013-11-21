#include "io.h"

int guamps_read_checkpoint_X(const char *path, const char *selector, gmx_data_t *result) {

  int part;
  gmx_large_int_t step;
  double time;
  t_state state;
  init_state(&state, -1, -1, -1, -1);

  read_checkpoint_state(path, &part, &step, &time, &state);

  return guamps_get_state_X(&state, selector, result);
}

int guamps_get_state_X(const t_state *state, const char *selector, gmx_data_t *result) {

  const char *sel = selector;
  const t_state *st = state;

  if      (strcmp(sel, "natoms") == 0) {
    result->type         = INT;
    result->data.number = st->natoms;
    return 0;
  }
  else if (strcmp(sel, "position") == 0) {
    result->type = RVEC;
    result->data.rvec = st->x;
    return 0;
  }
  else if (strcmp(sel, "velocity") == 0) {
    result->type = RVEC;
    result->data.rvec = st->v;
    return 0;
  }
  else {
    return 1;
  }

}

int guamps_write_rvec(FILE *fh, const rvec *vec, const int length) {

  const int
    ncells = 1,
    ncoords = length,
    ndims = 3;

  fprintf(fh, "ncells: %d\nncoords: %d\nndims: %d\n\n", ncells, ncoords, ndims);

  for (int i=0; i<ncoords; i++){
    for (int j=0; j<ndims; j++){
      fprintf(fh, "%f\n", vec[i][j]);
    }
  }

  return 0;
}

