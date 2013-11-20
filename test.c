#include "stdio.h"

#include "gromacs/types/state.h"
#include "gromacs/random.h"
#include "gromacs/gmxfio.h"
#include "gromacs/statutil.h"
#include "gromacs/tpxio.h"
#include "gromacs/smalloc.h"


typedef int gmx_rng_t;

void seed(){

  gmx_rng_t rng;
  unsigned int n;
  int seed;

  seed = 42; // gmx_rng_make_seed();
  rng  = gmx_rng_init(seed);

  n = gmx_rng_uniform_uint32(rng);
  printf("%u\n", n);

  n = gmx_rng_uniform_uint32(rng);
  printf("%u\n", n);

}

void tpx(){
  const char *inpath  = "topol.tpr";
  const char *outpath = "new.tpr";

  t_tpxheader header;
  int version;
  int generation;

  read_tpxheader(inpath, &header, TRUE, &version, &generation);

  t_inputrec ir;
  t_state state;
  gmx_mtop_t mtop;

  init_inputrec(&ir);
  init_state(&state, -1, -1, -1, -1);
  init_mtop(&mtop);

  matrix box;
  int natoms;
  rvec *f;

  read_tpx_state(inpath,
		 &ir, &state, NULL,
		 &mtop);

  write_tpx_state(outpath,
		  &ir, &state, &mtop);

  
  int ignore = 42;

}

void cpt(){

  const char *inpath  = "state0.cpt";
  const char *outpath = "state1.cpt";

  


  int simulation_part;
  gmx_large_int_t step;
  double t;
  t_state state;
  t_commrec cr;

  init_state(&state, -1, -1, -1, -1);

  FILE *log = fopen("md.log", "w");

  read_checkpoint_state(inpath, &simulation_part, &step, &t, &state);

  write_checkpoint(outpath, FALSE,
  		   log, &cr,
  		   FALSE, simulation_part,
  		   step, t,
  		   &state);

  fclose(log);


/* void write_checkpoint(const char *fn,gmx_bool bNumberAndKeep, */
/* 			     FILE *fplog,t_commrec *cr, */
/* 			     int eIntegrator,int simulation_part, */
/* 			     gmx_large_int_t step,double t, */
/* 			     t_state *state); */


}


void both(){
  const char 
    *intopol  = "topol.tpr",
    *incpt    = "state0.cpt",
    *outtopol = "new.tpr";
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


  // update the state to be written
  /* gmx_rng_t rng = gmx_rng_init(seed); */
  /* unsigned int rng_state, rng_index; */
  /* gmx_rng_get_state(rng, &rng_state, &rng_index); */

  ir.ld_seed = 42;
  ir.init_t = time;
  ir.init_step = step;
  ir.init_lambda = cpt_state.lambda;
  

  // write the new tpr
  write_tpx_state(outtopol,
		  &ir, &cpt_state, &mtop);

}


int main(int argc, char *argv[]){

  set_program_name("seed gmx");

  both();

  return 0;
}
