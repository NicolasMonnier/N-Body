#!/bin/bash

#SBATCH -p SKL-16c_edr-ib1_192gb_2666
#SBATCH -N 1
#SBATCH -J VECMA
#SBATCH --exclusive

source /home_nfs_robin_ib/bmonniern/VECMA/build_PilotJobManager_gnu_8.2.0/env_bench
source /home_nfs_robin_ib/xraffine/bin/setenv_gcc-6.4.0.sh

cd /home_nfs_robin_ib/bmonniern/VECMA/build_PilotJobManager_gnu_8.2.0/QCG-PilotJob-master/workspace

source /home_nfs_robin_ib/bmonniern/VECMA/env_bench
export OMP_NUM_THREADS=16

python3 -m qcg.appscheduler.service --net --file --file-path api-client.json --nodes 16
