/*
  author: Ludi
  file:   server.c
  start:  07.05.2018
  end:    07.05.2018
  lines:  171
*/
#include "common.h"

void handle_login(struct Message*);
void handle_mirror(struct Message*);
void handle_calc(struct Message*);
void handle_time(struct Message*);
void handle_end(struct Message*);

int create_message(struct Message*);
int find_queue_id(pid_t);
void close_queue();
void handle_sigint(int);

int queue = -2;
int active = 1;
int clients[MAX_CLIENTS][2];
int cnt = 0;

/* -------------------------------------------------------------------------- */
int main() {
    if (atexit(close_queue) == -1) {
        printf(CRED"server: registering server's atexit failed\n"CRST);
        return 2;
    }
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        printf(CRED"server: registering SIGINT handler failed\n"CRST);
        return 2;
    }

    key_t public_key = ftok(getenv("HOME"), SERVER_ID);
    queue = msgget(public_key, IPC_CREAT | IPC_EXCL | 0666);
    if (queue == -1) {
        printf(CRED"server: creation of public queue failed\n"CRST);
        return 2;
    }
    printf(CBLU"server: started successfully\n"CRST);

    struct msqid_ds state;
    Message msg;
    while (1) {
        if (active == 0) {
            msgctl(queue, IPC_STAT, &state);
            if (state.msg_qnum == 0) break;
        }
        if (msgrcv(queue, &msg, MSG_SIZE, 0, 0) < 0) {
            printf(CRED"server: receiving message failed\n"CRST);
            return 2;
        }
        switch (msg.mtype) {
            case LOGIN:  handle_login(&msg);  break;
            case MIRROR: handle_mirror(&msg); break;
            case CALC:   handle_calc(&msg);   break;
            case TIME:   handle_time(&msg);   break;
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
    if (msgsnd(cq_id, msg, MSG_SIZE, 0) == -1) {                               \
        printf(CRED"server: %s response failed\n"CRST, (name));                \
        exit(2);                                                               \
    }                                                                          \
}
/* -------------------------------------------------------------------------- */
void handle_login(struct Message* msg) {
    printf("server: handle  LOGIN for client %d\n", msg->spid);
    key_t cq_key; //client queue key
    sscanf(msg->mtext, "%d", &cq_key);

    int cq_id = msgget(cq_key, 0); //client queue ID
    int cpid = msg->spid; //client ID
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
}

void handle_mirror(Message* msg) {
    printf("server: handle MIRROR for client %d\n", msg->spid);
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
    printf("server: handle   CALC for client %d\n", msg->spid);
    MCREATE
    char cmd[MAX_MSIZE];
    sprintf(cmd, "echo '%s' | bc", msg->mtext);
    FILE* result = popen(cmd, "r");
    fgets(msg->mtext, MAX_MSIZE, result);
    pclose(result);
    MSEND("CALC")
}

void handle_time(struct Message* msg) {
    printf("server: handle   TIME for client %d\n", msg->spid);
    MCREATE
    char stamp[20];
    time_t rawtime;
    time(&rawtime);
    strftime(stamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&rawtime));
    sprintf(msg->mtext, "%s", stamp);
    MSEND("TIME")
}

void handle_end(struct Message* msg) {
    printf("server: signal   END from client %d\n", msg->spid);
    active = 0;
}
/* -------------------------------------------------------------------------- */

int create_message(struct Message* msg) {
    int cq_id = find_queue_id(msg->spid);
    if (cq_id == -1) {
        printf(CRED"server: client not found\n"CRST);
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
    if (queue > -1) {
        msgctl(queue, IPC_RMID, NULL);
        printf(CBLU"server: queue deleted successfully\n"CRST);
    }
}

void handle_sigint(int sig) {
    printf(CRED"\nserver: received signal %d\n"CRST, sig);
    exit(2);
}
/* -------------------------------------------------------------------------- */
