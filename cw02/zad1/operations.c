/*
  author: Ludi
  file:   operations.c
  start:  16.03.2018
  end:    []
  lines:  219
*/
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "operations.h"

/* -------------------------------------------------------------------------- */
static const char *characters =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

char *random_string(int size_of_record) {
    if (size_of_record < 1) return NULL;
    char *str = (char *) malloc((size_of_record) * sizeof(char));

    for (int i = 0; i < size_of_record; i++) {
        str[i] = characters[rand() % strlen(characters)];
    }
    return str;
}
void print_read_error(char *file_name) {
    printf("Error occured while reading from %s.\n", file_name);
}
void print_write_error(char *file_name) {
    printf("Error occured while writing to %s.\n", file_name);
}

/* -------------------------------------------------------------------------- */
void generate_file_lib(char *file_name, int number_of_records, int size_of_record) {
    FILE *file = fopen(file_name, "w+");
    char *buff = (char *) calloc(size_of_record, sizeof(char));

    for (int i = 0; i < number_of_records; i++) {
        buff = random_string(size_of_record);
        buff[size_of_record - 1] = 10; // LF - line feed

        if (fwrite(buff, sizeof(char), size_of_record, file) != size_of_record) {
            print_write_error(file_name);
            return;
        }
    }
    free(buff);
    fclose(file);
}
void generate_file_sys(char *file_name, int number_of_records, int size_of_record) {
    int file = open(file_name, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
    char *buff = (char *) calloc(size_of_record, sizeof(char));

    for (int i = 0; i < number_of_records; i++) {
        buff = random_string(size_of_record);
        buff[size_of_record - 1] = 10; // LF - line feed

        if (write(file, buff, size_of_record) != size_of_record) {
            print_write_error(file_name);
            return;
        }
    }
    free(buff);
    close(file);
}
/* -------------------------------------------------------------------------- */
/* Insertion Sort (classic):
int i, j, value;
for (i = 1; i < length; ++i) {
    value = a[i];
    for (j = i - 1; j >= 0 && a[j] > value; --j)
        a[j + 1] = a[j];
    a[j + 1] = value;
}
*/
void sort_file_lib(char *file_name, int number_of_records, int size_of_record) {
    int i, j;
    FILE *file = fopen(file_name, "r+");
    long int offset = (long int) (size_of_record * sizeof(char));
    char* val = (char *) calloc(size_of_record, sizeof(char));
    char* tmp = (char *) calloc(size_of_record, sizeof(char));

    for (i = 1; i < number_of_records; i++) {
        fseek(file, i * offset, SEEK_SET);
        if (fread(val, sizeof(char), size_of_record, file) != size_of_record) {
            print_read_error(file_name);
            return;
        }

        fseek(file, (-2) * offset, SEEK_CUR);
        if (fread(tmp, sizeof(char), size_of_record, file) != size_of_record) {
            print_read_error(file_name);
            return;
        }

        for (j = i - 1; j > 0 && (int) tmp[0] > (int) val[0]; j--) {
            if (fwrite(tmp, sizeof(char), size_of_record, file) != size_of_record) {
                print_write_error(file_name);
                return;
            }

            fseek(file, (-3) * offset, SEEK_CUR);
            if (fread(tmp, sizeof(char), size_of_record, file) != size_of_record) {
                print_read_error(file_name);
                return;
            }
        }

        if ((int) tmp[0] > (int) val[0] && j == 0){
            if (fwrite(tmp, sizeof(char), size_of_record, file) != size_of_record) {
                print_write_error(file_name);
                return;
            }
            fseek(file, (-2) * offset, SEEK_CUR);
        }

        if (fwrite(val, sizeof(char), size_of_record, file) != size_of_record) {
            print_write_error(file_name);
            return;
        }
    }
    free(tmp);
    free(val);
    fclose(file);
}
void sort_file_sys(char *file_name, int number_of_records, int size_of_record) {
    int i, j;
    int file = open(file_name, O_RDWR, O_TRUNC);
    long int offset = (long int) (size_of_record * sizeof(char));
    char* val = (char *) calloc(size_of_record, sizeof(char));
    char* tmp = (char *) calloc(size_of_record, sizeof(char));

    for (i = 1; i < number_of_records; i++) {
        lseek(file, i * offset, SEEK_SET);
        if (read(file, val, size_of_record) != size_of_record) {
            print_read_error(file_name);
            return;
        }

        lseek(file, (-2) * offset, SEEK_CUR);
        if (read(file, tmp, size_of_record) != size_of_record) {
            print_read_error(file_name);
            return;
        }

        for (j = i - 1; j > 0 && (int) tmp[0] > (int) val[0]; j--) {
            if (write(file, tmp, size_of_record) != size_of_record) {
                print_write_error(file_name);
                return;
            }

            lseek(file, (-3) * offset, SEEK_CUR);
            if (read(file, tmp, size_of_record) != size_of_record) {
                print_read_error(file_name);
                return;
            }
        }

        if ((int) tmp[0] > (int) val[0] && j == 0){
            if (write(file, tmp, size_of_record) != size_of_record) {
                print_write_error(file_name);
                return;
            }
            lseek(file, (-2) * offset, SEEK_CUR);
        }

        if (write(file, val, size_of_record) != size_of_record) {
            print_write_error(file_name);
            return;
        }
    }
    free(tmp);
    free(val);
    close(file);
}

/* -------------------------------------------------------------------------- */
void copy_file_lib(char *file_name_1, char *file_name_2, int number_of_records, int buffer_size) {
    FILE *src = fopen(file_name_1, "r");
    FILE *dest = fopen(file_name_2, "w+");
    char *buff = (char *) calloc(buffer_size, sizeof(char));

    for (int i = 0; i < number_of_records; i++) {
        if (fread(buff, sizeof(char), buffer_size, src) != buffer_size) {
            print_read_error(file_name_1);
            return;
        }
        if (fwrite(buff, sizeof(char), buffer_size, dest) != buffer_size) {
            print_write_error(file_name_2);
            return;
        }
    }
    free(buff);
    fclose(src);
    fclose(dest);
}
void copy_file_sys(char *file_name_1, char *file_name_2, int number_of_records, int buffer_size) {
    int src = open(file_name_1, O_RDONLY);
    int dest = open(file_name_2, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
    char *buff = (char *) calloc(buffer_size, sizeof(char));

    for (int i = 0; i < number_of_records; i++) {
        if (read(src, buff, buffer_size) != buffer_size) {
            print_read_error(file_name_1);
            return;
        }
        if (write(dest, buff, buffer_size) != buffer_size) {
            print_write_error(file_name_2);
            return;
        }
    }
    free(buff);
    close(src);
    close(dest);
}
/* -------------------------------------------------------------------------- */
