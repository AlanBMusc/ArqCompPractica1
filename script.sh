#!/bin/bash

#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
#SBATCH --job-name p1acg

gcc acp1.c -o acp1 -O0
gcc acp2.c -o acp2 -O0
gcc acp3.c -o acp3 -O0

NL1=768
NL2=20480

L1=$((NL1 / 2))
L2=$((NL1 * 3 / 2))
L3=$((NL2 / 2))
L4=$((NL2 * 3 / 4))
L5=$((NL2 * 2))
L6=$((NL2 * 4))
L7=$((NL2 * 8))

for i in {1..10}
do
    for D in 1 2 4 8 16
    do
        for L in $L1 $L2 $L3 $L4 $L5 $L6 $L7
        do
            R=$L

            ./acp1 $D $R $L >> double_indirecto.csv
            ./acp2 $D $R $L >> int_indirecto.csv
            ./acp3 $D $R $L >> double_directo.csv

        done
    done
done
