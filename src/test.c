#include "io.h"

#include "stdio.h"

#include "gromacs/types/state.h"
#include "gromacs/checkpoint.h"
#include "gromacs/gmx_random.h"
#include "gromacs/gmxfio.h"
#include "gromacs/statutil.h"
#include "gromacs/tpxio.h"
#include "gromacs/trnio.h"
#include "gromacs/smalloc.h"


/*
  Test loading tpr
 */
void test6() {
  const char *path = "../topol.tpr";
  tpr_t *tpr = guamps_load_tpr(path);

  tpr->inputrec.nstxout = 100;
  tpr->inputrec.nstvout = 100;
  tpr->inputrec.nstfout = 100;
  tpr->inputrec.nstxtcout = 0;
  printf("%d\n", tpr->inputrec.nstxout);
  guamps_save_tpr("../topol2.tpr", tpr);
  guamps_free_tpr(tpr);
}

/*
  Test traj functions
 */
void test5() {

  const char *trrpath = "../sim/traj.trr";

  t_trnheader header;
  gmx_bool bOK;

  t_fileio *fh = open_trn(trrpath, "r");

    int step, natoms;
    real t, lambda;
    rvec *box, *x, *v, *f;


  int frame = 0;
  while(fread_trnheader(fh, &header, &bOK)) {
    box = (rvec *)calloc(3, sizeof(rvec));
    x   = (rvec *)calloc(header.natoms, sizeof(rvec));
    v   = (rvec *)calloc(header.natoms, sizeof(rvec));
    f   = (rvec *)calloc(header.natoms, sizeof(rvec));

    // `fread_trn` doesn't work so use `fread_htrn` instead
    if(fread_htrn(fh, &header, box, x, v, f)) {
      printf("%5d %5d %10.2f %f\n", frame++, header.step, header.t, header.lambda);
      guamps_write_rvec(stdout, f, header.natoms);
    }

    free(box);
    free(x); free(v); free(f);
  }

}

/*
  Show that /read_checkpoint_state/ does not read in the state of the RNG
*/
void test4() {
  const char *fnin = "../state0.cpt";
  int simulation_part;
  gmx_large_int_t step;
  double t;
  t_state  *state = (t_state *)calloc(1, sizeof(t_state));

  init_state(state, -1,-1,-1,-1);

  read_checkpoint_state(fnin, &simulation_part,
			&step, &t, state);

  printf("%u\n", (unsigned int)state->ld_rng);
  

}

void test3() {

  const char *fn = "../state0.cpt";
  FILE *fplog = stderr;
  t_commrec cr;
  gmx_bool bPartDecomp = FALSE;
  ivec dd_nc;
  t_inputrec ir;
  t_state state;
  gmx_bool
    bReadRNG = TRUE,
    bReadEkin = TRUE,
    bAppend = TRUE,
    bForceAppend = FALSE;

  init_state(&state, -1,-1,-1,-1);
  init_inputrec(&ir);


  int simulation_part;
  gmx_large_int_t step;
  double t;


  read_checkpoint_state(fn, &simulation_part,
			&step, &t, &state);

  /* load_checkpoint(fn, &fplog, */
  /* 		  &cr, bPartDecomp, dd_nc, */
  /* 		  &ir, &state, */
  /* 		  &bReadRNG, &bReadEkin, */
  /* 		  bAppend, bForceAppend); */

  printf("%u\n", *state.ld_rng);

}


void test2() {
  const char 
    *intopol  = "../topol.tpr",
    *incpt    = "../state0.cpt",
    *outtopol = "../new.tpr";
  const unsigned int seed = 42;

  // read the header
  t_tpxheader header;
  int version, generation;
  read_tpxheader(intopol, &header, FALSE, &version, &generation);

  // decide if forces can be read from tpr
  rvec *forces;


  // read the topology file
  t_inputrec ir;
  t_state topol_state;
  gmx_mtop_t mtop;

  // read the checkpoint
  int sim_part;
  gmx_large_int_t step;
  double time;
  t_state cpt_state;
  t_commrec comm;

  init_state(&cpt_state, -1, -1, -1, -1);

  printf("Opening %s for reading\n", incpt);
  read_checkpoint_state(incpt, &sim_part, &step, &time, &cpt_state);


  printf("ld_rng  = %d\n", (int) cpt_state.ld_rng);
  printf("ld_rngi = %d\n"  , (int) cpt_state.ld_rngi);

}


// works
void test1() {
  const char 
    *intopol  = "../topol.tpr",
    *incpt    = "../state0.cpt",
    *outtopol = "../new.tpr";
  const unsigned int seed = 42;

  // read the header
  t_tpxheader header;
  int version, generation;
  read_tpxheader(intopol, &header, FALSE, &version, &generation);

  // decide if forces can be read from tpr
  rvec *forces;

  if (header.bF) {
    snew(forces, 1);
  } else {
    forces = NULL;
  }

  // read the topology file
  t_inputrec ir;
  t_state topol_state;
  gmx_mtop_t mtop;

  init_inputrec(&ir);
  init_state(&topol_state, -1, -1, -1, -1);
  init_mtop(&mtop);

  read_tpx_state(intopol,
		 &ir, &topol_state, forces,
		 &mtop);

  // read the checkpoint
  int sim_part;
  gmx_large_int_t step;
  double time;
  t_state cpt_state;
  t_commrec comm;

  init_state(&cpt_state, -1, -1, -1, -1);

  printf("Opening %s for reading\n", incpt);
  read_checkpoint_state(incpt, &sim_part, &step, &time, &cpt_state);


  printf("ld_rng  = %d\n", (int) cpt_state.ld_rng);
  printf("ld_rngi = %d\n"  , (int) cpt_state.ld_rngi);


  // update the state to be written
  /* gmx_rng_t rng = gmx_rng_init(seed); */
  /* unsigned int rng_state, rng_index; */
  /* gmx_rng_get_state(rng, &rng_state, &rng_index); */

  ir.ld_seed = 42;
  ir.init_t = time;
  ir.init_step = step;
  ir.init_lambda = cpt_state.lambda;
  

  /* // write the new tpr */
  /* write_tpx_state(outtopol, */
  /* 		  &ir, &cpt_state, &mtop); */

}

// fails
void test0() {

  const char *path = "../state0.cpt";

  int sim_part;
  gmx_large_int_t step;
  double time;
  t_state state;

  init_state(&state, -1, -1, -1, -1);
  
  read_checkpoint_state(path, &sim_part, &step, &time, &state);
}


int main(int argc, char *argv[]) {
  set_program_name(argv[0]);
  test5();
  return 0;
}
