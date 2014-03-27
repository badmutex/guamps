#!/bin/sh
set -ex

name=gromacs
version=4.5.7
pkg=$name-$version

wget ftp://ftp.gromacs.org/pub/gromacs/$pkg.tar.gz
tar xf $pkg.tar.gz
cd $pkg
./configure --prefix=/usr && make && sudo make install
ls /usr/bin
which gmxdump
