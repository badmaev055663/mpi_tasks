#include "common.h"

void rand_fill_vec_int(int* vec, int n)
{
    srand(clock());
    for (int i = 0; i < n; i++) {
        vec[i] = RAND_OFFSET + rand() % MAX_RAND_VAL;
    }
}

int** alloc_sqr_mat(int n)
{
    int **res = (int**)malloc(sizeof(int*) * n);
    int i;
    if (!res)
        return NULL;
    for (i = 0; i < n; i++) {
        res[i] = (int*)malloc(sizeof(int) * n);
        if (!res[i])
            goto clean;
    }
    return res;

clean:
    for (int j = 0; j < i; j++)
        free(res[j]);
    free(res);
    return NULL;
}

void free_sqr_mat(int **mat, int n)
{
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}
