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
    res->type = FLOAT_T;
    res->value.v_float = trr->header.t;
    break;
  default:
    guamps_error("guamps_select_trr: cannot select %s from trr file\n", GUAMPS_SELECTOR_NAMES[sel]);
    ok = false;
    break;
  }

  return ok;
}

/* *********************************************************************
   Writing to files
 ***********************************************************************/
bool guamps_write(FILE *fh, const data_t *data) {
  switch(data->type) {
  case RVEC_T:
    return guamps_write_rvec(fh, data->value.v_rvec.rvec, data->value.v_rvec.length);
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

bool guamps_write_tpr(const char *path, const tpr_t *tpr); // TODO
bool guamps_write_cpt(const char *path, const cpt_t *cpt); // TODO

bool guamps_write_scalar(FILE *fh, const data_t *data) {
  char *fmt;
  switch(data->type) {
  case INT_T:
    fprintf(fh, "%d\n", data->value.v_int); break;
  case FLOAT_T:
    fprintf(fh, "%f\n", data->value.v_float); break;
  default:
    guamps_error("guamps_write_scalar: unknown scalar type %s\n", GUAMPS_TYPE_NAMES[data->type]);
    return false;
    break;
  }
  return true;
}

bool guamps_write_rvec(FILE *fh, const rvec *vec, const int length) {

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
  else {
    return false;
  }
  return true;
}

bool guamps_selector_type(const selector_t sel, type_t *type) {
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
