#!/bin/sh
gcc -c main.c -o main.o -O3 -g
gcc -c droste.c -o droste.o -O3 -g
gcc droste.o main.o -lm -o droste
