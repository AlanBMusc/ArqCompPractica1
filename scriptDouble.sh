#!/bin/bash
# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 00:15:00 --mem=16G
#SBATCH --job-name p1acgNN

# Compilación
gcc doublesCesga.c -o acp1 -O0

for i in {1..10}
do
    for D in 2 8 128 512 1024
    do
        for L in 384 1152 10240 15360 40960 81920 163840
        do
            ./acp1 $D $L >> double_indirecto.csv
        done
    done
done