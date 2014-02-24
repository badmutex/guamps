#ifndef __GUAMPS_SELECT_H__
#define __GUAMPS_SELECT_H__

typedef enum {
  NATOMS, POSITIONS, VELOCITIES,
  FORCES, LAMBDA, BOX, STEP, TIME,
  SEED, NSTLOG, NSTXOUT, NSTVOUT, NSTFOUT,
  NSTEPS,
  selector_t_LAST
} selector_t;
static const int selector_t_count = selector_t_LAST - 1;

static const char *GUAMPS_SELECTOR_NAMES[] =
  {[NATOMS]="NATOMS", [POSITIONS]="POSITIONS", [VELOCITIES]="VELOCITIES",
   [FORCES]="FORCES", [LAMBDA]="LAMBDA", [BOX]="BOX", [STEP]="STEP", [TIME]="TIME",
   [SEED]="SEED", [NSTLOG]="NSTLOG", [NSTXOUT]="NSTXOUT", [NSTVOUT]="NSTVOUT", [NSTFOUT]="NSTFOUT",
   [NSTEPS]="NSTEPS",
  };

#endif
