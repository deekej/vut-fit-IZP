# MAKEFILE for IZP course, 1st project
#######################################
# Author:	David Kaspar (aka Dee'Kej)
#			FIT VUT Brno, 1BIT
# E-mail:	xkaspa34@stud.fit.vutbr.cz
# Date:		19-04-2012
#######################################


# Compiler
CC=gcc

# Parameters of compilation.
CFLAGS=-std=c99 -pedantic -W -Wall -Wextra -O2

# Dependencies:
proj1: proj1.o proj1timeconv.o
	$(CC) proj1.o proj1timeconv.o -o $@

proj1.o: proj1.c proj1timeconv.h
	$(CC) $(CFLAGS) -c proj1.c -o $@

proj1timeconv.o: proj1timeconv.c proj1timeconv.h
	$(CC) $(CFLAGS) -c proj1timeconv.c -o $@

	
