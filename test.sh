#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Nespravny pocet argumentov, spravny je 1"
    exit 1
fi

list=$1
size=${#list}
let proc_no="(2*$size)-2"

mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp

mpirun --prefix /usr/local/share/OpenMPI -np $proc_no pro $1

rm -f pro
