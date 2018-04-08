/*
  author: Ludi
  file:   main.c
  start:  08.04.2018
  end:    08.04.2018
  lines:  61
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
#define DATE "./time_stamp.sh"
int STOPPED = 0;
int FORKED = 0;
pid_t CHILD_PID = 1;

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
    while (CHILD_PID != 0){
        if (!FORKED){
            CHILD_PID = fork();
            FORKED = 1;
        }
    }
    printf("\nCurrent pid: %d  Parent pid: %d\n", (int)getpid(), (int)getppid());
    execlp(DATE, DATE, NULL);
    /* ============================== */
    return 0;
}

void handler_sigstp(int signum) {
    if (!STOPPED) {
        STOPPED = 1;
        kill(CHILD_PID, 2);
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    } else {
        FORKED = 0;
        STOPPED = 0;
        return;
    }
}
void handler_sigint(int signum) {
    printf("\nOdebrano sygnał SIGINT.\n");
    exit(0);
}
