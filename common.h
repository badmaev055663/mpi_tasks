#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mpi.h>

/* iterations for obtaining average */
#define NUM_ITER 5

#define MAX_RAND_VAL 1000000
#define RAND_OFFSET -500000

#define MAX_STR_LEN 64
#define M 1000000
#define LOCAL_BUFF_SIZE 64

void rand_fill_vec_int(int* vec, int n);
int** alloc_sqr_mat(int n);
void free_sqr_mat(int **mat, int n);
