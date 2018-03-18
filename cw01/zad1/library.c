/*
  library.c
  11.03.2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "library.h"

char static_cbarray[2000][500];

/* ---------------------------------------------------------------------- */
CBArray *initialize_array(int arr_size, int block_size, int is_dynamic) {
    if (arr_size < 0) return NULL;

    CBArray *cba = (CBArray *) calloc(1, sizeof(CBArray));
    cba->arr_size = arr_size;
    cba->block_size = block_size;
    cba->is_dynamic = is_dynamic;

    if (is_dynamic) {
        char **dynamic_cbarray = (char **) calloc(arr_size, sizeof(char *));
        cba->arr = dynamic_cbarray;
    } else {
        cba->arr = (char **) static_cbarray;
    }

    return cba;
}

void delete_array(CBArray *cba) {
    if (cba->is_dynamic) {
        for (int i = 0; i < cba->arr_size; ++i) {
            free(cba->arr[i]);
        }
        free(cba->arr);
        free(cba);
        return;
    } else {
        for (int i = 0; i < cba->arr_size; ++i) {
            cba->arr[i] = "";
        }
    }
}

/* ---------------------------------------------------------------------- */
void add_block(CBArray *cba, int index, char *block) {
    if (index < 0 || index >= cba->arr_size) return;
    if (cba->is_dynamic) {
        cba->arr[index] = calloc(strlen(block), sizeof(char));
        strcpy(cba->arr[index], block);
    } else {
        cba->arr[index] = block;
    }
}

void remove_block(CBArray *cba, int index) {
    if (cba == NULL || cba->arr[index] == NULL) return;
    if (cba->is_dynamic) {
        free(cba->arr[index]);
        cba->arr[index] = NULL;
    } else {
        cba->arr[index] = "";
    }
}

/* ---------------------------------------------------------------------- */
int count_ascii_sum(char *block) {
    int sum = 0;
    for (int i = 0; i < strlen(block); i++)
        sum += (int) block[i];
    return sum;
}

char *find_closest_block(CBArray *cba, int index) {
    char *ans = NULL;
    int closest_diff = INT_MAX;
    int sum = count_ascii_sum(cba->arr[index]);

    for (int i = 0; i < cba->arr_size; i++) {
        char *tmp_block = cba->arr[i];
        if (i != index) {
            int diff = abs(count_ascii_sum(tmp_block) - sum);
            if (closest_diff > diff) {
                closest_diff = diff;
                ans = tmp_block;
            }
        }
    }
    return ans;
}
/* ---------------------------------------------------------------------- */
