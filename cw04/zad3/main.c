/*
  author: Ludi
  file:   main.c
  start:  11.04.2018
  end:    11.04.2018
  lines:  117
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
int TYPE = 0;
int CLD_CNT = 0;
int PAR_CNT = 0;
int SIG_CNT = 0;

void set_handlers_parent();
void set_handlers_child();
void send_signal(pid_t);
void handler_sig1_parent(int, siginfo_t*, void*);
void handler_sig1_child(int, siginfo_t*, void*);
void handler_sig2_child(int, siginfo_t*, void*);

#define CREATE_SIGACT                                                          \
    struct sigaction act;                                                      \
    memset(&act, 0, sizeof(act));                                              \
    sigset_t real_mask;                                                        \
    sigfillset(&real_mask);                                                    \

#define SIG_CHOOSE                                                             \
    int sig = SIGUSR1;                                                         \
    if (TYPE == 3) sig = SIGRTMIN;                                             \

#define SIG_HANDLER(signal, func) {                                            \
    act.sa_sigaction = &(func);                                                \
    sigaction((signal), &act, NULL);                                           \
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Wrong number of arguments. Proper call:\n"
               "./program  L  TYPE\n"
               "where: L - number of requests, TYPE - sending method \n");
        return 2;
    }
    if ((L = atoi(argv[1])) == 0 || (TYPE = atoi(argv[2])) == 0 || L < 0 || TYPE < 0) {
        printf("Parameters L, TYPE must be positive integers.\n");
        return 2;
    }
    if (TYPE < 1 || TYPE > 3) {
        printf("Parameter TYPE must be 1, 2 or 3.\n");
        return 2;
    }
    /* ============================== */
    pid_t pid = fork();
    if (pid != 0) {
        set_handlers_parent();
        sleep(3);
        for(int i = 0; i < L; i++) {
            send_signal(pid);
        }
        if (TYPE != 3) kill(pid, SIGUSR2);
        else kill(pid, SIGRTMIN + 1);
    } else {
        set_handlers_child();
        while(1) { pause(); }
    }
    printf("Parent: send %d signals\n", SIG_CNT);
    printf("Parent: received %d signals\n", PAR_CNT);
    /* ============================== */
    return 0;
}

void set_handlers_parent() {
    CREATE_SIGACT
    SIG_HANDLER(SIGRTMIN, handler_sig1_parent);
    SIG_HANDLER(SIGUSR1, handler_sig1_parent);
}
void set_handlers_child() {
    CREATE_SIGACT
    act.sa_mask = real_mask;
    act.sa_flags = SA_RESTART;
    SIG_HANDLER(SIGRTMIN, handler_sig1_child);
    SIG_HANDLER(SIGRTMIN + 1, handler_sig2_child);
    SIG_HANDLER(SIGUSR1, handler_sig1_child);
    SIG_HANDLER(SIGUSR2, handler_sig2_child);
}
void send_signal(pid_t pid) {
    SIG_CHOOSE
    if (kill(pid, sig) == -1) {
        printf("Error: sending signal failed.\n");
        exit(1);
    } else {
        SIG_CNT++;
    }
    if (TYPE == 2) sleep(1);
}

void handler_sig1_parent(int num, siginfo_t *info, void *context) {
    PAR_CNT++;
}
void handler_sig1_child(int num, siginfo_t *info, void *context) {
    SIG_CHOOSE
    CLD_CNT++;
    kill(getppid(), sig);
}
void handler_sig2_child(int num, siginfo_t *info, void *context) {
    printf("Child: received %d signals\n", CLD_CNT);
    exit(0);
}
