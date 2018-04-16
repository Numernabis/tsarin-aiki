#Define variables
CC= gcc
CFLAGS= -Wall -std=c99 -I.

#Default rule
all: main master slave clean

main: main.c
	$(CC) $< -o $@ $(CFLAGS)

master: master.c
	$(CC) $< -o $@ $(CFLAGS)

slave: slave.c
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm -f *.o
