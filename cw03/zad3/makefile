#Define variables
CC= gcc
CFLAGS= -Wall -lm
PROG= main
TIME_LIM= 5
MEMO_LIM= 32

#Default rule
all: program over_time over_memo test clean

program: $(PROG).c
	#Compile main
	$(CC) -o $@ $< $(CFLAGS)

over_time: over_time.c
	#Compile over_time
	$(CC) -o $@ $< $(CFLAGS)

over_memo: over_memo.c
	#Compile over_memo
	$(CC) -o $@ $< $(CFLAGS)

test:
	#---------------------------------------
	./program test_batch $(TIME_LIM) $(MEMO_LIM)

clean:
	rm -f *.o
	rm over_time over_memo
