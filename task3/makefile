# MAKEFILE for IZP course, 3rd project
#######################################
# Author:	David Kaspar (aka Dee'Kej)
#			FIT VUT Brno, 1BIT
# E-mail:	xkaspa34@stud.fit.vutbr.cz
# Date:		28-11-2011
#######################################


# Compiler
CC=gcc

# Parameters of compilation.
CFLAGS=-std=c99 -pedantic -W -Wall -Wextra -O

# Dependencies:
proj3: proj3.o proj3_matrlib.o
	$(CC) proj3.o proj3_matrlib.o -o $@

proj3.o: proj3.c proj3_matrlib.h
	$(CC) $(CFLAGS) -c proj3.c -o $@

proj3_matrlib.o: proj3_matrlib.c proj3_matrlib.h
	$(CC) $(CFLAGS) -c proj3_matrlib.c -o $@


#Rule to mark "false-positive" targets in project fodler.
.PHONY: clean

#Remove object files generated when compiling.
clean:
	rm -f *.o proj3

	
 
