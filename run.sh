#!/bin/bash

# script for testing on home pc
if [[ $# -ne 2 ]]; then
    echo "wrong numbers of arguments"
    exit 1
fi

mpirun -np $2 ./out/$1