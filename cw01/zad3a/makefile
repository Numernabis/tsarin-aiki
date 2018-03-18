#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.
PROG= ../zad2/main
LIB= ../zad1/library

#Default rule
all: zad3_static zad3_shared zad3_dynamic clean

zad3_static:
	#With static library
	$(CC) -o test -D STATIC $(PROG).c $(LIB).a $(CFLAGS)
	make testset

zad3_shared:
	#With shared library
	$(CC) -o test -D SHARED $(PROG).c -L. ../zad1/liblibrary.so $(CFLAGS)
	make testset

zad3_dynamic:
	#With dynamic library
	$(CC) -o test -D DYNAMIC $(PROG).c $(CFLAGS) -ldl
	make testset

testset:
	#Tests - dynamic allocation
	./test 5000 500 D init change 4000 find 444
	./test 5000 500 D init alt_change 3000 change 2000

	#Tests - static allocation
	./test 5000 500 S init change 4000 find 444
	./test 5000 500 S init alt_change 3000 change 2000

clean:
	rm -f *.o
	rm -f test
