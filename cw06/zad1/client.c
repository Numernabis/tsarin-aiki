/*
  author: Ludi
  file:   client.c
  start:  07.05.2018
  end:    07.05.2018
  lines:  128
*/
#include "common.h"

void register_client(key_t);
int open_public_queue(char*, int);
void close_private_queue();
void handle_sigint(int);

int public_queue = -2;
int session_id = -1;
int private_queue = -1;

/* -------------------------------------------------------------------------- */
#define MREAD                                                                  \
    printf("enter expression: ");                                              \
    if (fgets(msg.mtext, MAX_MSIZE, stdin) == 0)                               \
        printf("client: too many characters\n");                               \

#define MSEND(name) {                                                          \
    if (msgsnd(public_queue, &msg, MSG_SIZE, 0) == -1)                         \
        printf(CRED"client: %s request failed\n"CRST, (name));                 \
}

#define MRECEIVE                                                               \
    msgrcv(private_queue, &msg, MSG_SIZE, 0, 0);                               \
    printf("%s\n", msg.mtext);                                                 \
/* -------------------------------------------------------------------------- */
int main() {
    if (atexit(close_private_queue) == -1) {
        printf(CRED"client: registering client's atexit failed\n"CRST);
        return 2;
    }
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        printf(CRED"client: registering SIGINT handler failed\n"CRST);
        return 2;
    }

    char* path = getenv("HOME");
    public_queue = open_public_queue(path, SERVER_ID);

    key_t private_key = ftok(path, getpid());
    private_queue = msgget(private_key, IPC_CREAT | IPC_EXCL | 0666);
    if (private_queue == -1) {
        printf(CRED"client: creation of private queue failed\n"CRST);
        return 2;
    }
    register_client(private_key);

    char cmd[20];
    while (1) {
        Message msg;
        msg.spid = getpid();
        printf("client: enter your request: ");
        if (fgets(cmd, 20, stdin) == NULL){
            printf("client: error reading your command\n");
            continue;
        }
        int n = strlen(cmd);
        if (cmd[n - 1] == '\n') cmd[n - 1] = 0;

        if (strcmp(cmd, "mirror") == 0) {
            msg.mtype = MIRROR;
            MREAD
            MSEND("MIRROR")
            MRECEIVE
        } else if (strcmp(cmd, "calc") == 0) {
            msg.mtype = CALC;
            MREAD
            MSEND("CALC")
            MRECEIVE
        } else if (strcmp(cmd, "time") == 0) {
            msg.mtype = TIME;
            MSEND("TIME")
            MRECEIVE
        } else if (strcmp(cmd, "end") == 0) {
            msg.mtype = END;
            MSEND("END")
            exit(0);
        } else {
            printf("client: incorrect command\n");
        }
    }
}
/* -------------------------------------------------------------------------- */
void register_client(key_t private_key) {
    Message msg;
    msg.mtype = LOGIN;
    msg.spid = getpid();
    sprintf(msg.mtext, "%d", private_key);

    msgsnd(public_queue, &msg, MSG_SIZE, 0);
    msgrcv(private_queue, &msg, MSG_SIZE, 0, 0);
    sscanf(msg.mtext, "%d", &session_id);
    if (session_id < 0) {
        printf(CRED"client: server cannot have more clients\n"CRST);
        exit(2);
    }
    printf(CBLU"client: client registered | session id: %d\n"CRST, session_id);
}

int open_public_queue(char *path, int id) {
    key_t public_key = ftok(path, id);
    int public_queue = msgget(public_key, 0);
    if (public_queue == -1) {
        printf(CRED"client: opening public queue failed\n"CRST);
        return 2;
    }
    return public_queue;
}

void close_private_queue() {
    if (private_queue > -1) {
        msgctl(private_queue, IPC_RMID, NULL);
        printf(CBLU"client: private queue deleted successfully\n"CRST);
    }
}

void handle_sigint(int sig) {
    printf(CRED"\nclient: received signal %d\n"CRST, sig);
    exit(2);
}
/* -------------------------------------------------------------------------- */
