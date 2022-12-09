#include "common.h"


double send_msg(int n, int dst, int *buf)
{
    rand_fill_vec_int(buf, n);
    double t1 = MPI_Wtime();
    int ret = MPI_Send(buf, n, MPI_INT, dst, 0, MPI_COMM_WORLD);
    if (ret) {
		printf("send failed with code: %d\n", ret);
        return -1;
	}
    double t2 = MPI_Wtime();
    return t2 - t1;
}

double recv_msg(int n, int src, int *buf)
{
    MPI_Status status;
    double t1 = MPI_Wtime();
	int ret = MPI_Recv(buf, n, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
	if (ret) {
		printf("receive failed with code: %d\n", ret);
        return -1;
	}
    double t2 = MPI_Wtime();
    return t2 - t1;
}

int send_recv_test(int n, int k)
{
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int *send_buf = (int*)malloc(sizeof(int) * n);
    int *recv_buf = (int*)malloc(sizeof(int) * n);
    double t_send = 0, t_recv = 0;
    for (int i = 0; i < k; i++) {
        if (rank == 0) {
            t_send += send_msg(n, 1, send_buf);
            t_recv += recv_msg(n, 1, recv_buf);
        } else if (rank == 1) {
            t_recv += recv_msg(n, 0, recv_buf);
            t_send += send_msg(n, 0, send_buf);
        }
    }
    if (rank == 0) {
        printf("msg length: %d\n", n);
        printf("average send time from 0 to 1 (us): %.2lf\n", t_send * M / k);
        printf("average recv time from 1 (us): %.2lf\n", t_recv * M / k);
    } else if (rank == 1) {
        printf("average send time from 1 to 0 (us): %.2lf\n", t_send * M / k);
        printf("average recv time from 0 (us): %.2lf\n", t_recv * M / k);
    }
    free(send_buf);
    free(recv_buf);
    
}


int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);
    send_recv_test(200, 1000);
    send_recv_test(500, 1000);
    send_recv_test(1000, 500);
    send_recv_test(2000, 200);
    send_recv_test(5000, 200);
    send_recv_test(10000, 100);
    MPI_Finalize();
 
    return 0;
}