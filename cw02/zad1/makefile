#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= main
LIB= operations
F= files/

#Default rule
all: static build sort_tests clean

static: $(LIB).c
	#Compile static library and build it
	$(CC) -c $(LIB).c -o $(LIB).o $(CFLAGS)
	ar rcs lib$(LIB).a $(LIB).o

build: $(PROG).c
	#Compile main
	$(CC) -o program $(PROG).c lib$(LIB).a $(CFLAGS)

sort_tests:
	#File: 1000 x 4B
	./program generate $(F)1k2 1000 4 sys
	#Test: copying file
	./program copy $(F)1k2 $(F)1k2s 1000 4 sys
	./program copy $(F)1k2 $(F)1k2l 1000 4 lib
	#Test: sorting file
	./program sort $(F)1k2 1000 4 sys
	./program sort $(F)1k2s 1000 4 lib

clean:
	rm -f *.o *.a
