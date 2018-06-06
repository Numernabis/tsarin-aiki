/*
  author: Ludi
  file:   main.c
  start:  05.06.2018
  end:    []
  lines:  163
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#define CON "\e[1;35mConsumer[%ld]: "
#define PRO "\e[1;36mProducer[%ld]: "
#define YEL "\e[1;33m"
#define RST "\e[0m"

int P, K, N, L, search_mode, verbose, nk;
char file_name[FILENAME_MAX], sign;
FILE* source;
char** buffer;
int wpos = 0, rpos = 0, fin = 0;
pthread_t* threads;
sem_t* sem;
// 0..N-1 - buff, N - wcond, N+1 - rcond, N+2 - buff available

/* -------------------------------------------------------------------------- */
void handle_signal(int signum) {
    printf(YEL"\nReceived SIG=%d -- cancelling threads\n"RST, signum);
    for (int i = 0; i < (P + K); i++)
        pthread_cancel(threads[i]);
    exit(EXIT_SUCCESS);
}

void load_config(char* config_file_name) {
    FILE* config;
    if ((config = fopen(config_file_name, "r")) == NULL) {
        printf("Failed to open config file :(\n");
        return;
    }
    fscanf(config, "%d\n%d\n%d\n%s\n%d\n%d\n%d\n%d",
        &P, &K, &N, file_name, &L, &search_mode, &verbose, &nk);
    printf(YEL"CONFIG >> P = %d   K = %d   N = %d   file_name = %s\n"
        "\t  L = %d   search_mode = %d   verbose = %d   nk = %d\n"RST,
        P, K, N, file_name, L, search_mode, verbose, nk);
    printf("---------------------------------------------------------\n");
    fclose(config);
}

int compare(int length) {
    if (length > L) { sign = '>'; return search_mode ==  1; }
    if (length < L) { sign = '<'; return search_mode == -1; }
               else { sign = '='; return search_mode ==  0; }
}

/* -------------------------------------------------------------------------- */
void* producer(void* just_a_pointer) {
    long int pnum = pthread_self();
    if (verbose) printf(PRO"started work\n"RST, pnum);
    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, source) != NULL) {
        if (verbose) printf(PRO"loaded text line\n"RST, pnum);
        sem_wait(&sem[N]);
        sem_wait(&sem[N + 2]);

        if (verbose) printf(PRO"lock sem[%d]\n"RST, pnum, wpos);
        sem_wait(&sem[wpos]);
        sem_post(&sem[N]);

        buffer[wpos] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[wpos], line);
        if (verbose) printf(PRO"copied line to buffer[%d]\n"RST, pnum, wpos);

        sem_post(&sem[wpos]);
        if (verbose) printf(PRO"unlock sem[%d]\n"RST, pnum, wpos);
        wpos = (wpos + 1) % N;
    }
    if (verbose) printf(PRO"finished work\n"RST, pnum);
    return NULL;
}

void* consumer(void* just_a_pointer) {
    long int cnum = pthread_self();
    if (verbose) printf(CON"started work\n"RST, cnum);
    while (1) {
        sem_wait(&sem[N + 1]);
        while (buffer[rpos] == NULL) {
            sem_post(&sem[N + 1]);
            if (fin) {
                if (verbose) printf(CON"finished work\n"RST, cnum);
                return NULL;
            }
            sem_wait(&sem[N + 1]);
        }

        if (verbose) printf(CON"lock sem[%d]\n"RST, cnum, rpos);
        sem_wait(&sem[rpos]);

        char* line = buffer[rpos];
        buffer[rpos] = NULL;
        int length = (int) strlen(line);
        if (verbose) printf(CON"read line from buffer[%d]\n"RST, cnum, rpos);

        sem_post(&sem[N + 2]);
        sem_post(&sem[N + 1]);
        sem_post(&sem[rpos]);
        if (verbose) printf(CON"unlock sem[%d]\n"RST, cnum, rpos);

        if (compare(length)) {
            printf(CON"found line with length %d %c %d\n"RST,
                cnum, length, sign, L);
            printf("%s\n", line);
        }
        rpos = (rpos + 1) % N;
    }
}

/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./main  config_file_name\n");
        return 2;
    }
    load_config(argv[1]);
    if ((source = fopen(file_name, "r")) == NULL) {
        printf("Failed to open text source file :(\n");
        return 2;
    }
    signal(SIGINT, handle_signal);
    if (nk > 0) signal(SIGALRM, handle_signal);

    buffer  = malloc(     N  * sizeof(char*));
    threads = malloc((P + K) * sizeof(pthread_t));
    sem     = malloc((N + 3) * sizeof(sem_t));

    for (int i = 0; i < N + 2; i++)
        sem_init(&sem[i], 0, 1);
    sem_init(&sem[N + 2], 0, N);

    /* ---------------------------------------------------------------------- */
    for (int i = 0; i < P; i++)
        pthread_create(&(threads[i]), NULL, producer, NULL);
    for (int i = 0; i < K; i++)
        pthread_create(&(threads[P + i]), NULL, consumer, NULL);

    if (nk > 0) alarm(nk);
    for (int i = 0; i < P; i++)
        pthread_join(threads[i], NULL);
    fin = 1;
    for (int i = 0; i < K; i++)
        pthread_join(threads[P + i], NULL);
    /* ---------------------------------------------------------------------- */

    if (source) fclose(source);
    for (int i = 0; i < N; i++) if (buffer[i]) free(buffer[i]);
    for (int i = 0; i < N + 3; i++) sem_destroy(&sem[i]);

    free(buffer); free(threads); free(sem);
    return 0;
}
/* -------------------------------------------------------------------------- */
