# MAKEFILE for IZP course, 2st project
#######################################
# Author:	David Kaspar (aka Dee'Kej)
#			FIT VUT Brno, 1BIT
# E-mail:	xkaspa34@stud.fit.vutbr.cz
# Date:		20-11-2011
#######################################


# Compiler
CC=gcc

# Parameters of compilation.
CFLAGS=-std=c99 -pedantic -W -Wall -Wextra -O -lm

# Dependencies:
proj2: proj2.o proj2_mathlib.o proj2_geomlib.o
	$(CC) proj2.o proj2_mathlib.o proj2_geomlib.o -lm -o $@

proj2.o: proj2.c proj2_mathlib.h proj2_geomlib.h
	$(CC) $(CFLAGS) -c proj2.c -o $@

proj2_mathlib.o: proj2_mathlib.c proj2_mathlib.h
	$(CC) $(CFLAGS) -c proj2_mathlib.c -o $@

proj2_geomlib.o: proj2_geomlib.c proj2_geomlib.h proj2_mathlib.h
	$(CC) $(CFLAGS) -c proj2_geomlib.c -o $@

 	
