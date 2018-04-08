/*
  author: Ludi
  file:   main.c
  start:  05.04.2018
  end:    []
  lines:  65
*/
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define SIGINT  2
#define SIGSTP 20
int STOPPED = 0;

void handler_int(int signum);
void handler_start(int signum);
void handler_stop(int signum);

int time_stamp() {
    time_t rawtime;
    time(&rawtime);
    struct tm *stamp = localtime(&rawtime);

    printf("\n%ld", rawtime);
    printf("\n%s", asctime(stamp));
    return 0;
}

int main(int argc, char** argv) {
    struct sigaction act;
    act.sa_handler = handler_stop;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGSTP, &act, NULL);
    signal(SIGINT, handler_int);

    /* ============================== */
    while(1){
        if (!STOPPED)
            time_stamp();
        sleep(1);
    }
    /* ============================== */
    return 0;
}

void handler_stop(int signum) {
    STOPPED = 1;
    printf("\nOczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu. \n");
    signal(SIGSTP, handler_start);
}
void handler_start(int signum) {
    STOPPED = 0;
    signal(SIGSTP, handler_stop);
}
void handler_int(int signum) {
    printf("\nOdebrano sygnal SIGINT.\n");
    raise(2);
    signal(SIGINT, SIG_DFL);
}
