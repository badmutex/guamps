#ifndef __GUAMPS_SELECT_H__
#define __GUAMPS_SELECT_H__

#include "data.h"

typedef enum {
  NATOMS, POSITIONS, VELOCITIES,
  FORCES, LAMBDA, BOX,
  SEED, NSTLOG, NSTXOUT, NSTVOUT, NSTFOUT,
  NSTEPS, LD_SEED,
  DELTAT,
  NSTXTCOUT,
  STEP,
  TIME,
  TAU_T,
  selector_t_LAST
} selector_t;
static const int selector_t_count = selector_t_LAST;

static const char *GUAMPS_SELECTOR_NAMES[] =
  {[NATOMS]="NATOMS", [POSITIONS]="POSITIONS", [VELOCITIES]="VELOCITIES",
   [FORCES]="FORCES", [LAMBDA]="LAMBDA", [BOX]="BOX",
   [SEED]="SEED", [NSTLOG]="NSTLOG", [NSTXOUT]="NSTXOUT", [NSTVOUT]="NSTVOUT", [NSTFOUT]="NSTFOUT",
   [NSTEPS]="NSTEPS", [LD_SEED]="LD_SEED",
   [DELTAT]="DELTAT",
   [NSTXTCOUT]="NSTXTCOUT",
   [STEP]="STEP",
   [TIME]="TIME",
   [TAU_T]="TAU_T"
  };

static const type_t GUAMPS_SELECTOR_TYPES[] =
  {[NATOMS]=INT_T,  [POSITIONS]=RVEC_T, [VELOCITIES]=RVEC_T,
   [FORCES]=RVEC_T, [LAMBDA]=FLOAT_T, [BOX]=MATRIX_T,
   [SEED]=INT_T, [NSTLOG]=INT_T, [NSTXOUT]=INT_T, [NSTVOUT]=INT_T, [NSTFOUT]=INT_T,
   [NSTEPS]=LLINT_T, [LD_SEED]=INT_T,
   [DELTAT]=DOUBLE_T,
   [NSTXTCOUT]=INT_T,
   [STEP]=INT_T,
   [TAU_T]=RVEC_T
  };

type_t guamps_selector_type(const filetype_t, const selector_t);

#endif
