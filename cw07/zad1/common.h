#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#define CRED "\e[91m"
#define CBLU "\e[96m"
#define CRST "\e[0m"
#define BARBER_ID 0xABC
#define WROOM_ID 0xDEF
#define FLAGS (IPC_CREAT | 0666)
#define MAX_QSIZE 16

enum State {
    SLEEP, AWAKEN, READY, IDLE, BUSY
};

struct Barber {
    enum State status;      //barber status
    pid_t client;           //currrent client pid
} *barber;

struct WaitingRoom {
    int clients;            //number of clients
    int capacity;           //waiting room capacity
    pid_t queue[MAX_QSIZE]; //waiting room queue
} *wroom;

int is_queue_empty() {
    if (wroom->clients == 0) return 1;
    return 0;
}

int is_queue_full() {
    if (wroom->clients >= wroom->capacity) return 1;
    return 0;
}

void enter_queue(pid_t client) {
    wroom->queue[wroom->clients] = client;
    wroom->clients += 1;
}

void leave_queue() {
    for (int i = 0; i < wroom->clients - 1; i++) {
        wroom->queue[i] = wroom->queue[i + 1];
    }
    wroom->queue[wroom->clients - 1] = 0;
    wroom->clients -= 1;
}

long time_stamp() {
    struct timespec stamp;
    clock_gettime(CLOCK_MONOTONIC, &stamp);
    return stamp.tv_nsec / 1000;
}

void print_info(int op, long int time_stamp, int pid) {
    char* info;
    switch (op) {
        case 1: info = "woke up the barber"; break;
        case 2: info = "entering the queue"; break;
        case 3: info = "queue is full"; break;
        case 4: info = "sitted in barber chair"; break;
        case 5: info = "shaved"; break;
        default: info = "..."; break;
    }
    printf("%ld  ~%d: %s\n", time_stamp, pid, info);
}

void smf(int sem_id, int op) {
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = op;
    sop.sem_flg = 0;
    semop(sem_id, &sop, 1);
}

void take_semaphore(int sem1, int sem2) {
    if (sem1 != 0) smf(sem1, -1);
    if (sem2 != 0) smf(sem2, -1);
}

void free_semaphore(int sem1, int sem2) {
    if (sem1 != 0) smf(sem1, 1);
    if (sem2 != 0) smf(sem2, 1);
}

#endif
