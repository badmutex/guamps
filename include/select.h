#ifndef __GUAMPS_SELECT_H__
#define __GUAMPS_SELECT_H__

typedef enum {
  NATOMS, POSITIONS, VELOCITIES,
  FORCES, LAMBDA, BOX, STEP, TIME,
  RNG, NSTLOG, NSTXOUT, NSTVOUT, NSTFOUT
} selector_t;

static const char *GUAMPS_SELECTOR_NAMES[] =
  {[NATOMS]="NATOMS", [POSITIONS]="POSITIONS", [VELOCITIES]="VELOCITIES",
   [FORCES]="FORCES", [LAMBDA]="LAMBDA", [BOX]="BOX", [STEP]="STEP", [TIME]="TIME",
   [RNG]="RNG", [NSTLOG]="NSTLOG", [NSTXOUT]="NSTXOUT", [NSTVOUT]="NSTVOUT", [NSTFOUT]="NSTFOUT",
  };

#endif
