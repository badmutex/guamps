#include "gmx_t.h"

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


trr_t * guamps_init_trr(const int natoms) {
  trr_t *trr = (trr_t *)calloc(1, sizeof(trr_t));
  trr->x = (rvec *)calloc(natoms, sizeof(rvec));
  trr->v = (rvec *)calloc(natoms, sizeof(rvec));
  trr->f = (rvec *)calloc(natoms, sizeof(rvec));
  return trr;
}
void guamps_free_trr(trr_t *p) {
  if(p->x) free(p->x);
  if(p->v) free(p->v);
  if(p->f) free(p->f);
  free(p);
}
