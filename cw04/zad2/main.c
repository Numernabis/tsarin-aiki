/*
  author: Ludi
  file:   main.c
  start:  11.04.2018
  end:    11.04.2018
  lines:  107
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

int N = 0;
int K = 0;
int sig_cnt = 0;
pid_t *req;

void handler_sigusr1(int, siginfo_t*, void*);
void handler_sigrt(int, siginfo_t*, void*);
void handler_sigint();
void handler_sigusr2();

#define CREATE_SIGACT                                                          \
    struct sigaction act;                                                      \
    memset(&act, 0, sizeof(act));                                              \
    sigemptyset(&act.sa_mask);                                                 \
    act.sa_flags = SA_SIGINFO | SA_RESTART;                                    \

#define SIG_HANDLER(signal, func) {                                            \
    act.sa_sigaction = &(func);                                                \
    sigaction((signal), &act, NULL);                                           \
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Wrong number of arguments. Proper call:\n"
               "./program  N  K\n"
               "where: N - number of child, K - numer of requests\n");
        return 2;
    }
    if ((N = atoi(argv[1])) == 0 || (K = atoi(argv[2])) == 0 || N < 0 || K < 0) {
        printf("Parameters N, K must be positive integers.\n");
        return 2;
    }
    req = malloc(K * sizeof(pid_t));

    CREATE_SIGACT
    SIG_HANDLER(SIGUSR1, handler_sigusr1)
    act.sa_sigaction = &handler_sigrt;
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        sigaction(i, &act, NULL);
    }
    /* ============================== */
    for (int i = 0; i < N; i++) {
        srand(time(NULL));
        pid_t pid = fork();
        if (pid == 0) {
            printf("<%d> created new child <%d>\n", getppid(), getpid());
            SIG_HANDLER(SIGUSR2, handler_sigusr2)

            int s = rand() % 10 + 1;
            int rt = rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN;
            union sigval u;
            u.sival_int = s;
            sleep(s);
            kill(getppid(), SIGUSR1);
            pause();
            sigqueue(getppid(), rt, u);
            exit(s);

        } else {
            sleep(1);
        }
    }
    /* ============================== */
    while (wait(NULL) > 0) { }
    printf("%d\n", sig_cnt);
    return 0;
}

void handler_sigusr1(int num, siginfo_t *child, void *context) {
    printf("<%d> received request from <%d>\n", getpid(), child->si_pid);
    if (sig_cnt < K - 1) {
        req[sig_cnt] = child->si_pid;
    } else if (sig_cnt == K - 1) {
        for (int i = 0; i < K - 1; i++){
            kill(req[i], SIGUSR2);
        }
        kill(child->si_pid, SIGUSR2);
    } else {
        kill(child->si_pid, SIGUSR2);
    }
    sig_cnt++;
}

void handler_sigusr2(int num, siginfo_t *child, void *context) {
    printf("<%d> permission granted.\n", getpid());
}

void handler_sigrt(int num, siginfo_t *child, void *context) {
    printf("<%d> received SIGRT+%d | child <%d> ended with %d\n", \
        getpid(), num, child->si_pid, child->si_value.sival_int);
}
