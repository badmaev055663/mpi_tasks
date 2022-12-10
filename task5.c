#include "common.h"


// cnt - количество строк котрое мы берем в mat1 для ленточного алгоритма
void sqr_mat_mult(int *mat1, int *mat2, int *res, int n, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		for (int j = 0; j < n; j++) {
            int tmp = 0;
	        for (int k = 0; k < n; k++) {
		        tmp += mat1[i * n + k] * mat2[k * n + j];
	        }
			res[i * n + j] = tmp;
		}
	}
}

void mpi_sqr_mat_mult(int *mat1, int *mat2, int *res, int n)
{
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int *buf1 = (int*)malloc(sizeof(int) * n * n / size); // for mat1
    int *buf2 = (int*)malloc(sizeof(int) * n * n); // for mat2
    int *res_buf = (int*)malloc(sizeof(int) * n * n / size);
    if (rank == 0) {
        memcpy(buf2, mat2, n * n * sizeof(int));
    }

    MPI_Scatter(mat1, n * n / size, MPI_INT, buf1, n * n / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(buf2, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    sqr_mat_mult(buf1, buf2, res_buf, n, n / size);
    MPI_Gather(res_buf, n * n / size, MPI_INT, res, n * n / size, MPI_INT, 0, MPI_COMM_WORLD);
    free(buf1);
    free(buf2);
    free(res_buf);
}


static double results_mpi[NUM_ITER];
static double results_seq[NUM_ITER];

static int test(int n, int iter)
{
    int rank, ret;
    int *mat1, *mat2, *res1, *res2;
    double t1, t2;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        mat1 = (int*)malloc(sizeof(int) * n * n);
        mat2 = (int*)malloc(sizeof(int) * n * n);
        res1 = (int*)malloc(sizeof(int) * n * n);
        res2 = (int*)malloc(sizeof(int) * n * n);
        rand_fill_vec_int(mat1, n * n);
        rand_fill_vec_int(mat2, n * n);
        t1 = MPI_Wtime();
        sqr_mat_mult(mat1, mat2, res1, n, n);
        t2 = MPI_Wtime();
        results_seq[iter] = t2 - t1;
    }
    if (rank == 0) {
        t1 = MPI_Wtime();
    }
    mpi_sqr_mat_mult(mat1, mat2, res2, n);
    if (rank == 0) {
        t2 = MPI_Wtime();
        if (memcmp(res1, res2, n * n * sizeof(int))) {
            printf("fail !!!\n");
            ret = 1;
        } else {
            results_mpi[iter] = t2 - t1;
            ret = 0;
        }
        free(mat1);
        free(mat2);
        free(res1);
        free(res2);
    }
    return ret;
}


void get_average()
{
    double mpi_average = 0;
    double seq_average = 0;
    for (int j = 0 ; j < NUM_ITER; j++) {
        mpi_average += results_mpi[j];
        seq_average += results_seq[j];
    } 
    printf("mpi average (ms): %.1lf\n", mpi_average * 1000 / NUM_ITER);
    printf("seq average (ms): %.1lf\n", seq_average * 1000 / NUM_ITER);
    for (int j = 0 ; j < NUM_ITER; j++) {
        results_mpi[j] = results_seq[j] = 0.0;
    }
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
    big_test(160);
    big_test(320);
    big_test(480);
 
    MPI_Finalize();
 
    return 0;
}