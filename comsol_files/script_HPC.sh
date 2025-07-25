#!/bin/bash 
#SBATCH -p batch        # partition name
#SBATCH --nodes=1
#SBATCH --ntasks=128
#SBATCH --mem=440G      # Memory allocation
#SBATCH -t 70:00:00    # Time limit (hh:mm:ss)
#SBATCH --job-name=bubble_10
#SBATCH -o out-bubble_10.txt
#SBATCH -e error-bubble_10.txt

filename="3D_compressible_bubble_coal_movingmesh_10nm.mph"
echo "Start $filename"
comsol batch -np 128 -inputfile ${filename}.mph -outputfile ${filename}.mph -batchlog log_${filename}
