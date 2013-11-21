#include "stdbool.h"

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
    result->type        = INT;
    result->data.number = st->natoms;
    return true;
  }
  else if (strcmp(sel, "position") == 0) {
    result->type	       = RVEC;
    result->data.vector.rvec   = st->x;
    result->data.vector.natoms = st->natoms;
    return true;
  }
  else if (strcmp(sel, "velocity") == 0) {
    result->type               = RVEC;
    result->data.vector.rvec   = st->v;
    result->data.vector.natoms = st->natoms;
    return true;
  }
  else {
    return false;
  }

}

int guamps_write(FILE *fh, const gmx_data_t *data) {
  switch(data->type) {
  case RVEC:
    return guamps_write_rvec(fh, data->data.vector.rvec, data->data.vector.natoms);
    break;
  case INT:
    return guamps_write_int(fh, data->data.number);
    break;
  default:
    return false;
    break;
  }
  return true;
}

int guamps_write_int(FILE *fh, const int i){
  return fprintf(fh, "%d\n", i);
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

  return true;
}

