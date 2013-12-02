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

typedef struct gmx_data_s {
  enum type_e { RVEC_T, INT_T, RNG_T } type;
  union data_u {
    gmx_rvec_t vector;
    int number;
    gmx_rng_t rng;
  } data;
} gmx_data_t;

typedef enum selector_e {
  NATOMS, POSITIONS, VELOCITIES, FORCES, LAMBDA, BOX, STEP, TIME, RNG
} selector_t;

typedef enum filetype_e {
  CPT, TPR
} filetype_t;

gmx_rvec_t * guamps_init_gmx_rvec(int natoms);

#endif
