/*
  author: Ludi
  file:   clients.c
  start:  18.05.2018
  end:    []
  lines:  106
*/
#include "common.h"

void sit_barber_chair();
void shave_client_S_times(int);

int shm_id;
int sem_id;
int status; // 0 - new client, 1 - invited, 2 - shaved

/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./clients  clients_number  S\n");
        return 2;
    }
    int clients_number = atoi(argv[1]);
    int S = atoi(argv[2]);

    key_t public_key = ftok(getenv("HOME"), BARBER_ID);
    shm_id = shmget(public_key, sizeof(struct BShop), 0);
    bshop = shmat(shm_id, 0, 0);
    sem_id = semget(public_key, 0, 0);

    for (int i = 0; i < clients_number; i++) {
        if (fork() == 0) {
            shave_client_S_times(S);
            exit(0);
        }
    }
    while (wait(NULL)) {
        if (errno == ECHILD) break;
    }
    return 0;
}
/* -------------------------------------------------------------------------- */

void sit_barber_chair() {
    pid_t pid = getpid();

    if (status == 1) {
        leave_queue();
    } else if (status == 0) {
        while (1) {
            free_semaphore(sem_id);
            take_semaphore(sem_id);
            if (bshop->bstatus == READY) break;
        }
        status = 1;
    }
    bshop->client = pid;
    print_info(4, time_stamp(), pid);
}

void shave_client_S_times(int S) {
    pid_t pid = getpid();
    int num = 0;

    while (num < S) {
        status = 0;
        take_semaphore(sem_id);
        if (is_queue_full()) {
            print_info(3, time_stamp(), pid);
            free_semaphore(sem_id);
            break;
        } else if (bshop->bstatus == SLEEP) {
            print_info(1, time_stamp(), pid);
            bshop->bstatus = AWAKEN;
            sit_barber_chair();
            bshop->bstatus = BUSY;
        } else {
            print_info(2, time_stamp(), pid);
            enter_queue(pid);
        }
        free_semaphore(sem_id);

        while (status < 1) {
            take_semaphore(sem_id);
            if (bshop->client == pid) {
                status = 1;
                sit_barber_chair();
                bshop->bstatus = BUSY;
            }
            free_semaphore(sem_id);
        }
        while (status < 2) {
            take_semaphore(sem_id);
            if (bshop->client != pid) {
                status = 2;
                print_info(5, time_stamp(), pid);
                bshop->bstatus = IDLE;
                num++;
            }
            free_semaphore(sem_id);
        }
    }
    printf("%ld  ~%d: left bshop after %d cuts\n", time_stamp(), pid, num);
}
/* -------------------------------------------------------------------------- */
