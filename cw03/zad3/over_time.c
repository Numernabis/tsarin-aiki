#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    double x = 2.71828182846;
    double y = 44.44;
    long num = strtol(argv[1], 0, 10);
    for (int i = 0; i < num * num; i++) {
        x = cos(y);
        y = sin(x) * x;
    }
    printf("Success!\tResult: x = %lf  y = %lf\n", x, y);
    return 0;
}
