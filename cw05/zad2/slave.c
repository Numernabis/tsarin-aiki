/*
  author: Ludi
  file:   slave.c
  start:  16.04.2018
  end:    18.04.2018
  lines:  47
*/
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define BUFF_SIZE 64
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Invalid program call. Proper arguments as follows:\n"
               "./slave  path_to_named_pipe  N\n");
        return 2;
    }
    pid_t pid = getpid();
    srand((time(0) * pid));
    char buffs[2][BUFF_SIZE];
    char* fpath = argv[1];
    int n = atoi(argv[2]);
    FILE *date;
    /* --------------------------------------------- */
    int fifo = open(fpath, O_WRONLY);
    printf("slave pid: %i\n", pid);

    for (int i = 0; i < n; i++) {
        date = popen("date", "r");
        fgets(buffs[0], BUFF_SIZE, date);
        sprintf(buffs[1], "%i\t%s", pid, buffs[0]);
        write(fifo, buffs[1], strlen(buffs[1]));
        fclose(date);
        sleep((rand() % 4 + 2));
    }
    close(fifo);
    /* --------------------------------------------- */
    return 0;
}
