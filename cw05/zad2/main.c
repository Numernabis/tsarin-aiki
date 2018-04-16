/*
  author: Ludi
  file:   main.c
  start:  16.04.2018
  end:    []
  lines:  54
*/
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

int START = 0;
void handle_usr2(int num, siginfo_t* siginfo, void* context) {
    printf("Received USR2\n");
    START = 1;
}
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Invalid program call. Proper arguments as follows:\n"
               "./program  path_to_named_pipe  N\n");
        return 2;
    }
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = &handle_usr2;
    sigaction(SIGUSR2, &act, NULL);
    /* ---------------------------------------------------------------------- */
    pid_t pid = fork();
    if (pid == 0) {
        execlp("./master", "./master", argv[1], NULL);
        return 0;
    }
    while (!START) {
        printf("%i: waiting for signal from master...\n", getpid());
        sleep(1);
    }
    for (int i = 0; i < 10; i++) {
        pid_t slave = fork();
        if (slave != 0) {
            execlp("./slave", "./slave", argv[1], argv[2], NULL);
        }
    }
    while (wait(NULL) > 0) { }
    /* ---------------------------------------------------------------------- */
    return 0;
}
