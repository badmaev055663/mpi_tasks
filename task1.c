#include "common.h"

#define SEQ 0
#define MPI 1

int find_min(int* vec, int n)
{
    int min_val = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (min_val >= vec[i])
            min_val = vec[i];
    }
    return min_val;
}

int find_min_mpi(int* vec, int n)
{
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int *buf = (int*)malloc(sizeof(int) * n / size);
    int res_buf[LOCAL_BUFF_SIZE];

    int local_min = INT_MAX;
    MPI_Scatter(vec, n / size, MPI_INT, buf, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < n / size; i++) {
        if (local_min >= buf[i])
            local_min = buf[i];
    }
    MPI_Gather(&local_min, 1, MPI_INT, res_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
    free(buf);

    if (rank == 0) {
        return find_min(res_buf, size);
    }
}

static long results_mpi[NUM_ITER];
static long results_seq[NUM_ITER];

static int test(int n, int iter)
{
    int rank, min1, min2;
    int *vec;
    struct timeval begin, end;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        vec = (int*)malloc(sizeof(int) * n);
        rand_fill_vec_int(vec, n);
        gettimeofday(&begin, 0);
        min1 = find_min(vec, n);
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        long elapsed = seconds * 1e6 + microseconds;
        results_seq[iter] = elapsed;
    }
    if (rank == 0) {
        gettimeofday(&begin, 0);
    }
    min2 = find_min_mpi(vec, n);
    if (rank == 0) {
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        long elapsed = seconds * 1e6 + microseconds;
        if (min1 != min2) {
            printf("fail !!! seq min: %d; mpi min: %d\n", min1, min2);
            free(vec);
            return 1;
        }
        results_mpi[iter] = elapsed;
        free(vec);
        return 0;
    }
    return 0;
}


void get_average()
{
    long mpi_average = 0;
    long seq_average = 0;
    for (int j = 0 ; j < NUM_ITER; j++) {
        mpi_average += results_mpi[j];
        seq_average += results_seq[j];
    } 
    printf("mpi average (ms): %.1lf\n", mpi_average / (NUM_ITER * 1000.));
    printf("seq average (ms): %.1lf\n", seq_average / (NUM_ITER * 1000.));
    memset(results_mpi, 0, sizeof(long) * NUM_ITER);
    memset(results_seq, 0, sizeof(long) * NUM_ITER);
}

static void big_test(int n)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
        printf("test size: %d\n", n);
    for (int i = 0 ; i < NUM_ITER; i++) {
        if (test(n, i)) {
            printf("test failed\n");
            return;
        }
    }      
    if (rank == 0)
        get_average();
}

int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);
    big_test(M);
    big_test(2 * M);
    big_test(5 * M);
    big_test(10 * M);
    big_test(20 * M);
    MPI_Finalize();
 
    return 0;
}