/*
  author: Ludi
  file:   main.c
  start:  08.04.2018
  end:    []
  lines:  []
*/
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define SIGINT  2
#define SIGSTP 20
int STOPPED = 0;

void handler_sigint(int signum);
void handler_sigstp(int signum);

int main(int argc, char** argv) {
    struct sigaction stp;
    stp.sa_handler = handler_sigstp;
    sigemptyset(&stp.sa_mask);
    stp.sa_flags = 0;

    sigaction(SIGSTP, &stp, NULL);
    signal(SIGINT, handler_sigint);

    /* ============================== */
    while(1){
        if (!STOPPED)
            /* do stuff here */
    }
    /* ============================== */
    return 0;
}

void handler_sigstp(int signum) {
    if (!STOPPED) {
        STOPPED = 1;
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    } else {
        STOPPED = 0;
        // kill
        return;
    }
}
void handler_sigint(int signum) {
    printf("\nOdebrano sygnał SIGINT.\n");
    exit(0);
}
