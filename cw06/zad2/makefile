#Define variables
CC= gcc
CFLAGS= -Wall -std=gnu99 -Wextra -lrt
.PHONY: clean

#Default rule
all: client server

client: client.c
	$(CC) -o $@ $< $(CFLAGS)

server: server.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f *.o
#---------------------------------------
