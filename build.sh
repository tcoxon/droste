#!/bin/sh
CFL='-O3 -g'
LFL='-lm'

gcc -c main.c -o main.o $CFL
gcc -c droste.c -o droste.o $CFL
gcc droste.o main.o -o droste $LFL
