#include "common.h"

void rand_fill_vec_int(int* vec, int n)
{
    srand(clock());
    for (int i = 0; i < n; i++) {
        vec[i] = RAND_OFFSET + rand() % MAX_RAND_VAL;
    }
}
