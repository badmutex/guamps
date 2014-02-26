#include "guampsio.h"

#include "gromacs/tpxio.h"
#include "string.h"

/* *********************************************************************
   Reading from GROMACS files
 ***********************************************************************/
selectable_t *guamps_load(const char *path) {

  filetype_t ftype;
  if(!guamps_pick_filetype(path, &ftype)) {
    guamps_error("guamps_load: unable to choose filetype of %s\n", path);
    return NULL;
  }

  selectable_t *sel = (selectable_t *)calloc(1, sizeof(selectable_t));
  sel->kind = ftype;
  switch(ftype) {
  /* case CPT_F: */
  /*   sel->data.p_cpt = *guamps_load_cpt(path); */
  /*   break; */
  case TPR_F:
    sel->data.p_tpr = *guamps_load_tpr(path);
    break;
  case TRR_F:
    sel->data.p_trr = *guamps_load_trr(path);
    break;
  default:
    guamps_error("guamps_load: unable to load %s as %s file\n", path, ftype);
    return NULL;
  }

  return sel;
}

cpt_t * guamps_load_cpt(const char *path); // TODO

tpr_t * guamps_load_tpr(const char *path) {
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

trr_t * guamps_load_trr(const char *path) {
  trr_t *trr;

  t_trnheader h;
  read_trnheader(path, &h);
  trr = guamps_init_trr(h.natoms);

  t_fileio *fh = open_trn(path, "r");
  int bOK;

  while(fread_trnheader(fh, &trr->header, &bOK)) {
    fread_htrn(fh, &trr->header, (rvec *)&trr->box, trr->x, trr->v, trr->f);
  }

  close_trn(fh);
  return trr;
  
}

/* *********************************************************************
   Reading GUAMPS data
 ***********************************************************************/
bool guamps_fread(FILE *fh, const type_t type, data_t *data) {

  bool ok = true;

  data->type = type;

  switch(type) {
  case INT_T:
    ok = guamps_fread_scalar(fh, "%d", &data->value.v_int);
    break;
  case LLINT_T:
    ok = guamps_fread_scalar(fh, "%lld", &data->value.v_llint);
    break;
  case FLOAT_T:
    ok = guamps_fread_scalar(fh, "%f", &data->value.v_float);
    break;
  case DOUBLE_T:
    ok = guamps_fread_scalar(fh, "%lf", &data->value.v_double);
    break;
  case RVEC_T:
    ok = guamps_fread_rvec(fh, &data->value.v_rvec);
    break;
  default:
    guamps_error("guamps_fread: unknown type: %s\n", GUAMPS_TYPE_NAMES[type]);
    ok = false;
    break;
  }

  return ok;
}

bool guamps_fread_scalar(FILE *fh, const char *spec, void *value) {
  const int buffer_size = 100;
  char buffer[buffer_size];

  fgets(buffer, buffer_size, fh);
  if(sscanf(buffer, spec, value) != 1) {
    guamps_error("guamps_fread_scalar: failed to parse value: '%s'\n", buffer);
    return false;
  }
  return true;
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

bool guamps_fread_rvec(FILE *fh, rvec_t *value) {

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
  value->length = ncoords;
  value->rvec   = (rvec *)calloc(ncoords, sizeof(rvec));
  rvec *vec = value->rvec;


  // parse values
  int
    lineno = 0,
    coord  = 0,
    dim    = 0;
  float val;

  while (!feof(fh)) {

    fgets(buffer, buffer_size, fh);
    if (sscanf(buffer, "%f", &val) != 1) {
      guamps_error("guamps_fread_rvec: Failed to parse value from value %d: '%s'\n", lineno+1, buffer);
      return false;
    }

    coord = lineno / ndims;
    dim   = lineno % ndims;
    vec[coord][dim] = val;
    lineno += 1;
  }

  return true;
}

/* *********************************************************************
   Extracting data
 ***********************************************************************/
bool guamps_select    (const selectable_t *data, const selector_t sel, data_t *res) {
  switch(data->kind) {
  case CPT_F:
    return guamps_select_cpt(&data->data.p_cpt, sel, res);
    break;
  case TPR_F:
    return guamps_select_tpr(&data->data.p_tpr, sel, res);
    break;
  case TRR_F:
    return guamps_select_trr(&data->data.p_trr, sel, res);
    break;
  }
}

bool guamps_select_cpt(const cpt_t *cpt , const selector_t sel, data_t *res) {
  bool ok = true;

  switch(sel) {
  default:
    guamps_error("guamps_select_cpt: unknown selector %s for CPT file\n", GUAMPS_SELECTOR_NAMES[sel]);
    ok = false;
  }

  return ok;

}

bool guamps_select_tpr(const tpr_t *tpr , const selector_t sel, data_t *res) {
  int ret = true;

  switch(sel) {
  case NATOMS:
    res->type       = INT_T;
    res->value.v_int = tpr->natoms;
    break;
  case POSITIONS:
    res->type               = RVEC_T;
    res->value.v_rvec.rvec   = tpr->state.x;
    res->value.v_rvec.length = tpr->natoms;
    break;
  case VELOCITIES:
    res->type               = RVEC_T;
    res->value.v_rvec.rvec   = tpr->state.v;
    res->value.v_rvec.length = tpr->natoms;
    break;
  case FORCES:
    res->type               = RVEC_T;
    res->value.v_rvec.rvec   = tpr->f;
    res->value.v_rvec.length = tpr->natoms;
    break;
  case LAMBDA:
    res->type         = FLOAT_T;
    res->value.v_float = tpr->state.lambda;
    break;
  case BOX:
    res->type               = RVEC_T;
    res->value.v_rvec.rvec   = (rvec *) tpr->state.box;
    res->value.v_rvec.length = 3;
    break;
  case NSTLOG:
    res->type       = INT_T;
    res->value.v_int = tpr->inputrec.nstlog;
    break;
  case NSTXOUT:
    res->type       = INT_T;
    res->value.v_int = tpr->inputrec.nstxout;
    break;
  case NSTVOUT:
    res->type       = INT_T;
    res->value.v_int = tpr->inputrec.nstvout;
    break;
  case NSTFOUT:
    res->type       = INT_T;
    res->value.v_int = tpr->inputrec.nstfout;
    break;
  case NSTEPS:
    res->type	     = INT_T;
    res->value.v_int = tpr->inputrec.nsteps;
    break;
  case TIME:
    res->type = DOUBLE_T;
    res->value.v_double = tpr->inputrec.init_t;
    break;
  case LD_SEED:
    res->type = INT_T;
    res->value.v_int = tpr->inputrec.ld_seed;
    break;
  default:
    guamps_error("guamps_select_tpr: getting %s from tpr not supported\n", GUAMPS_SELECTOR_NAMES[sel]);
    ret = false;
    break;
  }

  return ret;
}

bool guamps_select_trr(const trr_t *trr , const selector_t sel, data_t *res) {

  bool ok = true;

  switch(sel) {
  case NATOMS:
    res->type = INT_T;
    res->value.v_int = trr->header.natoms;
    break;
  case POSITIONS:
    res->type = RVEC_T;
    res->value.v_rvec.rvec = trr->x;
    res->value.v_rvec.length = trr->header.natoms;
    break;
  case VELOCITIES:
    res->type = RVEC_T;
    res->value.v_rvec.rvec = trr->v;
    res->value.v_rvec.length = trr->header.natoms;
    break;
  case FORCES:
    res->type = RVEC_T;
    res->value.v_rvec.rvec = trr->f;
    res->value.v_rvec.length = trr->header.natoms;
    break;
  case LAMBDA:
    res->type = FLOAT_T;
    res->value.v_float = trr->header.lambda;
    break;
  case BOX:
    res->type = RVEC_T;
    res->value.v_rvec.rvec = (rvec *)trr->box;
    res->value.v_rvec.length = 3;
    break;
  case STEP:
    res->type = INT_T;
    res->value.v_int = trr->header.step;
    break;
  case TIME:
    res->type = DOUBLE_T;
    res->value.v_double = trr->header.t;
    break;
  default:
    guamps_error("guamps_select_trr: cannot select %s from trr file\n", GUAMPS_SELECTOR_NAMES[sel]);
    ok = false;
    break;
  }

  return ok;
}

/* *********************************************************************
   Updates
 ***********************************************************************/
bool guamps_update(selectable_t *obj, const selector_t sel, const data_t *new) {
  switch(obj->kind) {
  case TPR_F:
    return guamps_update_tpr(&obj->data.p_tpr, sel, new);
    break;
  default:
    guamps_error("guamps_update: I don't know how to update %s\n", GUAMPS_FILETYPE_NAMES[obj->kind]);
    return false;
  }
}

bool guamps_update_tpr(tpr_t *tpr, const selector_t sel, const data_t *new) {

  bool ok = true;

  switch(sel) {
  case NATOMS:
    if(!typecheck(INT_T,new->type)){ ok = false; break;}
    tpr->state.natoms = new->value.v_int;
    break;
  case POSITIONS:
    if(!typecheck(RVEC_T,new->type)) ok = false;
    tpr->state.x = new->value.v_rvec.rvec;
    break;
  case VELOCITIES:
    if(!typecheck(RVEC_T,new->type)) ok = false;
    tpr->state.v = new->value.v_rvec.rvec;
    break;
  case FORCES:
    if (!typecheck(RVEC_T, new->type)) ok = false;
    tpr->f = new->value.v_rvec.rvec;
    break;
  case LAMBDA:
    if(!typecheck(FLOAT_T, new->type)) ok = false;
    tpr->state.lambda = new->value.v_float;
    break;
  case STEP:
    if(!typecheck(LLINT_T, new->type)) ok = false;
    tpr->inputrec.init_step = new->value.v_llint;
    break;
  case TIME:
    if(!typecheck(DOUBLE_T, new->type)) ok = false;
    tpr->inputrec.init_t = new->value.v_double;
    break;
  case LD_SEED:
    if(!typecheck(INT_T, new->type)) ok = false;
    tpr->inputrec.ld_seed = new->value.v_int;
    break;
  case NSTLOG:
    if(!typecheck(LLINT_T, new->type)) ok = false;
    tpr->inputrec.nstlog = new->value.v_llint;
    break;
  case NSTXOUT:
    if(!typecheck(LLINT_T, new->type)) ok = false;
    tpr->inputrec.nstxout = new->value.v_llint;
    break;
  case NSTVOUT:
    if(!typecheck(LLINT_T, new->type)) ok = false;
    tpr->inputrec.nstvout = new->value.v_llint;
    break;
  case NSTFOUT:
    if(!typecheck(LLINT_T, new->type)) ok = false;
    tpr->inputrec.nstfout = new->value.v_llint;
    break;
  case NSTEPS:
    tpr->inputrec.nsteps = new->value.v_int;
    break;
  default:
    guamps_error("guamps_update_tpr: unknown selector %s\n", GUAMPS_SELECTOR_NAMES[sel]);
    ok = false;
    break;
  }

  return ok;
}


/* *********************************************************************
   Writing to files
 ***********************************************************************/
bool guamps_write(const char *path, const selectable_t *obj) {

  switch(obj->kind) {
  case TPR_F:
    return guamps_write_tpr(path, &obj->data.p_tpr);
    break;
  default:
    guamps_error("guamps_write: I don't know how to write a %s file\n", GUAMPS_FILETYPE_NAMES[obj->kind]);
    return false;
  }

}

bool guamps_fwrite(FILE *fh, const data_t *data) {
  switch(data->type) {
  case RVEC_T:
    return guamps_fwrite_rvec(fh, data->value.v_rvec.rvec, data->value.v_rvec.length);
    break;
  case INT_T:
  case LLINT_T:
  case FLOAT_T:
  case DOUBLE_T:
    return guamps_fwrite_scalar(fh, data);
  default:
    guamps_error("guamps_fwrite: unknown type %s\n", GUAMPS_TYPE_NAMES[data->type]);
    return false;
    break;
  }
  return true;
}

bool guamps_write_tpr(const char *path, const tpr_t *tpr) {

  // casting to ignore the `const` qualifier
  tpr_t *t = (tpr_t *)tpr;

  // gmx doesn't store forces in the tpr
  if(t->f) {
    guamps_warn("Forces not stored in TPR\n");
  }

  write_tpx_state(path,
		  &t->inputrec,
		  &t->state,
		  &t->mtop);

  return true;
}

bool guamps_write_cpt(const char *path, const cpt_t *cpt); // TODO

bool guamps_fwrite_scalar(FILE *fh, const data_t *data) {
  char *fmt;
  switch(data->type) {
  case INT_T:
    fprintf(fh, "%d\n", data->value.v_int); break;
  case LLINT_T:
    fprintf(fh, "%lld\n", data->value.v_llint); break;
  case FLOAT_T:
    fprintf(fh, "%f\n", data->value.v_float); break;
  case DOUBLE_T:
    fprintf(fh, "%f\n", data->value.v_double); break;
  default:
    guamps_error("guamps_write_scalar: unknown scalar type %s\n", GUAMPS_TYPE_NAMES[data->type]);
    return false;
    break;
  }
  return true;
}

bool guamps_fwrite_rvec(FILE *fh, const rvec *vec, const int length) {

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



/* *********************************************************************
   Parsing selection string from user
 ***********************************************************************/
bool guamps_pick_selector(const char *str, selector_t *sel) {
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
  else if (0 == strcmp(str, "nsteps")) { *sel = NSTEPS;  }
  else if (0 == strcmp(str, "ld_seed")){ *sel = LD_SEED; }
  else {
    guamps_error("guamps_pick_selector: unknown option: %s\n", str);
    return false;
  }
  return true;
}

bool guamps_pick_filetype(const char *path, filetype_t *ftype) {
  const char
    *cpt = ".cpt",
    *tpr = ".tpr",
    *trr = ".trr";

  const char *suffix = strrchr(path, '.');

  if (suffix == NULL){
    return false;
  } else if (0 == strcmp(suffix, cpt)) {
    *ftype = CPT_F;
    return true;
  } else if (0 == strcmp(suffix, tpr)) {
    *ftype = TPR_F;
    return true;
  } else if (0 == strcmp(suffix, trr)) {
    *ftype = TRR_F;
    return true;
  } else {
    return false;
  }

}
