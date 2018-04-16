#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= main

#Default rule
all: program clean

program: $(PROG).c
	#Compile main
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f *.o
