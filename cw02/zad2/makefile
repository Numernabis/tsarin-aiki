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
	#Test [sys]
	./program ../zad1 '<' 2018-03-18 20:00:00 sys
	#----------------------------------
	#Test [nftw]
	./program ../zad1 '<' 2018-03-18 20:00:00 nftw

clean:
	rm -f *.o *.a
