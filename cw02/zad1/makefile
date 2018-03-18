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
	#----------------------------------
	#File: 2000 x 4B
	./program generate $(F)2k2 2000 4 sys
	#Test: copying file
	./program copy $(F)2k2 $(F)2k2s 2000 4 sys
	./program copy $(F)2k2 $(F)2k2l 2000 4 lib
	#Test: sorting file
	./program sort $(F)2k2 2000 4 sys
	./program sort $(F)2k2s 2000 4 lib

	#----------------------------------
	#File: 6000 x 4B
	./program generate $(F)6k2 6000 4 sys
	#Test: copying file
	./program copy $(F)6k2 $(F)6k2s 6000 4 sys
	./program copy $(F)6k2 $(F)6k2l 6000 4 lib
	#Test: sorting file
	./program sort $(F)6k2 6000 4 sys
	./program sort $(F)6k2s 6000 4 lib

	#----------------------------------
	#File: 2000 x 512B
	./program generate $(F)2k9 2000 512 sys
	#Test: copying file
	./program copy $(F)2k9 $(F)2k9s 2000 512 sys
	./program copy $(F)2k9 $(F)2k9l 2000 512 lib
	#Test: sorting file
	./program sort $(F)2k9 2000 512 sys
	./program sort $(F)2k9s 2000 512 lib

	#----------------------------------
	#File: 6000 x 512B
	./program generate $(F)6k9 6000 512 sys
	#Test: copying file
	./program copy $(F)6k9 $(F)6k9s 6000 512 sys
	./program copy $(F)6k9 $(F)6k9l 6000 512 lib
	#Test: sorting file
	./program sort $(F)6k9 6000 512 sys
	./program sort $(F)6k9s 6000 512 lib

	#----------------------------------
	#File: 2000 x 4096B
	./program generate $(F)2k12 2000 4096 sys
	#Test: copying file
	./program copy $(F)2k12 $(F)2k12s 2000 4096 sys
	./program copy $(F)2k12 $(F)2k12l 2000 4096 lib
	#Test: sorting file
	./program sort $(F)2k12 2000 4096 sys
	./program sort $(F)2k12s 2000 4096 lib

	#----------------------------------
	#File: 6000 x 4096B
	./program generate $(F)6k12 6000 4096 sys
	#Test: copying file
	./program copy $(F)6k12 $(F)6k12s 6000 4096 sys
	./program copy $(F)6k12 $(F)6k12l 6000 4096 lib
	#Test: sorting file
	./program sort $(F)6k12 6000 4096 sys
	./program sort $(F)6k12s 6000 4096 lib

	#----------------------------------
	#File: 2000 x 8192B
	./program generate $(F)2k13 2000 8192 sys
	#Test: copying file
	./program copy $(F)2k13 $(F)2k13s 2000 8192 sys
	./program copy $(F)2k13 $(F)2k13l 2000 8192 lib
	#Test: sorting file
	./program sort $(F)2k13 2000 8192 sys
	./program sort $(F)2k13s 2000 8192 lib

	#----------------------------------
	#File: 6000 x 8192B
	./program generate $(F)6k13 6000 8192 sys
	#Test: copying file
	./program copy $(F)6k13 $(F)6k13s 6000 8192 sys
	./program copy $(F)6k13 $(F)6k13l 6000 8192 lib
	#Test: sorting file
	./program sort $(F)6k13 6000 8192 sys
	./program sort $(F)6k13s 6000 8192 lib

clean:
	rm -f *.o *.a
