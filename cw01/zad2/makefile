#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= main
LIB= ../zad1/library.a

#Default rule
all: build run clean

build: $(PROG).c
	#Compile
	$(CC) -o test $(PROG).c $(LIB) $(CFLAGS)

run:
	#Tests - dynamic allocation
	./test 5000 500 D init change 4000 find 444
	./test 5000 500 D init alt_change 3000 change 2000

	#Tests - static allocation
	./test 5000 500 S init change 4000 find 444
	./test 5000 500 S init alt_change 3000 change 2000

clean:
	rm -f *.o
	rm -f test
