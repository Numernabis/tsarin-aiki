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
	#---------------------------------------
	./program test_batch 10 30

clean:
	rm -f *.o
