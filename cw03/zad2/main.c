/*
  author: Ludi
  file:   main.c
  start:  25.03.2018
  end:    []
  lines:  90
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define ARGS_MAX 16
#define LINE_MAX 16*16

/* -------------------------------------------------------------------------- */
void print_header(int pid, char** args) {
    printf("\n-------------------------------------------------------------\n");
    printf("%-6s%-12d%-6s", "PID: ", pid, "Task: ");
    int i = 0;
    while(args[i] != 0) { printf("%s ", args[i]); i++; }
    printf("\n-------------------------------------------------------------\n");
}
void print_footer() {
    printf("---------------------------------------------------------------\n");
}
#define CLOSE_EXIT(num) {                                                      \
    fclose(file);                                                              \
    exit(num);                                                                 \
}
/* -------------------------------------------------------------------------- */
int main(int argc, char **argv) {
    /*
    proper arguments:
        argv[0] - ./program
        argv[1] - batch_file_name
    */
    if (argc != 2) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./program  batch_file_name\n");
        return 2;
    }

    // variables
    FILE* file;
    char* batch_file_name = argv[1];
    char command[LINE_MAX];
    char* args[ARGS_MAX];

    if ((file = fopen(batch_file_name, "r")) == 0) {
        printf("Unable to open batch file. :(\n");
        return 2;
    }

    /* ---------------------------------------------------------------------- */
    while (fgets(command, LINE_MAX, file)) {
        if (command[0] == '\n') continue;
        /* load task */
        char* w = strtok(command, " \n\t");
        int i = 0;
        while (w) {
            args[i] = w;
            w = strtok(0, " \n\t");
            i += 1;
        }
        args[i] = 0;

        pid_t pid = fork();
        if (pid < 0) {
            printf("Error: fork() failed.");
            CLOSE_EXIT(2);
        }
        if (pid == 0) {
            print_header((int)getpid(), args);
            execvp(args[0], args);
            print_footer();
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WEXITSTATUS(status)) {
                printf("Error: %i\n", status);
                CLOSE_EXIT(2);
            }
        }
    }
    /* ---------------------------------------------------------------------- */
    CLOSE_EXIT(0);
}
