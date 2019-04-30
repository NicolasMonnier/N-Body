#!/bin/bash

#SBATCH -p SKL-16c_edr-ib1_192gb_2666
#SBATCH -N 1
#SBATCH -J VECMA
#SBATCH --exclusive
#SBATCH -c 64
#SBATCH --nodelist genji240

set -ex -o pipefail

source /home_nfs_robin_ib/bmonniern/VECMA/build_PilotJobManager_gnu_8.2.0/env_bench


cd /home_nfs_robin_ib/bmonniern/VECMA/build_PilotJobManager_gnu_8.2.0/QCG-PilotJob-master/workspace

export OMP_NUM_THREADS=8

python3 -m qcg.appscheduler.service --net --file --file-path api-client-nbody.json  --wd /home_nfs_robin_ib/bmonniern/VECMA/build_PilotJobManager_gnu_8.2.0/QCG-PilotJob-master/workspace

