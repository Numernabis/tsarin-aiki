#Define variables
CC= gcc
CFLAGS= -Wall -std=gnu99 -lpthread -lrt
.PHONY: clean

#Default rule
all: main

main: main.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f *.o
#---------------------------------------
