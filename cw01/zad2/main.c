/*
  file:  main.c
  start: 12.03.2018
  end:   15.03.2018
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <dlfcn.h>
#include "../zad1/library.h"

#define MULTI_TEST 100

static const char *characters =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

/* -------------------------------------------------------------------------- */
#ifdef DYNAMIC
    void* handle;

    void load_lib() {
        handle = dlopen("../zad1/liblibrary.so", RTLD_LAZY);
        if (!handle) {
            printf("[%s] Unable to load library: %s\n", __FILE__, dlerror());
            exit(EXIT_FAILURE);
        }
    }
    void close_lib() {
        if (dlclose(handle) != 0) {
            printf("[%s] Problem closing library: %s", __FILE__, dlerror());
        }
    }
#endif

/* -------------------------------------------------------------------------- */
/* s - system, u - user, r - real */
int get_time(struct timeval *t) {
    return (int) (t->tv_sec) * 1000000 + (int) (t->tv_usec);
}

#define MEASURE(name, times, block) {                                          \
    struct rusage usage;                                                       \
    long s_start, s_end, u_start, u_end, r_start, r_end;                       \
    struct timeval real;                                                       \
    getrusage(RUSAGE_SELF, &usage);                                            \
    gettimeofday(&real, 0);                                                    \
    s_start = get_time(&usage.ru_stime);                                       \
    u_start = get_time(&usage.ru_utime);                                       \
    r_start = get_time(&real);                                                 \
    for (int _i = 0; _i < (times); _i++) {                                     \
        block                                                                  \
    }                                                                          \
    getrusage(RUSAGE_SELF, &usage);                                            \
    gettimeofday(&real, 0);                                                    \
    s_end = get_time(&usage.ru_stime);                                         \
    u_end = get_time(&usage.ru_utime);                                         \
    r_end = get_time(&real);                                                   \
    printf(#name ":\n%ldµs %ldµs %ldµs\n\n",                                   \
        (s_end - s_start),                                                     \
        (u_end - u_start),                                                     \
        (r_end - r_start));                                                    \
}

/* -------------------------------------------------------------------------- */
char *random_string(int size) {
    if (size < 1) return NULL;
    char *str = (char *) malloc((size) * sizeof(char));

    for (int i = 0; i < size; i++) {
        str[i] = characters[rand() % strlen(characters)];
    }
    return str;
}
/* -------------------------------------------------------------------------- */

void execute_operation(CBArray *cba, char *op_name, int arg, int block_size) {

#ifdef DYNAMIC
    void (*add_block)(CBArray*, int, char*) = dlsym(handle, "add_block");
    void (*remove_block)(CBArray*, int) = dlsym(handle, "remove_block");
    char (*find_closest_block)(CBArray*, int) = dlsym(handle, "find_closest_block");
#endif

    if (strcmp(op_name, "change") == 0) {
        MEASURE(remove then add, MULTI_TEST,
            //first_remove_then_add(cba, arg, block_size);
            for (int i = 0; i < arg; i++) {
                remove_block(cba, i);
            }
            for (int i = 0; i < arg; i++) {
                char *block = random_string(block_size);
                add_block(cba, i, block);
            }
        )

    } else if (strcmp(op_name, "alt_change") == 0) {
        MEASURE(alternately remove and add, MULTI_TEST,
            //alt_remove_and_add(cba, arg, block_size);
            for (int i = 0; i < arg; i++) {
                remove_block(cba, i);
                add_block(cba, i, random_string(block_size));
            }
        )

    } else if (strcmp(op_name, "find") == 0) {
        MEASURE(find closest to the given block, MULTI_TEST,
            find_closest_block(cba, arg);
        )

    } else if (strcmp(op_name, "gen") == 0) {
        for (int i = 0; i < cba->arr_size; i++) {
            char *randomString = random_string(block_size);
            add_block(cba, i, randomString);
        }
    }
}
/* -------------------------------------------------------------------------- */

int main(int argc, char **argv) {

    srand((unsigned int) time(NULL));
    /*
		argv[0] - prog name
		argv[1] - blocks number
		argv[2] - block size
		argv[3] - alocation
                - list of operations:
		argv[4] - init
        argv[5] - operation no.1
        argv[6] - argument no.1
        argv[7] - operation no.2
        argv[8] - argument no.2
	*/
    #ifdef DYNAMIC
        printf("-------------------------------------\n");
        printf("------------->> Using dynamic library\n");
    #endif

    #ifdef SHARED
        printf("-------------------------------------\n");
        printf("-------------->> Using shared library\n");
    #endif

    #ifdef STATIC
        printf("-------------------------------------\n");
        printf("-------------->> Using static library\n");
    #endif

    if (argc < 4) {
        printf("More arguments you should enter. Give me please: array_size,");
        printf("block_size, allocation_method and then list of operations!");
        return 2;
    }

    int array_size = (int) strtol(argv[1], '\0', 10);
    int block_size = (int) strtol(argv[2], '\0', 10);
    
    int allocation_method;
    if (strcmp(argv[3], "D") == 0) {
        allocation_method = 1;
    } else if (strcmp(argv[3], "S") == 0){
        allocation_method = 0;
    } else {
        printf("Wrong type of memory allocation! Use \"D\" or \"S\". \n");
        return 1;
    }


    char *op_name[2];
    int args[2];

    if (strcmp(argv[4], "init") != 0) {
        printf("Wrong arguments you entered. List of operation with");
        printf("\"init\" must start, char-block-array to create.\n");
        return 2;
    }
    if (argc != 9) {
        printf("Stop. Proper command line, as follows:\n");
        printf("./main array_size block_size allocation_method ");
        printf("[list of 3 operations - starting with \"init\"]\n");
        printf("./main array_size block_size D/S init find 5 alt_change 10\n");
        return 2;
    } else {
        op_name[0] = argv[5];
        args[0] = (int) strtol(argv[6], '\0', 10);
        op_name[1] = argv[7];
        args[1] = (int) strtol(argv[8], '\0', 10);
    }


    printf("-------------------------------------\n");
    printf("array_size = %d   block_size = %d\nallocation_method = ", array_size, block_size);
    if (allocation_method)
        printf("dynamic\n");
    else
        printf("static\n");
    printf("-------------------------------------\n");

    #ifdef DYNAMIC
        load_lib();
        CBArray* (*initialize_array)(int, int, int) = dlsym(handle, "initialize_array");
        void (*delete_array)(CBArray*) = dlsym(handle, "delete_array");
    #endif

    CBArray *cba;

    MEASURE(initialization, MULTI_TEST, {
        cba = initialize_array(array_size, block_size, allocation_method);
        execute_operation(cba, "gen", 0, block_size);
        }
    )

    for(int i = 0; i < 2; i++) {
        execute_operation(cba, op_name[i], args[i], block_size);
    }

    delete_array(cba);
    printf("=====================================\n\n");

    #ifdef DYNAMIC
        close_lib();
    #endif

    return 0;
}
