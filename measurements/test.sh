#!/bin/bash

#SBATCH --overcommit                        # Allow more processes than CPUs per node
#SBATCH -J concurrent_bellos_durmientes     # Job name
#SBATCH -o job.%j.txt                       # Stdout output file (%j expands to jobId)
#SBATCH -N 1                                # Total number of nodes requested
#SBATCH -n 1                                # Total number of processes requested
#SBATCH -t 02:00:00                         # Run time (hh:mm:ss)
#SBATCH -p normal                           # Partition to use (default, normal, debug)

perf stat prun bin/proyecto02 test/job001/job001.tsv
