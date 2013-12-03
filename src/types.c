#include "types.h"


gmx_rvec_t * guamps_init_gmx_rvec(int natoms) {
  gmx_rvec_t	*vec = (gmx_rvec_t *)calloc(1     , sizeof(gmx_rvec_t));
  vec->rvec	     = (rvec *)      calloc(natoms, sizeof(rvec));
  vec->natoms	     = natoms;
  return vec;
}


tpr_t * guamps_init_tpr(const int natoms) {
  tpr_t *tpr = (tpr_t *)calloc(1, sizeof(tpr_t));
  tpr->natoms = natoms;
  init_inputrec (&tpr->inputrec);
  init_state    (&tpr->state, -1,-1,-1,-1);
  tpr->f      = (rvec *)calloc(natoms, sizeof(rvec));
  init_mtop     (&tpr->mtop);
  return tpr;
}


void guamps_free_tpr(tpr_t *tpr) {
  if (tpr) {
    if(tpr->f) free(tpr->f);
    free(tpr);
  }
}
