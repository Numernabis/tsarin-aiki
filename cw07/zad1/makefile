#Define variables
CC= gcc
CFLAGS= -Wall -std=gnu99
.PHONY: clean

#Default rule
all: clients barber

clients: clients.c
	$(CC) -o $@ $< $(CFLAGS)

barber: barber.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f *.o
#---------------------------------------
