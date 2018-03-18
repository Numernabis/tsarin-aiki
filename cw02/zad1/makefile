#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= main
LIB= operations

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
	./program generate 1k2 1000 4 sys
	#Test: copying file
	./program copy 1k2 1k2s 1000 4 sys
	./program copy 1k2 1k2l 1000 4 lib
	#Test: sorting file
	./program sort 1k2 1000 4 sys
	./program sort 1k2s 1000 4 lib

	#----------------------------------
	#File: 5000 x 4B
	./program generate 5k2 5000 4 sys
	#Test: copying file
	./program copy 5k2 5k2s 5000 4 sys
	./program copy 5k2 5k2l 5000 4 lib
	#Test: sorting file
	./program sort 5k2 5000 4 sys
	./program sort 5k2s 5000 4 lib

	#----------------------------------
	#File: 1000 x 512B
	./program generate 1k9 1000 512 sys
	#Test: copying file
	./program copy 1k9 1k9s 1000 512 sys
	./program copy 1k9 1k9l 1000 512 lib
	#Test: sorting file
	./program sort 1k9 1000 512 sys
	./program sort 1k9s 1000 512 lib

	#----------------------------------
	#File: 5000 x 512B
	./program generate 5k9 5000 512 sys
	#Test: copying file
	./program copy 5k9 5k9s 5000 512 sys
	./program copy 5k9 5k9l 5000 512 lib
	#Test: sorting file
	./program sort 5k9 5000 512 sys
	./program sort 5k9s 5000 512 lib

clean:
	rm -f *.o
