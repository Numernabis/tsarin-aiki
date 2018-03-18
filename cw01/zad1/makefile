#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
LIB= library

#Default rule
all: static shared

static: $(LIB).c
	#Compile static library and build it
	$(CC) -c $(LIB).c -o $(LIB).o $(CFLAGS)
	ar rcs $(LIB).a $(LIB).o
	make clean

shared: $(LIB).c
	#Compile shared library
	$(CC) -fPIC -c $(LIB).c -o $(LIB).o $(CFLAGS)
	$(CC) -fPIC -shared -o lib$(LIB).so $(LIB).o $(CFLAGS)
	make clean

clean:
	rm -f *.o
