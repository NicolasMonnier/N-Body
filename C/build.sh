#!/bin/sh

echo "Source the environment for the Intel compiler"
source /opt/intel/compilers_and_libraries_2016.0.109/linux/bin/compilervars.sh intel64

echo "Source the environment for the Intel MPI"
source /opt/intel/impi_5.0.3/bin64/mpivars.sh

echo "Build"
make
