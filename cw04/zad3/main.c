/*
  author: Ludi
  file:   main.c
  start:  11.04.2018
  end:    []
  lines:  []
*/
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int L = 0;
int Type = 0;
int sig_cnt = 0;

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Wrong number of arguments. Proper call:\n"
               "./program  L  Type\n"
               "where: L - number of requests, Type - sending method \n");
        return 2;
    }
    if ((L = atoi(argv[1])) == 0 || (Type = atoi(argv[2])) == 0 || L < 0 || Type < 0) {
        printf("Parameters L, Type must be positive integers.\n");
        return 2;
    }
    if (Type < 1 || Type > 3) {
        printf("Parameter Type must be 1, 2 or 3.\n");
        return 2;
    }
    /* ============================== */

    /* ============================== */
    return 0;
}
