#!/usr/bin/env bash

export GMX_MAXBACKUP=-1

pdb2gmx -f dipeptide.pdb -ff amber96 -water none
grompp -f run.mdp

rm -rf .mdq
mdq init gromacs -g 1 -t 200000 -o 10 -c 1 -s 42
mdq add -T topol.tpr -n ala.0
mdq prepare
