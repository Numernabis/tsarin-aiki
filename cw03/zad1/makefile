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
	#----------------------------------
	#Test [sys with forks]
	./program ../../cw02 '<' 2018-03-23 20:00:00

clean:
	rm -f *.o
