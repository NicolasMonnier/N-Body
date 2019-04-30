#!/bin/bash

#SBATCH -J VECMA
#SBATCH -N 1
#SBATCH -p SKL-16c_edr-ib1_192gb_2666
#SBATCH --licenses=scratch_lustre_DDN7k*5
#SBATCH --time=1:0:0
#SBATCH -c 64 
#SBATCH --exclusive

source /home_nfs_robin_ib/bmonniern/VECMA/build_EasyVVUQ_intel_19.1.144/env_bench

cd /scratch_lustre_DDN7k/bmonniern/VECMA/EasyVVUQ

python3 run_workflow_nbody.py > log_2.out

python3 run_workflow_nbody.py > log_3.out

