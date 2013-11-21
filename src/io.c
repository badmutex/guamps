#include "stdbool.h"

#include "io.h"

int guamps_pick_selector(const char *str, selector_t *sel) {
  if (0 == strcmp(str, "natoms")) {
    *sel = NATOMS; }
  else if (0 == strcmp(str, "positions")) {
    *sel = POSITIONS; }
  else if (0 == strcmp(str, "velocities")) {
    *sel = VELOCITIES; }
  else if (0 == strcmp(str, "forces")) {
    *sel = FORCES; }
  else if (0 == strcmp(str, "lambda")) {
    *sel = LAMBDA; }
  else if (0 == strcmp(str, "box")) {
    *sel = BOX; }
  else if (0 == strcmp(str, "step")) {
    *sel = STEP; }
  else if (0 == strcmp(str, "time")) {
    *sel = TIME; }
  else {
    return false;
  }
  return true;
}

int guamps_read_checkpoint_X(const char *path, const selector_t selector, gmx_data_t *result) {

  int part;
  gmx_large_int_t step;
  double time;
  t_state state;
  init_state(&state, -1, -1, -1, -1);

  read_checkpoint_state(path, &part, &step, &time, &state);

  return guamps_get_state_X(&state, selector, result);
}

int guamps_get_state_X(const t_state *st, const selector_t sel, gmx_data_t *result) {

  switch (sel) {
  case NATOMS:
    result->type        = INT;
    result->data.number = st->natoms;
    return true;
    break;
  case POSITIONS:
    result->type	       = RVEC;
    result->data.vector.rvec   = st->x;
    result->data.vector.natoms = st->natoms;
    return true;
    break;
  case VELOCITIES:
    result->type               = RVEC;
    result->data.vector.rvec   = st->v;
    result->data.vector.natoms = st->natoms;
    return true;
    break;
  default:
    return false;
    break;
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

