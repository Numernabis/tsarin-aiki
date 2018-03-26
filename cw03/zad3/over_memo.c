#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int blocks = atoi(argv[1]);
    int size = atoi(argv[2]);
    int total = 0;
    char* block;
    for (int i = 0; i < blocks; i++) {
        printf("Allocating %d MB for a total of %d MB.\n", size, total + size);
        block = malloc(size * 1024 * 1024);
        memset(block, 1, size * 1024 * 1024);
        total += size;
    }
    return 0;
}
