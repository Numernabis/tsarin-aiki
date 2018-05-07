#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define MAX_CLIENTS  16
#define MAX_MSIZE 1024
#define SERVER_ID 0xCBA

typedef enum mtype {
    LOGIN = 1, MIRROR = 2, CALC = 3, TIME = 4, END = 5, INIT = 6
} mtype;

typedef struct Message {
    long mtype; //message type
    pid_t spid; //sender PID
    char mtext[MAX_MSIZE]; //message text
} Message;

const size_t MSG_SIZE = sizeof(Message) - sizeof(long);

#endif
