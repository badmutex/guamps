#!/usr/bin/env bash

set -ex

export GMX_MAXBACKUP=-1

pdb2gmx -f dipeptide.pdb -ff amber96 -water none
grompp -f run.mdp

names=(
    ld_seed
    nsteps
    nstxout
    nstxtcout
)
values=(
    7090
    200000000
    10000
    10000
)

# for i in `seq 0 $(( ${#names[@]} - 1 ))`; do
#     echo ${values[$i]} > value.gps
#     guamps_set -i value.gps -s ${names[$i]} -O -f topol.tpr
# done

rm -rf .mdq
mdq -vvvv init gromacs -g 1 -t 200000 -o 10 -c 1 -s 42
mdq -vvvv add -T topol.tpr -n ala.0
mdq -vvvv prepare
