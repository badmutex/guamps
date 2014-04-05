#!/usr/bin/env bash
# exit if any command fails
set -x
set -o errexit 

export PATH=$PWD:$PATH

# input files
x_i=x_i.gps
v_i=v_i.gps
t_i=t_i.gps
tpr=topol.tpr
cpus=cpus.gps

# output files
x_o=x_o.gps
v_o=v_o.gps
t_o=t_o.gps

# disable gromacs automatic backups
export GMX_MAXBACKUP=-1

# continue from previous positions, velocities, and time
guamps_set -f $tpr -s positions  -i $x_i
guamps_set -f $tpr -s velocities -i $v_i
guamps_set -f $tpr -s time       -i $t_i

# run with given number of processors
mdrun -nt $(cat $cpus) -s $tpr

# retrieve the positions, velocities, and time
guamps_get -f traj.trr -s positions  -o $x_o
guamps_get -f traj.trr -s velocities -o $v_o
guamps_get -f traj.trr -s time       -o $t_o

