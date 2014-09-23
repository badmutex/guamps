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
  REF_T,
  selector_key_LAST
} selector_key;
static const int selector_key_count = selector_key_LAST;

static const char *GUAMPS_SELECTOR_NAMES[] =
  {[NATOMS]="NATOMS", [POSITIONS]="POSITIONS", [VELOCITIES]="VELOCITIES",
   [FORCES]="FORCES", [LAMBDA]="LAMBDA", [BOX]="BOX",
   [SEED]="SEED", [NSTLOG]="NSTLOG", [NSTXOUT]="NSTXOUT", [NSTVOUT]="NSTVOUT", [NSTFOUT]="NSTFOUT",
   [NSTEPS]="NSTEPS", [LD_SEED]="LD_SEED",
   [DELTAT]="DELTAT",
   [NSTXTCOUT]="NSTXTCOUT",
   [STEP]="STEP",
   [TIME]="TIME",
   [REF_T]="REF_T"
  };

static const type_t GUAMPS_SELECTOR_TYPES[] =
  {[NATOMS]=INT_T,  [POSITIONS]=RVEC_T, [VELOCITIES]=RVEC_T,
   [FORCES]=RVEC_T, [LAMBDA]=FLOAT_T, [BOX]=MATRIX_T,
   [SEED]=INT_T, [NSTLOG]=INT_T, [NSTXOUT]=INT_T, [NSTVOUT]=INT_T, [NSTFOUT]=INT_T,
   [NSTEPS]=LLINT_T, [LD_SEED]=INT_T,
   [DELTAT]=DOUBLE_T,
   [NSTXTCOUT]=INT_T,
   [STEP]=INT_T,
   [REF_T]=RVEC_T
  };


typedef unsigned long long index_t;

typedef struct {
  selector_key key;
  index_t* index;
} selector_t;

const selector_t* guamps_selector_t_create(const selector_key key, const index_t* index);

type_t guamps_selector_type(const filetype_t, const selector_t);

#endif
