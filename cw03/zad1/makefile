#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= main

#Default rule
all: build tests clean

build: $(PROG).c
	#Compile main
	$(CC) -o program $(PROG).c $(CFLAGS)

tests:
	#----------------------------------
	#Test [sys with forks]
	./program ../../cw02 '<' 2018-03-23 20:00:00

clean:
	rm -f *.o *.a
