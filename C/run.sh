#!/bin/sh

NAME=toy_nBody

if test "x$N" == "x"; then
	N=2
fi

echo "Source the environment for the Intel compiler"
source /opt/intel/compilers_and_libraries_2016.0.109/linux/bin/compilervars.sh intel64

echo "Source the environment for the Intel MPI"
source /opt/intel/impi_5.0.3/bin64/mpivars.sh

test -x ${NAME} || echo "The binary ${NAME} is missing"
test -x ${NAME} || exit -1

echo "Run"
#srun --label --mpi=pmi2 -J TPMPI -N 1 -n $N -p E52650v3_htoff_mofed24 --reservation=stagetp ./${NAME} | dshbak -c
./${NAME} #| dshbak -c
