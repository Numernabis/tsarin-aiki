/*
  author: Ludi
  file:   operations.h
  start:  16.03.2018
  end:    []
  lines:  24
*/
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void generate_file_lib(char *file1, int number_of_records, int size_of_record);
void generate_file_sys(char *file1, int number_of_records, int size_of_record);

void sort_file_lib(char *file1, int number_of_records, int size_of_record);
void sort_file_sys(char *file1, int number_of_records, int size_of_record);

void copy_file_lib(char *file1, char *file2, int number_of_records, int buffer_size);
void copy_file_sys(char *file1, char *file2, int number_of_records, int buffer_size);
