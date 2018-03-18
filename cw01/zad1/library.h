/*
  library.h
  11.03.2018
*/

#ifndef ZESTAW1_LIBRARY_H
#define ZESTAW1_LIBRARY_H

typedef struct CBArray{
    char **arr;
    int arr_size;
    int block_size;
    int is_dynamic;
} CBArray;

extern char static_cbarray[2000][500];

/* Zaprojektuj i przygotuj zestaw funkcji (bibliotekę)
do zarządzania tablicą bloków zawierających znaki.
Biblioteka powinna umożliwiać:  */

/* - tworzenie i usuwanie tablicy */
CBArray *initialize_array(int arr_size, int block_size, int is_dynamic);
void delete_array(CBArray *cba);

/* - dodanie i usunięcie bloków na które wskazują wybrane indeksy elementów tablicy  */
void add_block(CBArray *cba, int index, char *block);
void remove_block(CBArray *cba, int index);

/* - wyszukiwanie bloku w tablicy, którego suma znaków (kodów ASCII)
w bloku jest najbliższa elementowi o zadanym numerze */
char *find_closest_block(CBArray *cba, int index);
int count_ascii_sum(char *block);

#endif
