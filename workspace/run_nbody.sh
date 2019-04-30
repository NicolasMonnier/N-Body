#!/bin/sh

NAME=toy_nBody
export OMP_NUM_THREADS=10

echo "Source the environment for the Intel compiler"
source /opt/intel/compilers_and_libraries_2016.0.109/linux/bin/compilervars.sh intel64


echo "Run"
export KMP_AFFINITY="verbose,granularity=fine" #,proclist=[0,1,2,3,4,5,6,7,8,9], explicit"
./${NAME} nbody_input.json #| dshbak -c
