/*
  author: Ludi
  file:   client.c
  start:  08.05.2018
  end:    []
  lines:  []
*/
#include "common.h"

void register_client();
void close_private_queue();
void handle_sigint(int);

mqd_t public_queue = -1;
int session_id = -1;
mqd_t private_queue = -1;
char path[20];

/* -------------------------------------------------------------------------- */
#define MREAD                                                                  \
    printf("enter expression: ");                                              \
    if (fgets(msg.mtext, MAX_MSIZE, stdin) == 0)                               \
        printf("client: too many characters\n");                               \

#define MSEND(name) {                                                          \
    if (mq_send(public_queue, (char*) &msg, MSG_SIZE, 0) == -1)                \
        printf("client: %s request failed\n", (name));                         \
}

#define MRECEIVE(name) {                                                       \
    if (mq_receive(private_queue, (char*) &msg, MSG_SIZE, NULL) == -1)         \
        printf("client: catching %s response failed\n", (name));               \
    printf("%s\n", msg.mtext);                                                 \
}
/* -------------------------------------------------------------------------- */
int main() {
    if (atexit(close_private_queue) == -1) {
        printf("client: registering client's atexit failed\n");
        return 2;
    }
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        printf("client: registering SIGINT handler failed\n");
        return 2;
    }

    public_queue= mq_open(server_path, O_WRONLY);

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_QSIZE;
    attr.mq_msgsize = MSG_SIZE;

    sprintf(path, "/%d", getpid());
    private_queue = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    if (private_queue == -1) {
        printf("client: creation of private queue failed\n");
        return 2;
    }
    register_client();

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
            MRECEIVE("MIRROR")
        } else if (strcmp(cmd, "calc") == 0) {
            msg.mtype = CALC;
            MREAD
            MSEND("CALC")
            MRECEIVE("CALC")
        } else if (strcmp(cmd, "time") == 0) {
            msg.mtype = TIME;
            MSEND("TIME")
            MRECEIVE("TIME")
        } else if (strcmp(cmd, "end") == 0) {
            msg.mtype = END;
            MSEND("END")
        } else if (strcmp(cmd, "quit") == 0) {
            exit(0);
        } else {
            printf("client: incorrect command\n");
        }
    }
}
/* -------------------------------------------------------------------------- */
void register_client() {
    Message msg;
    msg.mtype = LOGIN;
    msg.spid = getpid();

    mq_send(public_queue, (char*) &msg, MSG_SIZE, 1);
    mq_receive(private_queue, (char*) &msg, MSG_SIZE, NULL);
    sscanf(msg.mtext, "%d", &session_id);
    if (session_id < 0) {
        printf(CRED"client: server cannot have more clients\n"CRST);
        exit(2);
    }
    printf(CBLU"client: client registered | session id: %d\n"CRST, session_id);
}

void close_private_queue() {
    if (private_queue > -1) {
        if (session_id >= 0) {
            Message msg;
            msg.mtype = QUIT;
            msg.spid = getpid();
            MSEND("QUIT")
        }
        mq_close(public_queue);
        mq_close(private_queue);
        mq_unlink(path);
        printf(CBLU"client: private queue deleted successfully\n"CRST);
    }
}

void handle_sigint(int sig) {
    printf(CRED"\nclient: received signal %d\n"CRST, sig);
    exit(2);
}
/* -------------------------------------------------------------------------- */
