#!/usr/bin/env bash

module load gromacs
module load guamps
module load homebrew # for go

go  build -gcflags "-N -l" -a gmx.go
