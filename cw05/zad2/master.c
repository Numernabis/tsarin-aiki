/*
  author: Ludi
  file:   master.c
  start:  16.04.2018
  end:    []
  lines:  39
*/
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define BUFF_SIZE 64
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid program call. Proper arguments as follows:\n"
               "./master  path_to_named_pipe\n");
        return 2;
    }
    char* fpath = argv[1];
    char buff[BUFF_SIZE];
    printf("master: sending signal to main (%i)\n", getppid());
    kill(getppid(), SIGUSR2);
    /* --------------------------------------------- */
    mkfifo(fpath, S_IRUSR | S_IWUSR);
    FILE *fifo = fopen(fpath, "r");
    while (fgets(buff, BUFF_SIZE, fifo) != 0) {
        write(1, buff, strlen(buff));
    }
    fclose(fifo);
    remove(fpath);
    /* --------------------------------------------- */
    return 0;
}
