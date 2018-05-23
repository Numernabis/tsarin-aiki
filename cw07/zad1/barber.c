/*
  author: Ludi
  file:   barber.c
  start:  18.05.2018
  end:    []
  lines:  []
*/
#include "common.h"

void invite_client();
void shave_client();
void clean_memory();
void handle_sigint(int);

int shm_wr, shm_bb;
int sem_wr, sem_bb;

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
    int capacity = atoi(argv[1]);
    if (capacity > MAX_QSIZE) {
        printf(CRED"given waiting_room_capacity is too big \
                    MAX_QSIZE = %d\n"CRST, MAX_QSIZE);
        return 2;
    }

    key_t key1 = ftok(getenv("HOME"), WROOM_ID);
    shm_wr = shmget(key1, sizeof(struct WaitingRoom), FLAGS);
    wroom = shmat(shm_wr, 0, 0);
    sem_wr = semget(key1, 1, FLAGS);
    semctl(sem_wr, 0, SETVAL, 1);

    key_t key2 = ftok(getenv("HOME"), BARBER_ID);
    shm_bb = shmget(key2, sizeof(struct Barber), FLAGS);
    barber = shmat(shm_bb, 0, 0);
    sem_bb = semget(key2, 1, FLAGS);
    semctl(sem_bb, 0, SETVAL, 1);

    barber->status = SLEEP;
    barber->client = 0;
    wroom->capacity = capacity;
    wroom->clients = 0;
    for (int i = 0; i < MAX_QSIZE; i++) wroom->queue[i] = 0;
    printf(CBLU"---- bshop opened (welcome) ----\n"CRST);

    while (1) {
        take_semaphore(sem_bb, sem_wr);
        switch (barber->status) {
            case IDLE:
                if (is_queue_empty()) {
                    printf("%ld  barber is falling asleep\n", time_stamp());
                    barber->status = SLEEP;
                } else {
                    invite_client();
                    barber->status = READY;
                }
                break;
            case AWAKEN:
                printf("%ld  barber force awakens\n", time_stamp());
                barber->status = READY;
                break;
            case BUSY:
                shave_client();
                barber->status = READY;
                break;
            default:
                break;
        }
        free_semaphore(sem_bb, sem_wr);
    }

    return 0;
}
/* -------------------------------------------------------------------------- */

void invite_client() {
    pid_t client = wroom->queue[0];
    barber->client = client;
    printf("%ld  +%d: invited\n", time_stamp(), client);
}

void shave_client() {
    printf("%ld  +%d: start\n", time_stamp(), barber->client);
    printf("%ld  +%d: finish\n", time_stamp(), barber->client);
    barber->client = 0;
}

void clean_memory() {
    semctl(sem_wr, 0, IPC_RMID);
    semctl(sem_bb, 0, IPC_RMID);
    shmctl(shm_wr, IPC_RMID, NULL);
    shmctl(shm_bb, IPC_RMID, NULL);
    shmdt(barber);
    shmdt(wroom);
}

void handle_sigint(int signum) {
    printf(CBLU"\n---- bshop closed (%d) ----\n"CRST, signum);
    exit(0);
}
/* -------------------------------------------------------------------------- */
