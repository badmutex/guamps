#include "guampsio.h"

#include "gromacs/trnio.h"
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
  res->type = guamps_selector_type(TPR_F, sel);

  switch(sel) {
  case NATOMS:
    guamps_data_set(res->type, (void*)&tpr->natoms, res);
    break;
  case POSITIONS:
    res->value.v_rvec.rvec   = tpr->state.x;
    res->value.v_rvec.length = tpr->natoms;
    break;
  case VELOCITIES:
    res->value.v_rvec.rvec   = tpr->state.v;
    res->value.v_rvec.length = tpr->natoms;
    break;
  case FORCES:
    res->value.v_rvec.rvec   = tpr->f;
    res->value.v_rvec.length = tpr->natoms;
    break;
  case LAMBDA:
    guamps_data_set(res->type, (void*)&tpr->state.lambda, res);
    break;
  case BOX:
    res->value.v_rvec.rvec   = (rvec *) tpr->state.box;
    res->value.v_rvec.length = 3;
    break;
  case NSTLOG:
    guamps_data_set(res->type, (void*)&tpr->inputrec.nstlog, res);
    break;
  case NSTXOUT:
    guamps_data_set(res->type, (void*)&tpr->inputrec.nstxout, res);
    break;
  case NSTVOUT:
    guamps_data_set(res->type, (void*)&tpr->inputrec.nstvout, res);
    break;
  case NSTFOUT:
    guamps_data_set(res->type, (void*)&tpr->inputrec.nstfout, res);
    break;
  case NSTEPS:
    guamps_data_set(res->type, (void*)&tpr->inputrec.nsteps, res);
    break;
  case TIME:
    guamps_data_set(res->type, (void*)&tpr->inputrec.init_t, res);
    break;
  case LD_SEED:
    guamps_data_set(res->type, (void*)&tpr->inputrec.ld_seed, res);
    break;
  case DELTAT:
    guamps_data_set(res->type, &tpr->inputrec.delta_t, res);
    break;
  case NSTXTCOUT:
    guamps_data_set(res->type, &tpr->inputrec.nstxtcout, res);
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
  res->type = guamps_selector_type(TRR_F, sel);
  rvec_t vec;

  switch(sel) {
  case NATOMS:
    guamps_data_set(res->type, &trr->header.natoms, res);
    break;
  case POSITIONS:
    vec.rvec = trr->x;
    vec.length = trr->header.natoms;
    guamps_data_set(res->type, &vec, res);
    break;
  case VELOCITIES:
    vec.rvec = trr->v;
    vec.length = trr->header.natoms;
    guamps_data_set(res->type, &vec, res);
    break;
  case FORCES:
    vec.rvec = trr->f;
    vec.length = trr->header.natoms;
    guamps_data_set(res->type, &vec, res);
    break;
  case LAMBDA:
    guamps_data_set(res->type, &trr->header.lambda, res);
    break;
  case BOX:
    guamps_data_set(res->type, &trr->box, res);
    break;
  case TIME:
    guamps_data_set(res->type, &trr->header.t, res);
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
  rvec_t vec;

  /* when modifying the cases it is best to do it in several steps:
     1. add the new entry without casting. the compiler warning/error
        will act as a sanity check
     2. add the appropriate cast
   */

  switch(sel) {
  case NATOMS:
    tpr->state.natoms = *(int*)guamps_data_get(new);
    break;
  case POSITIONS:
    vec = *(rvec_t*)guamps_data_get(new);
    tpr->state.x = vec.rvec;
    break;
  case VELOCITIES:
    vec = *(rvec_t*)guamps_data_get(new);
    tpr->state.v = vec.rvec;
    break;
  case FORCES:
    vec = *(rvec_t*)guamps_data_get(new);
    tpr->f = vec.rvec;
    break;
  case LAMBDA:
    tpr->state.lambda = *(float*)guamps_data_get(new);
    break;
  case TIME:
    tpr->inputrec.init_t = *(double*)guamps_data_get(new);
    break;
  case LD_SEED:
    tpr->inputrec.ld_seed = *(int*)guamps_data_get(new);
    break;
  case NSTLOG:
    tpr->inputrec.nstlog = *(int*)guamps_data_get(new);
    break;
  case NSTXOUT:
    tpr->inputrec.nstxout = *(int*)guamps_data_get(new);
    break;
  case NSTVOUT:
    tpr->inputrec.nstvout = *(int*)guamps_data_get(new);
    break;
  case NSTFOUT:
    tpr->inputrec.nstfout = *(int*)guamps_data_get(new);
    break;
  case NSTEPS:
    tpr->inputrec.nsteps = *(long long int*)guamps_data_get(new);
    break;
  case DELTAT:
    tpr->inputrec.delta_t = *(double*)guamps_data_get(new);
    break;
  case NSTXTCOUT:
    tpr->inputrec.nstxtcout = *(int*)guamps_data_get(new);
    break;
  default:
    guamps_error("guamps_update_tpr: unknown selector %s\n", GUAMPS_SELECTOR_NAMES[sel]);
    ok = false;
    break;
  }

  return ok;
}

bool guamps_update_trr(trr_t *trr, const selector_t sel, const data_t *new) {

  bool ok = true;
  rvec_t vec;

  switch(sel) {
  case NATOMS:
    trr->header.natoms = *(int*)guamps_data_get(new);
    break;
  case POSITIONS:
    vec = *(rvec_t*)guamps_data_get(new);
    trr->x = vec.rvec;
    break;
  case VELOCITIES:
    vec = *(rvec_t*)guamps_data_get(new);
    trr->v = vec.rvec;
    break;
  case FORCES:
    vec = *(rvec_t*)guamps_data_get(new);
    trr->f = vec.rvec;
    break;
  case LAMBDA:
    trr->header.lambda = *(float*)guamps_data_get(new);
    break;
  case BOX:
    guamps_error("guamps_update_trr: don't yet know how to set BOX\n");
    ok = false;
    break;
  case TIME:
    trr->header.t = *(real*)guamps_data_get(new);
    break;
  case STEP:
    trr->header.step = *(int*)guamps_data_get(new);
    break;
  default:
    guamps_error("guamps_update_trr: unknown selector %s\n", GUAMPS_SELECTOR_NAMES[sel]);
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
  case TRR_F:
    return guamps_write_trr(path, &obj->data.p_trr);
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
  case MATRIX_T:
    return guamps_fwrite_rvec(fh, data->value.v_matrix, 3);
    break;
  case INT_T:
  case LLINT_T:
  case FLOAT_T:
  case DOUBLE_T:
    return guamps_fwrite_scalar(fh, data);
    break;
  case REAL_T:
    guamps_error("guamps_write: Unknown type REAL_T\n");
    return false;
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

bool guamps_write_trr(const char *path, const trr_t *trr) {
  trr_t *t = (trr_t *)trr;

  /*
  void write_trn(const char *fn,int step,real t,real lambda,
		       rvec *box,int natoms,rvec *x,rvec *v,rvec *f);
  */
  write_trn(path, t->header.step, t->header.t, t->header.lambda,
	    t->box, t->header.natoms, t->x, t->v, t->f);

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
  else if (0 == strcmp(str, "time")) { *sel = TIME; }
  // RNG not supported
  else if (0 == strcmp(str, "nstlog")) { *sel = NSTLOG; }
  else if (0 == strcmp(str, "nstxout")){ *sel = NSTXOUT; }
  else if (0 == strcmp(str, "nstvout")){ *sel = NSTVOUT; }
  else if (0 == strcmp(str, "nstfout")){ *sel = NSTFOUT; }
  else if (0 == strcmp(str, "nsteps")) { *sel = NSTEPS;  }
  else if (0 == strcmp(str, "ld_seed")){ *sel = LD_SEED; }
  else if (0 == strcmp(str, "deltat")) { *sel = DELTAT;  }
  else if (0 == strcmp(str, "nstxtcout")){*sel= NSTXTCOUT;}
  else if (0 == strcmp(str, "step"))   {*sel= STEP;}
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
