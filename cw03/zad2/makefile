#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= main

#Default rule
all: program test clean

program: $(PROG).c
	#Compile main
	$(CC) -o $@ $< $(CFLAGS)

test:
	#---------------------------------------
	./program test_batch

clean:
	rm -f *.o
