#ifndef __GUAMPS_GMX_T_H__
#define __GUAMPS_GMX_T_H__

#include "gromacs/types/inputrec.h"
#include "gromacs/types/state.h"
#include "gromacs/types/commrec.h"
#include "gromacs/trnio.h"

typedef struct {
  t_commrec  commrec;
  t_inputrec inputrec;
  t_state    state;
} cpt_t;

typedef struct {
  int natoms;
  t_inputrec inputrec;
  t_state state;
  rvec *f;
  gmx_mtop_t mtop;
} tpr_t;

typedef struct {
  t_trnheader header;
  matrix box;
  rvec *x, *v, *f;
} trr_t;

tpr_t * guamps_init_tpr(const int natoms);
void guamps_free_tpr(tpr_t *tpr);

trr_t * guamps_init_trr(const int natoms);
void guamps_free_trr(trr_t *trr);

#endif
