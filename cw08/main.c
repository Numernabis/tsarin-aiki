/*
  author: Ludi
  file:   main.c
  start:  29.05.2018
  end:    []
  lines:  []
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct Image Image;
typedef struct Filter Filter;

struct Image {
    int **I, **J;
    int w, h, m;
};
struct Filter {
    double **K;
    int c;
};

Image image;
Filter filter;
int threads_num;
int max(int a, int b) { return a > b ? a : b; }
int min(int a, int b) { return a < b ? a : b; }

/* -------------------------------------------------------------------------- */
Image load_image(char* path) {
    Image image;
    FILE* input = fopen(path, "r");
    fscanf(input, "P2 %d %d %d", &image.w, &image.h, &image.m);

    image.I = calloc(image.w, sizeof(int*));
    image.J = calloc(image.w, sizeof(int*));

    for (int n = 0; n < image.w; ++n) {
        image.I[n] = calloc(image.h, sizeof(int));
        image.J[n] = calloc(image.h, sizeof(int));
        for (int m = 0; m < image.h; ++m) {
            fscanf(input, "%d ", &image.I[n][m]);
        }
    }
    fclose(input);
    return image;
}

Filter load_filter(char* path) {
    Filter filter;
    FILE* ffile = fopen(path, "r");
    fscanf(ffile, "%d\n", &filter.c);

    filter.K = calloc(filter.c, sizeof(double*));
    for (int i = 0; i < filter.c; ++i) {
        filter.K[i] = calloc(filter.c, sizeof(double));
        for (int j = 0; j < filter.c; ++j) {
            fscanf(ffile, "%lf ", &filter.K[i][j]);
        }
    }
    fclose(ffile);
    return filter;
}

void save_image(Image image, char* path) {
    FILE* output = fopen(path, "w");
    fprintf(output, "P2\n%d %d\n%d\n", image.w, image.h, image.m);

    for (int n = 0; n < image.w; ++n) {
        for (int m = 0; m < image.h; ++m) {
            fprintf(output, "%d ", image.J[n][m]);
        }
        fprintf(output, "\n");
    }
    fprintf(output, "\n");
    fclose(output);
}

/* -------------------------------------------------------------------------- */
int filter_pixel(int x, int y) {
    int c = filter.c;
    double sum = 0;
    for (int i = 0; i < c; ++i) {
        for (int j = 0; j < c; ++j) {
            int ix = min(max(0, x - ceil(c / 2) + i), image.w - 1);
            int iy = min(max(0, y - ceil(c / 2) + j), image.h - 1);
            sum += image.I[ix][iy] * filter.K[i][j];
        }
    }
    return round(sum);
}

void *process_image(void *param) {
    int i = *(int*) param;
    int start, end;
    start = image.w * i / threads_num;
    end = image.w * (i + 1) / threads_num;

    for (int x = start; x < end; ++x) {
        for (int y = 0; y < image.h; ++y) {
            image.J[x][y] = filter_pixel(x, y);
        }
    }
    pthread_exit(param);
}

/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Invalid program call. Proper arguments as follows:\n");
        printf("./main  threads_num  input_path  filter_path  output_path\n");
        return 2;
    }
    threads_num = atoi(argv[1]);
    image = load_image(argv[2]);
    filter = load_filter(argv[3]);
    char* output_path = argv[4];

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    /* ---------------------------------------------------------------------- */
    pthread_t *threads = malloc(threads_num * sizeof(pthread_t));
    for (int i = 0; i < threads_num; ++i) {
        int *param = malloc(sizeof(int));
        *param = i;
        pthread_create(&(threads[i]), NULL, process_image, param);
    }
    for (int i = 0; i < threads_num; ++i) {
        void *pointer_for_extra_needs;
        pthread_join(threads[i], &pointer_for_extra_needs);
    }
    /* ---------------------------------------------------------------------- */
    clock_gettime(CLOCK_REALTIME, &end);
    unsigned long ms = (end.tv_sec - start.tv_sec) * 1000000
                     + (end.tv_nsec - start.tv_nsec) / 1000;
    FILE *fp = fopen("Times.txt", "a");
    fprintf(fp, "|  %dx%-4d |   %dx%-4d |  %-12d |  %-15lu |\n",
        image.w, image.h, filter.c, filter.c, threads_num, ms);
    fclose(fp);

    free(threads);
    save_image(image, output_path);
}
/* -------------------------------------------------------------------------- */
