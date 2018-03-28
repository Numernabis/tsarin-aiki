#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    int blocks = atoi(argv[1]);
    int size = atoi(argv[2]);
    int size_mb = size * 1024 * 1024;
    int total = 0;
    for (int i = 0; i < blocks; i++) {
        printf("Total allocated memory: %d MB\n", total + size);
        char* block = malloc(size_mb);
        memset(block, 1, size_mb);
        total += size;
    }
    printf("Success! Allocated %dx%d MB of memory.\n", blocks, size);
    return 0;
}
