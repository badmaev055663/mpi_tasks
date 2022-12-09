#include "common.h"

#define SEQ 0
#define MPI 1

int dot_product(int* vec1, int* vec2, int n)
{
    int res = 0;
    for (int i = 0; i < n; i++) {
        res += vec1[i] * vec2[i];
    }
    return res;
}

int dot_product_mpi(int* vec1, int* vec2, int n)
{
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int *buf1 = (int*)malloc(sizeof(int) * n / size);
    int *buf2 = (int*)malloc(sizeof(int) * n / size);
    int res_buf[LOCAL_BUFF_SIZE];

    MPI_Scatter(vec1, n / size, MPI_INT, buf1, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec2, n / size, MPI_INT, buf2, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    int res = dot_product(buf1, buf2, n / size);
    
    MPI_Gather(&res, 1, MPI_INT, res_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
    free(buf1);
    free(buf2);

    if (rank == 0) {
        int total_res = 0;
        for (int i = 0; i < size; i++) {
                total_res += res_buf[i];
        }
        return total_res;
    }
}

static long results_mpi[NUM_ITER];
static long results_seq[NUM_ITER];

static int test(int n, int iter)
{
    int rank, res1, res2;
    int *vec1, *vec2;
    struct timeval begin, end;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        vec1 = (int*)malloc(sizeof(int) * n);
        vec2 = (int*)malloc(sizeof(int) * n);
        rand_fill_vec_int(vec1, n);
        rand_fill_vec_int(vec2, n);
        gettimeofday(&begin, 0);
        res1 = dot_product(vec1, vec2, n);
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        long elapsed = seconds * 1e6 + microseconds;
        results_seq[iter] = elapsed;
    }
    if (rank == 0) {
        gettimeofday(&begin, 0);
    }
    res2 = dot_product_mpi(vec1, vec2, n);
    if (rank == 0) {
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        long elapsed = seconds * 1e6 + microseconds;
        if (res1 != res2) {
            printf("fail !!! seq res: %d; mpi res: %d\n", res1, res2);
            free(vec1);
            free(vec2);
            return 1;
        }
        results_mpi[iter] = elapsed;
        free(vec1);
        free(vec2);
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