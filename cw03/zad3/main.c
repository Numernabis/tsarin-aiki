/*
  author: Ludi
  file:   main.c
  start:  26.03.2018
  end:    28.03.2018
  lines:  118
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define ARGS_MAX 16
#define LINE_MAX 16*16
/* -------------------------------------------------------------------------- */
void print_header(int pid, char** args) {
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("%-6s%-12d%-6s", "PID: ", pid, "Task: ");
    int i = 0;
    while(args[i] != 0) { printf("%s ", args[i]); i++; }
    printf("\n-------------------------------------------------------------\n");
}
void print_footer() {
    printf("-------------------------------------------------------------\n");
}
#define CLOSE_EXIT(num) {                                                      \
    fclose(file);                                                              \
    exit(num);                                                                 \
}
void print_usage_info(struct rusage *before, struct rusage *after) {
    double user_time =
        after->ru_utime.tv_sec - before->ru_utime.tv_sec +
        (after->ru_utime.tv_usec - before->ru_utime.tv_usec) * 0.000001;
    double system_time =
        after->ru_stime.tv_sec - before->ru_stime.tv_sec +
        (after->ru_stime.tv_usec - before->ru_stime.tv_usec) * 0.000001;
    long res_size = (after->ru_maxrss - before->ru_maxrss);
    printf("Usage info >>  UT: %.4fs   ST: %.4fs   RSS: %ldkB\n",
           user_time, system_time, res_size);
}
void set_limits(int time_limit, int memory_limit) {
    struct rlimit r;
    r.rlim_cur = r.rlim_max = (rlim_t)time_limit;
    setrlimit(RLIMIT_CPU, &r);
    r.rlim_cur = r.rlim_max = ((rlim_t)memory_limit * 1024 * 1024);
    setrlimit(RLIMIT_AS, &r);
}
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    /*
    proper arguments:
        argv[0] - ./program
        argv[1] - batch_file_name
        argv[2] - time_limit [in seconds]
        argv[3] - memory_limit [in MB]
    */
    if (argc != 4) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./program  batch_file_name  time_limit[in seconds]  memory_limit[in MB]\n");
        return 2;
    }
    /* variables */
    FILE* file;
    char* batch_file_name = argv[1];
    int time_limit = (int) strtol(argv[2], '\0', 10);
    int memory_limit = (int) strtol(argv[3], '\0', 10);
    char command[LINE_MAX];
    char* args[ARGS_MAX];

    if ((file = fopen(batch_file_name, "r")) == 0) {
        printf("Unable to open batch file: %s\n", batch_file_name);
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
        /* execute task in new process */
        struct rusage ru_before, ru_after;
        getrusage(RUSAGE_CHILDREN, &ru_before);
        pid_t pid = fork();
        if (pid < 0) {
            printf("Error: fork() failed.");
            CLOSE_EXIT(2);
        } else if (pid == 0) {
            print_header((int)getpid(), args);
            set_limits(time_limit, memory_limit);
            execvp(args[0], args);
            printf("Unable to execute task!\n");
            break;
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WEXITSTATUS(status)) {
                printf("Error: %i\n", status);
                CLOSE_EXIT(2);
            }
        }
        print_footer();
        getrusage(RUSAGE_CHILDREN, &ru_after);
        print_usage_info(&ru_before, &ru_after);
        print_footer();
    }
    /* ---------------------------------------------------------------------- */
    CLOSE_EXIT(0);
}
