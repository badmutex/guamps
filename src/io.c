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
    result->type        = INT_T;
    result->data.number = st->natoms;
    return true;
    break;
  case POSITIONS:
    result->type	       = RVEC_T;
    result->data.vector.rvec   = st->x;
    result->data.vector.natoms = st->natoms;
    return true;
    break;
  case VELOCITIES:
    result->type               = RVEC_T;
    result->data.vector.rvec   = st->v;
    result->data.vector.natoms = st->natoms;
    return true;
    break;
  case RNG:
    guamps_error("Getting RNG state from GMX t_state is not supported\n");
    return false;
    break;
  default:
    return false;
    break;
  }

}

int guamps_write(FILE *fh, const gmx_data_t *data) {
  switch(data->type) {
  case RVEC_T:
    return guamps_write_rvec(fh, data->data.vector.rvec, data->data.vector.natoms);
    break;
  case INT_T:
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


int guamps_pick_filetype(const char *path, filetype_t *type) {
  const char
    *cpt = ".cpt",
    *tpr = ".tpr";

  const char *suffix = strrchr(path, '.');

  if (suffix == NULL){
    return false;
  } else if (0 == strcmp(suffix, cpt)) {
    *type = CPT;
    return true;
  } else if (0 == strcmp(suffix, tpr)) {
    *type = TPR;
    return true;
  } else {
    return false;
  }

}

int guamps_read_tpr_X(const char *path, const selector_t sel, gmx_data_t *result) {

  t_inputrec ir;
  t_state st;
  rvec *forces;
  gmx_mtop_t mtop;

  init_inputrec(&ir);
  init_state(&st, -1, -1, -1, -1);
  init_mtop(&mtop);

  // check to see if the forces can be read
  t_tpxheader header;
  int version, generation;
  read_tpxheader(path, &header,
		 FALSE, // runtime error on version mismatch
		 &version, &generation);
  if (!header.bF) {
    forces = NULL;
  } else {
    snew(forces, header.natoms);
  }

  // read the data
  read_tpx_state(path, &ir, &st, forces, &mtop);


  // store the result
  if (sel == FORCES && header.bF) {
    result->type	       = RVEC_T;
    result->data.vector.rvec   = forces;
    result->data.vector.natoms = header.natoms;
    return true;
  } else {
    return guamps_get_state_X(&st, sel, result);
  }

}


void guamps_error(const char *str) {
  const char *extra = "[GUAMPS] ";
  char *msg = (char *) calloc (strlen(extra) + strlen(str), sizeof(char));
  strcat(msg, extra);
  strcat(msg, str);
  fputs(msg, stderr);
}
