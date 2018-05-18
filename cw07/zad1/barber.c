/*
  author: Ludi
  file:   barber.c
  start:  18.05.2018
  end:    []
  lines:  106
*/
#include "common.h"

void invite_client();
void shave_client();
void clean_memory();
void handle_sigint(int);

int shm_id;
int sem_id;

/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./barber  waiting_room_size\n");
        return 2;
    }
    if (atexit(clean_memory) == -1) {
        printf(CRED"registering barber's atexit failed\n"CRST);
        return 2;
    }
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        printf(CRED"registering SIGINT handler failed\n"CRST);
        return 2;
    }
    int wr_size = atoi(argv[1]);
    if (wr_size > MAX_QSIZE) {
        printf(CRED"given waiting_room_size is too big \
                    MAX_QSIZE = %d\n"CRST, MAX_QSIZE);
        return 2;
    }

    key_t public_key = ftok(getenv("HOME"), BARBER_ID);
    shm_id = shmget(public_key, sizeof(struct BShop), S_IRWXU | IPC_CREAT);
    bshop = shmat(shm_id, 0, 0);

    sem_id = semget(public_key, 1, S_IRWXU | IPC_CREAT);
    semctl(sem_id, 0, SETVAL, 0);
    free_semaphore(sem_id);

    bshop->bstatus = SLEEP;
    bshop->wr_size = wr_size;
    bshop->cnt = 0;
    bshop->client = 0;
    for (int i = 0; i < MAX_QSIZE; i++) bshop->queue[i] = 0;
    printf(CBLU"---- bshop opened (welcome) ----\n"CRST);

    while(1) {
        take_semaphore(sem_id);
        switch (bshop->bstatus) {
            case IDLE:
                if (is_queue_empty()) {
                    printf("%ld  barber is falling asleep\n", time_stamp());
                    bshop->bstatus = SLEEP;
                } else {
                    invite_client();
                    bshop->bstatus = READY;
                }
                break;
            case AWAKEN:
                printf("%ld  barber force awakens\n", time_stamp());
                bshop->bstatus = READY;
                break;
            case BUSY:
                shave_client();
                bshop->bstatus = READY;
                break;
            default:
                break;
        }
        free_semaphore(sem_id);
    }

    return 0;
}
/* -------------------------------------------------------------------------- */

void invite_client() {
    pid_t client = bshop->queue[0];
    bshop->client = client;
    printf("%ld  +%d: invited\n", time_stamp(), client);
}

void shave_client() {
    printf("%ld  +%d: start\n", time_stamp(), bshop->client);
    printf("%ld  +%d: finish\n", time_stamp(), bshop->client);
    bshop->client = 0;
}

void clean_memory() {
    if (sem_id != 0) semctl(sem_id, 0, IPC_RMID);
    if (shm_id != 0) shmctl(shm_id, IPC_RMID, NULL);
}

void handle_sigint(int signum) {
    printf(CBLU"\n---- bshop closed (%d) ----\n"CRST, signum);
    exit(0);
}
/* -------------------------------------------------------------------------- */
