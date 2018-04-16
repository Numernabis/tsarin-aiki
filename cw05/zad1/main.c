/*
  author: Ludi
  file:   main.c
  start:  12.04.2018
  end:    16.04.2018
  lines:  92
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define ARGS_MAX 12
#define LINE_MAX 12*12
#define CLOSE_EXIT(num) { fclose(file); exit(num); }
#define CLOSE_PIPES { close(pipes[k % 2][0]); close(pipes[k % 2][1]); }
int pipes[2][2];

char** parse_args(char* line, char* delim) {
    int c = strlen(line) - 1;
    if (line[c] == '\n') line[c] = '\0';
    int size = -1;
    char** args = malloc(sizeof(char*) * ARGS_MAX);
    char* p = strtok(line, delim);
    while (p) {
        args[++size] = p;
        p = strtok(0, delim);
    }
    args[++size] = 0;
    return args;
}
void print_header(char** args) {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("%-8s", "Task: "); int i = 0;
    while(args[i] != 0) { printf("%s|", args[i]); i++; }
    printf("\n-------------------------------------------------------------\n");
}
void print_footer() {
    printf("-------------------------------------------------------------\n");
}
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./program  batch_file_name\n");
        return 2;
    }
    /* variables */
    FILE* file;
    char* batch_file_name = argv[1];
    char command[LINE_MAX];

    if ((file = fopen(batch_file_name, "r")) == 0) {
        printf("Unable to open batch file: %s\n", batch_file_name);
        return 2;
    }
    /* ---------------------------------------------------------------------- */
    while (fgets(command, LINE_MAX, file)) {
        if (command[0] == '\n') continue;
        char** line = parse_args(command, "|");
        print_header(line);
        int n, k;
        for (n = 0; line[n] != 0; n++);
        for (k = 0; k < n; k++) {
            if (k > 1) CLOSE_PIPES
            pipe(pipes[k % 2]);

            pid_t pid = fork();
            if (pid == 0) {
                char** args = parse_args(line[k], " ");
                if (k < n - 1) {
                    close(pipes[k % 2][0]);
                    dup2(pipes[k % 2][1], 1);
                }
                if (k > 0) {
                    close(pipes[(k + 1) % 2][1]);
                    dup2(pipes[(k + 1) % 2][0], 0);
                }
                /* ============================== */
                execvp(args[0], args);
                /* ============================== */
            }
        }
        CLOSE_PIPES
        while (wait(NULL) > 0) { }
        print_footer();
    }
    /* ---------------------------------------------------------------------- */
    CLOSE_EXIT(0);
}
