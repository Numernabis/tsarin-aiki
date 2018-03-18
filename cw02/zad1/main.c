/*
  author: Ludi
  file:   main.c
  start:  16.03.2018
  end:    []
  lines:  155
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include "operations.h"

/* -------------------------------------------------------------------------- */
/* s - system, u - user (time) */
int get_time(struct timeval *t) {
    return (int) (t->tv_sec) * 1000000 + (int) (t->tv_usec);
}

#define MEASURE(name, block) {                                                 \
    struct rusage usage;                                                       \
    long s_start, s_end, u_start, u_end;                                       \
    getrusage(RUSAGE_SELF, &usage);                                            \
    s_start = get_time(&usage.ru_stime);                                       \
    u_start = get_time(&usage.ru_utime);                                       \
        block                                                                  \
    getrusage(RUSAGE_SELF, &usage);                                            \
    s_end = get_time(&usage.ru_stime);                                         \
    u_end = get_time(&usage.ru_utime);                                         \
    printf(#name ":\n%ldµs %ldµs\n\n",                                         \
        (s_end - s_start),                                                     \
        (u_end - u_start));                                                    \
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int main(int argc, char **argv) {

    srand((unsigned int) time(NULL));
    /*
    version 1:
		argv[0] - ./program
		argv[1] - generate / sort
		argv[2] - file_name
		argv[3] - number_of_records
		argv[4] - size_of_record (4, 512, 4096, 8192)
        argv[5] - sys / lib

    version 2:
		argv[0] - ./program
		argv[1] - copy
		argv[2] - file_name_1 (from)
        argv[3] - file_name_2 (to)
		argv[4] - number_of_records
		argv[5] - buffer_size
        argv[6] - sys / lib
	*/
    /* argc = 6 || argc = 7 */

    if (argc < 6 || argc > 7) {
        // -1, because of argv[0] is program call
        printf("5 or 6 arguments you should enter.\n");
        return 2;
    }

    // load operation name
    char *op_name = argv[1];

    // arguments validation
    if (strcmp(op_name, "generate") != 0 && strcmp(op_name, "sort") != 0 && strcmp(op_name, "copy") != 0) {
        printf("Unsupported operation! [available: generate, sort, copy]\n");
        return 2;
    }
    if (strcmp(op_name, "copy") == 0 && argc != 7) {
        printf("Operation: copy. Proper arguments:\n");
        printf("./program copy file_name_1 file_name_2 number_of_records buffer_size sys/lib\n");
        return 2;
    }
    if (strcmp(op_name, "copy") != 0 && argc != 6) {
        printf("Operation: generate/sort. Proper arguments:\n");
        printf("./program generate/sort file_name number_of_records buffer_size sys/lib\n");
        return 2;
    }

    // variables
    char *file_name_1, *file_name_2;
    int number_of_records = 0;
    int size_of_record = 0;
    int buffer_size = 0;
    char *func_type = "";

    // load arguments
    if (strcmp(op_name, "copy") == 0) {
        file_name_1 = argv[2];
        file_name_2 = argv[3];
        number_of_records = (int) strtol(argv[4], '\0', 10);
        buffer_size = (int) strtol(argv[5], '\0', 10);
        func_type = argv[6];
    } else {
        file_name_1 = argv[2];
        number_of_records = (int) strtol(argv[3], '\0', 10);
        size_of_record = (int) strtol(argv[4], '\0', 10);
        func_type = argv[5];
    }

    if (strcmp(func_type, "sys") != 0 && strcmp(func_type, "lib") != 0) {
        printf("Unsupported variant! [available: sys, lib]\n");
        return 2;
    }
    if (number_of_records < 0 || size_of_record < 0 || buffer_size < 0) {
        printf("Error. Following variables must be positive integers:\n");
        printf("number_of_records, size_of_record / buffer_size\n");
        return 2;
    }
    // so far, arguments are validated :)

    /* ---------------------------------------------------------------------- */

    if (strcmp(op_name, "generate") == 0) {
        if (strcmp(func_type, "sys") == 0) {
            generate_file_sys(file_name_1, number_of_records, size_of_record);
        } else {
            generate_file_lib(file_name_1, number_of_records, size_of_record);
        }
    } else if (strcmp(op_name, "sort") == 0) {

        if (strcmp(func_type, "sys") == 0) {
            MEASURE(file sorting [sys],
                sort_file_sys(file_name_1, number_of_records, size_of_record);
            )
        } else {
            MEASURE(file sorting [lib],
                sort_file_lib(file_name_1, number_of_records, size_of_record);
            )
        }
    } else { //copy

        if (strcmp(func_type, "sys") == 0) {
            MEASURE(file copying [sys],
                copy_file_sys(file_name_1, file_name_2, number_of_records, buffer_size);
            )
        } else {
            MEASURE(file copying [lib],
                copy_file_lib(file_name_1, file_name_2, number_of_records, buffer_size);
            )
        }
    }

    /* ---------------------------------------------------------------------- */

    return 0;
}
