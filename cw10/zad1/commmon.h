#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <netdb.h>

#define MAX_NAME 20
#define MAX_PATH 104
#define MAX_EVENTS 5
#define MAX_CLIENTS 10
//#define READ_SIZE 2

//#define _BSD_SOURCE
//#define _SVID_SOURCE

#define SUM 0
#define SUB 1
#define MUL 2
#define DIV 3

#define EVAL 1
#define RESULT 2
#define PING 3
#define LOGIN 4

typedef struct Client{
    char name[MAX_NAME];
    int fd;
    int pings;
} client;

typedef struct Expr {
    int type;
    int arg1;
    int arg2;
} expr;

typedef struct Msg {
    int type;
    char name[MAX_NAME];
    int mid;
    expr expr;
} msg;

#endif
