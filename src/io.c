#include "stdbool.h"

#include "io.h"

#include "gromacs/smalloc.h"
#include "gromacs/trnio.h"


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
  // RNG not supported
  else if (0 == strcmp(str, "nstlog")) { *sel = NSTLOG; }
  else if (0 == strcmp(str, "nstxout")){ *sel = NSTXOUT; }
  else if (0 == strcmp(str, "nstvout")){ *sel = NSTVOUT; }
  else if (0 == strcmp(str, "nstfout")){ *sel = NSTFOUT; }
  else {
    return false;
  }
  return true;
}

int guamps_read_checkpoint_X(const char *path, const selector_t selector, guamps_data_t *result) {

  int part;
  gmx_large_int_t step;
  double time;
  t_state state;
  init_state(&state, -1, -1, -1, -1);

  read_checkpoint_state(path, &part, &step, &time, &state);

  return guamps_get_state_X(&state, selector, result);
}

int guamps_get_state_X(const t_state *st, const selector_t sel, guamps_data_t *result) {

  int ret = true;

  switch (sel) {
  case NATOMS:
    result->type        = INT_T;
    result->data.v_int = st->natoms;
    break;
  case POSITIONS:
    result->type	     = RVEC_T;
    result->data.v_rvec.rvec   = st->x;
    result->data.v_rvec.natoms = st->natoms;
    break;
  case VELOCITIES:
    result->type             = RVEC_T;
    result->data.v_rvec.rvec   = st->v;
    result->data.v_rvec.natoms = st->natoms;
    break;
  // forces not stored in t_state
  case LAMBDA:
    result->type         = FLOAT_T;
    result->data.v_float = st->lambda;
    break;
  case BOX:
    result->type = RVEC_T;
    result->data.v_rvec.rvec = (rvec *)st->box;
    result->data.v_rvec.natoms = 3;
    break;
  // step not stored in t_state
  // time not stored in t_state
  // rng access is buggy, so don't support it
  default:
    guamps_error("guamps_get_state_X: Getting %s field from GMX t_state is not supported\n",
		 GUAMPS_SELECTOR_NAMES[sel]);
    ret = false;
    break;
  }

  return ret;

}

int guamps_write(FILE *fh, const guamps_data_t *data) {
  switch(data->type) {
  case RVEC_T:
    return guamps_write_rvec(fh, data->data.v_rvec.rvec, data->data.v_rvec.natoms);
    break;
  case INT_T:
    return guamps_write_scalar(fh, data);
    break;
  case FLOAT_T:
    return guamps_write_scalar(fh, data);
  default:
    guamps_error("guamps_write: unknown type %s\n", GUAMPS_TYPE_NAMES[data->type]);
    return false;
    break;
  }
  return true;
}

int guamps_write_scalar(FILE *fh, const guamps_data_t *data) {
  char *fmt;
  switch(data->type) {
  case INT_T:
    fprintf(fh, "%d\n", data->data.v_int); break;
  case FLOAT_T:
    fprintf(fh, "%f\n", data->data.v_float); break;
  default:
    guamps_error("guamps_write_scalar: unknown scalar type %s\n", GUAMPS_TYPE_NAMES[data->type]);
    return false;
    break;
  }
  return true;
}

int guamps_write_rvec(FILE *fh, const rvec *vec, const int length) {

  if(vec == NULL) {
    guamps_error("cannot write NULL vector\n");
    return false;
  }

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

int guamps_read_tpr_X(const char *path, const selector_t sel, guamps_data_t *res) {

  tpr_t *tpr = guamps_load_tpr(path);
  int ret = true;

  switch(sel) {
  case NATOMS:
    res->type       = INT_T;
    res->data.v_int = tpr->natoms;
    break;
  case POSITIONS:
    res->type               = RVEC_T;
    res->data.v_rvec.rvec   = tpr->state.x;
    res->data.v_rvec.natoms = tpr->natoms;
    break;
  case VELOCITIES:
    res->type               = RVEC_T;
    res->data.v_rvec.rvec   = tpr->state.v;
    res->data.v_rvec.natoms = tpr->natoms;
    break;
  case FORCES:
    res->type               = RVEC_T;
    res->data.v_rvec.rvec   = tpr->f;
    res->data.v_rvec.natoms = tpr->natoms;
    break;
  case LAMBDA:
    res->type         = FLOAT_T;
    res->data.v_float = tpr->state.lambda;
    break;
  case BOX:
    res->type               = RVEC_T;
    res->data.v_rvec.rvec   = (rvec *) tpr->state.box;
    res->data.v_rvec.natoms = 3;
    break;
  case NSTLOG:
    res->type       = INT_T;
    res->data.v_int = tpr->inputrec.nstlog;
    break;
  case NSTXOUT:
    res->type       = INT_T;
    res->data.v_int = tpr->inputrec.nstxout;
    break;
  case NSTVOUT:
    res->type       = INT_T;
    res->data.v_int = tpr->inputrec.nstvout;
    break;
  case NSTFOUT:
    res->type       = INT_T;
    res->data.v_int = tpr->inputrec.nstfout;
    break;
  default:
    guamps_error("guamps_read_tpr_X: getting %s from tpr not supported\n", GUAMPS_SELECTOR_NAMES[sel]);
    ret = false;
    break;
  }

  guamps_free_tpr(tpr);

  return ret;

}


void guamps_error(const char *str, ...) {
  const char *extra = "[GUAMPS] ";
  char *msg = (char *) calloc (strlen(extra) + strlen(str), sizeof(char));
  strcat(msg, extra);
  strcat(msg, str);

  va_list args;
  va_start(args, str);
  vfprintf(stderr, msg, args);
  va_end(args);
}


/**
   Read the header of a vector file.

   Use `fgets` to read `size` bytes from `stream` into `buffer` and
   call `sscanf` to parse `expected` values given in `fmt` from `buffer` to `dst`

   @param name: the name of this portion of header for the error message on failure
   @param buffer: where to store the data
   @param size: buffer size
   @param stream: the file handle to read from
   @param fmt: format to parse
   @param dst: store parsed result here
   @param expected: expected number of parsed values (currently must be 1)
   @return `true` for success, `false` otherwise` after printing message to stderr
 */
static int guamps_read_vector_header(const char *name,
				           char *buffer,
				     const int   size,
				           FILE *stream,
				     const char *fmt,
				           void *dst,
				     const int   expected) {

  if(fgets(buffer, size, stream) == NULL){
    guamps_error("guamps_read_vector_header: Error reading '%s' from header\n", name);
    return false;
  } else if (sscanf(buffer, fmt, dst) != expected) {
    guamps_error("guamps_read_vector_header: Error parsing header '%s' for %d values from string: '%s'\n", name, expected, buffer);
    return false;
  } else {
    return true;
  }

}

/**
   Read a vector data file.
 */
int guamps_read_rvec(FILE *fh, guamps_data_t *data) {

  int ncells, ncoords, ndims;
  const int buffer_size = 100;
  char buffer[buffer_size];

  // header values: ncells, ncoords, ndims
  if (
      !guamps_read_vector_header("ncells" , buffer, buffer_size, fh, "ncells: %d" , &ncells , 1)
      ||
      !guamps_read_vector_header("ncoords", buffer, buffer_size, fh, "ncoords: %d", &ncoords, 1)
      ||
      !guamps_read_vector_header("ndims"  , buffer, buffer_size, fh, "ndims: %d"  , &ndims  , 1))
    { return false; }

  // empty line
  if (fgets(buffer, buffer_size, fh) == NULL) { perror("Error clearing empty line"); return false; }

  // result value
  data->type = RVEC_T;
  data->data.v_rvec = *guamps_init_gmx_rvec(ncoords);
  rvec *vec = data->data.v_rvec.rvec;


  // parse values
  int
    lineno = 0,
    coord  = 0,
    dim    = 0;
  float val;

  while (!feof(fh)) {

    fgets(buffer, buffer_size, fh);
    if (sscanf(buffer, "%f", &val) != 1) {
      guamps_error("guamps_read_rvec: Failed to parse value from value %d: '%s'\n", lineno+1, buffer);
      return false;
    }

    coord = lineno / ndims;
    dim   = lineno % ndims;
    vec[coord][dim] = val;
    lineno += 1;
  }

  // success!
  return true;
}

int guamps_read_int(FILE *fh, guamps_data_t *data) {
  const int buffer_size = 100;
  char buffer[buffer_size];

  data->type = INT_T;
  data->data.v_int = 0;

  fgets(buffer, buffer_size, fh);
  if (sscanf(buffer, "%d", &data->data.v_int) != 1) {
    guamps_error("guamps_read_int: Failed to parse integer: '%s'\n", buffer);
    return false;
  }

  return true;
}


int guamps_selector_type(const selector_t sel, type_t *type) {
  int retval = true;
  switch(sel) {
  case NATOMS:
    *type = INT_T;
    break;
  case POSITIONS:
    *type = RVEC_T;
    break;
  case VELOCITIES:
    *type = RVEC_T;
    break;
  case FORCES:
    *type = RVEC_T;
    break;
  case LAMBDA:
    *type = INT_T;
    break;
  default:
    guamps_error("guamps_selector_type: Unknown type for selector value %s\n", GUAMPS_SELECTOR_NAMES[sel]);
    retval = false;
    break;
  }

  return retval;

}

int guamps_read(FILE *fh, const selector_t sel, guamps_data_t *data) {

  type_t type;
  if(!guamps_selector_type(sel, &type)) {guamps_error("guamps_read: Unable to choose parser for file\n"); return false;}

  switch(type) {
  case INT_T:
    return guamps_read_int(fh, data);
    break;
  case RVEC_T:
    return guamps_read_rvec(fh, data);
    break;
  default:
    guamps_error("guamps_read: I don't know how to read files of type %s\n", GUAMPS_TYPE_NAMES[type]);
    return false;
  }
}


int guamps_read_trr_X(const char *path, const unsigned int frame, const selector_t sel, guamps_data_t *result) {

  t_fileio *fh = open_trn(path, "r");

  t_trnheader h;
  gmx_bool bOK;
  fread_trnheader(fh, &h, &bOK);

  return true;

}


tpr_t *  guamps_load_tpr(const char *path) {
  tpr_t *tpr;

  // read the header to get natoms for initializing tpr_t
  t_tpxheader h;
  int ver, gen;
  read_tpxheader(path, &h, FALSE, &ver, &gen);
  tpr = guamps_init_tpr(h.natoms);

  // set forces to null if header indicates their absence, else segfault
  if (h.bF == 0) {tpr->f = NULL;}

  // don't use `read_tpx` b/c there isn't a corresponding `write_tpx`
  // function in the GMX tpxio.h api
  read_tpx_state(path,
		 &tpr->inputrec, &tpr->state, tpr->f,
		 &tpr->mtop);

  return tpr;
}


void guamps_save_tpr(const char *path, tpr_t *tpr) {
  write_tpx_state(path, &tpr->inputrec, &tpr->state, &tpr->mtop);
}
