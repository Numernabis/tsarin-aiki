#Define variables
CC= gcc
CFLAGS= -Wall -std=gnu99 -lpthread -lrt -lm
IMG= images
FLT= filters/filter
.PHONY: clean

#Default rule
all: main tests

main: main.c
	$(CC) -o $@ $< $(CFLAGS)

tests:
	#----------------------------------
	#Image: 460x360  Filter: 3x3
	./main 1 $(IMG)/cas.pgm $(FLT)3.txt $(IMG)/out3.pgm
	./main 2 $(IMG)/cas.pgm $(FLT)3.txt $(IMG)/out3.pgm
	./main 4 $(IMG)/cas.pgm $(FLT)3.txt $(IMG)/out3.pgm
	./main 8 $(IMG)/cas.pgm $(FLT)3.txt $(IMG)/out3.pgm
	./main 16 $(IMG)/cas.pgm $(FLT)3.txt $(IMG)/out3.pgm

	#----------------------------------
	#Image: 460x360  Filter: 5x5
	./main 1 $(IMG)/cas.pgm $(FLT)5.txt $(IMG)/out5.pgm
	./main 2 $(IMG)/cas.pgm $(FLT)5.txt $(IMG)/out5.pgm
	./main 4 $(IMG)/cas.pgm $(FLT)5.txt $(IMG)/out5.pgm
	./main 8 $(IMG)/cas.pgm $(FLT)5.txt $(IMG)/out5.pgm
	./main 16 $(IMG)/cas.pgm $(FLT)5.txt $(IMG)/out5.pgm

	#----------------------------------
	#Image: 460x360  Filter: 10x10
	./main 1 $(IMG)/cas.pgm $(FLT)10.txt $(IMG)/out10.pgm
	./main 2 $(IMG)/cas.pgm $(FLT)10.txt $(IMG)/out10.pgm
	./main 4 $(IMG)/cas.pgm $(FLT)10.txt $(IMG)/out10.pgm
	./main 8 $(IMG)/cas.pgm $(FLT)10.txt $(IMG)/out10.pgm
	./main 16 $(IMG)/cas.pgm $(FLT)10.txt $(IMG)/out10.pgm

	#----------------------------------
	#Image: 460x360  Filter: 19x19
	./main 1 $(IMG)/cas.pgm $(FLT)19.txt $(IMG)/out19.pgm
	./main 2 $(IMG)/cas.pgm $(FLT)19.txt $(IMG)/out19.pgm
	./main 4 $(IMG)/cas.pgm $(FLT)19.txt $(IMG)/out19.pgm
	./main 8 $(IMG)/cas.pgm $(FLT)19.txt $(IMG)/out19.pgm
	./main 16 $(IMG)/cas.pgm $(FLT)19.txt $(IMG)/out19.pgm

clean:
	rm -f *.o
#---------------------------------------
