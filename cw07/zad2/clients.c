/*
  author: Ludi
  file:   clients.c
  start:  23.05.2018
  end:    23.05.2018
  lines:  122
*/
#include "common.h"

void sit_barber_chair();
void shave_client_S_times(int);

int shm_wr, shm_bb;
sem_t* sem_wr;
sem_t* sem_bb;
sem_t* sem_00 = NULL;
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

    shm_wr = shm_open(WROOM_ID, O_RDWR, MODE);
    ftruncate(shm_wr, sizeof(*wroom));
    wroom = mmap(NULL, sizeof(*wroom), PROT_RW, MAP_SHARED, shm_wr, 0);
                // address, length, prot, flags, file_desc, offset
    sem_wr = sem_open(WROOM_ID, O_WRONLY, MODE, 0);
                // path, flags, mode, value

    shm_bb = shm_open(BARBER_ID, O_RDWR, MODE);
    ftruncate(shm_bb, sizeof(*barber));
    barber = mmap(NULL, sizeof(*barber), PROT_RW, MAP_SHARED, shm_bb, 0);
    sem_bb = sem_open(BARBER_ID, O_WRONLY, MODE, 0);

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
        take_semaphore(sem_00, sem_wr);
        leave_queue();
        free_semaphore(sem_00, sem_wr);
    } else if (status == 0) {
        while (1) {
            free_semaphore(sem_bb, sem_00);
            take_semaphore(sem_bb, sem_00);
            if (barber->status == READY) break;
        }
        status = 1;
    }
    barber->client = pid;
    print_info(4, time_stamp(), pid);
}

void shave_client_S_times(int S) {
    pid_t pid = getpid();
    int num = 0;

    while (num < S) {
        status = 0;
        take_semaphore(sem_00, sem_wr);
        if (is_queue_full()) {
            print_info(3, time_stamp(), pid);
            free_semaphore(sem_00, sem_wr);
            break;
        }
        free_semaphore(sem_00, sem_wr);
        take_semaphore(sem_bb, sem_00);
        if (barber->status == SLEEP) {
            print_info(1, time_stamp(), pid);
            barber->status = AWAKEN;
            sit_barber_chair();
            barber->status = BUSY;
        } else {
            print_info(2, time_stamp(), pid);
            take_semaphore(sem_00, sem_wr);
            enter_queue(pid);
            free_semaphore(sem_00, sem_wr);
        }
        free_semaphore(sem_bb, sem_00);

        while (status < 1) {
            take_semaphore(sem_bb, sem_00);
            if (barber->client == pid) {
                status = 1;
                sit_barber_chair();
                barber->status = BUSY;
            }
            free_semaphore(sem_bb, sem_00);
        }
        while (status < 2) {
            take_semaphore(sem_bb, sem_00);
            if (barber->client != pid) {
                status = 2;
                print_info(5, time_stamp(), pid);
                barber->status = IDLE;
                num++;
            }
            free_semaphore(sem_bb, sem_00);
        }
    }
    printf("%ld  ~%d: left bshop after %d cuts\n", time_stamp(), pid, num);
}
/* -------------------------------------------------------------------------- */
