#ifndef __GUAMPS_TYPES_H__
#define __GUAMPS_TYPES_H__

#include "gromacs/gmx_random.h"
#include "gromacs/types/state.h"
#include "gromacs/tpxio.h"
#include "gromacs/checkpoint.h"


typedef struct gmx_rvec_s {
  rvec *rvec;
  int  natoms;
} gmx_rvec_t;

/**
   Initialize a gromacs rvec with `natom`
 */
gmx_rvec_t * guamps_init_gmx_rvec(int natoms);

// hold the state of a tpr
typedef struct gmx_tpr_s {
  int        natoms;
  t_inputrec inputrec;
  t_state    state;
  rvec      *f;
  gmx_mtop_t mtop;
} tpr_t;

tpr_t * guamps_init_tpr(const int natoms);
void guamps_free_tpr(tpr_t *);

// used in the data
typedef enum type_e {
  RVEC_T, // gmx `rvec`
  INT_T,  // numbers
  RNG_T,  // gmx RNG
  FLOAT_T // float
} type_t;

static const char *GUAMPS_TYPE_NAMES[] =
  {[RVEC_T]="RVEC_T", [INT_T]="INT_T", [RNG_T]="RNG_T",
   [FLOAT_T]="FLOAT_T"
  };

// data payload
typedef union data_u {
  gmx_rvec_t v_rvec  ;  // RVEC_T
  int        v_int   ;  // INT_T
  gmx_rng_t  v_rng   ;  // RNG_T
  float      v_float ;  // FLOAT_T
} data_t;

// the ADT
typedef struct guamps_data_s {
  type_t type;
  data_t data;
} guamps_data_t;

typedef enum selector_e {
  NATOMS, POSITIONS, VELOCITIES,
  FORCES, LAMBDA, BOX, STEP, TIME,
  RNG, NSTLOG, NSTXOUT, NSTVOUT, NSTFOUT
} selector_t;

static const char *GUAMPS_SELECTOR_NAMES[] =
  {[NATOMS]="NATOMS", [POSITIONS]="POSITIONS", [VELOCITIES]="VELOCITIES",
   [FORCES]="FORCES", [LAMBDA]="LAMBDA", [BOX]="BOX", [STEP]="STEP", [TIME]="TIME",
   [RNG]="RNG", [NSTLOG]="NSTLOG", [NSTXOUT]="NSTXOUT", [NSTVOUT]="NSTVOUT", [NSTFOUT]="NSTFOUT",
  };

typedef enum filetype_e {
  CPT, TPR
} filetype_t;


#endif
