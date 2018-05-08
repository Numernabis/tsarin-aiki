/*
  author: Ludi
  file:   server.c
  start:  08.05.2018
  end:    []
  lines:  []
*/
#include "common.h"

void handle_login(struct Message*);
void handle_mirror(struct Message*);
void handle_calc(struct Message*);
void handle_time(struct Message*);
void handle_quit(struct Message*);
void handle_end(struct Message*);

int create_message(struct Message*);
int find_queue_id(pid_t);
void close_queue();
void handle_sigint(int);

mqd_t queue = -1;
int active = 1;
int clients[MAX_CLIENTS][2];
int cnt = 0;

/* -------------------------------------------------------------------------- */
int main() {
    if (atexit(close_queue) == -1) {
        printf("server: registering server's atexit failed\n");
        return 2;
    }
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        printf("server: registering SIGINT handler failed\n");
        return 2;
    }

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_QSIZE;
    attr.mq_msgsize = MSG_SIZE;

    queue = mq_open(server_path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    if (queue == -1) {
        printf("server: creation of public queue failed\n");
        return 2;
    }
    printf(CBLU"server: started successfully\n"CRST);

    struct mq_attr state;
    Message msg;
    while (1) {
        if (active == 0) {
            mq_getattr(queue, &state);
            if (state.mq_curmsgs == 0) break;
        }
        if (mq_receive(queue, (char*) &msg, MSG_SIZE, NULL) < 0) {
            printf("server: receiving message failed\n");
            return 2;
        }
        switch (msg.mtype) {
            case LOGIN:  handle_login(&msg);  break;
            case MIRROR: handle_mirror(&msg); break;
            case CALC:   handle_calc(&msg);   break;
            case TIME:   handle_time(&msg);   break;
            case QUIT:   handle_quit(&msg);   break;
            case END:    handle_end(&msg);    break;
            default:     break;
        }
    }
    return 0;
}
/* -------------------------------------------------------------------------- */
#define MCREATE                                                                \
    int cq_id = create_message(msg);                                           \
    if (cq_id == -1) return;                                                   \

#define MSEND(name) {                                                          \
    if (mq_send(cq_id, (char*) msg, MSG_SIZE, 1) == -1) {                      \
        printf("server: %s response failed\n", (name));                        \
        exit(2);                                                               \
    }                                                                          \
}
/* -------------------------------------------------------------------------- */
void handle_login(struct Message* msg) {
    int cpid = msg->spid;
    char cpath[15];
    sprintf(cpath, "/%d", cpid); //client ID
    int cq_id = mq_open(cpath, O_WRONLY);
    msg->mtype = INIT;
    msg->spid = getpid();

    if (cnt > MAX_CLIENTS - 1) {
        printf("server: maximum number of clients reached\n");
        sprintf(msg->mtext, "%d", -1);
    } else {
        clients[cnt][0] = cpid;
        clients[cnt++][1] = cq_id;
        sprintf(msg->mtext, "%d", cnt - 1);
    }
    MSEND("LOGIN")
    if (strcmp(msg->mtext, "-1") == 0) mq_close(cq_id);
}

void handle_mirror(Message* msg) {
    MCREATE
    int mlen = (int) strlen(msg->mtext); //message length
    if (msg->mtext[mlen - 1] == '\n') mlen--;
    for (int i = 0; i < mlen / 2; i++) {
        char buff = msg->mtext[i];
        msg->mtext[i] = msg->mtext[mlen - i - 1];
        msg->mtext[mlen - i - 1] = buff;
    }
    MSEND("MIRROR")
}

void handle_calc(Message* msg) {
    MCREATE
    char cmd[MAX_MSIZE];
    sprintf(cmd, "echo '%s' | bc", msg->mtext);
    FILE* result = popen(cmd, "r");
    fgets(msg->mtext, MAX_MSIZE, result);
    pclose(result);
    MSEND("CALC")
}

void handle_time(struct Message* msg) {
    MCREATE
    char stamp[20];
    time_t rawtime;
    time(&rawtime);
    strftime(stamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&rawtime));
    sprintf(msg->mtext, "%s", stamp);
    MSEND("TIME")
}

void handle_quit(struct Message* msg) {
    int i; for (i = 0; i < cnt; ++i) {
        if (clients[i][0] == msg->spid) break;
    }
    if (i == cnt) {
        printf("server: client not found\n");
        return;
    }
    mq_close(clients[i][1]);
    for (; i + 1 < cnt; ++i) {
        clients[i][0] = clients[i + 1][0];
        clients[i][1] = clients[i + 1][1];
    }
    cnt--;
    printf("server: cleared data of removed client\n");
}

void handle_end(struct Message* msg) {
    printf("server: received END from client %d\n", msg->spid);
    active = 0;
}
/* -------------------------------------------------------------------------- */

int create_message(struct Message* msg) {
    int cq_id = find_queue_id(msg->spid);
    if (cq_id == -1) {
        printf("server: client not found\n");
        return -1;
    }
    msg->mtype = msg->spid;
    msg->spid = getpid();
    return cq_id;
}

int find_queue_id(pid_t spid) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i][0] == spid)
            return clients[i][1];
    }
    return -1;
}

void close_queue() {
    for (int i = 0; i < cnt; ++i) {
        mq_close(clients[i][1]);
        kill(clients[i][0], SIGINT);
    }
    if (queue > -1) {
        mq_close(queue);
        mq_unlink(server_path);
        printf(CBLU"server: queue deleted successfully\n"CRST);
    }
}

void handle_sigint(int sig) {
    printf(CRED"\nserver: received signal %d\n"CRST, sig);
    exit(2);
}
/* -------------------------------------------------------------------------- */
