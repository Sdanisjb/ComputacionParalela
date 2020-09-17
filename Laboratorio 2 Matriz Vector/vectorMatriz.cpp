#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define ELEM 200

void Print_vector(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm) {
    double* b = NULL;
    int i;
    if (my_rank == 0) {
        b = (double*)malloc(n * sizeof(double));
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, comm);
        printf("%s\n", title);
        for (i = 0; i < n; i++) {
            printf("%f\t", b[i]);
        }
        printf("\n");
        free(b);
    }
    else {
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, comm);
    }
}

void Mat_vect_mult(double local_A[], double local_x[], double local_y[], int local_m, int n, int local_n, MPI_Comm comm) {
    double* x;
    int local_i, j;
    int local_ok = 1;
    x = (double*)malloc(n * sizeof(double));
    MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, comm);
    for (local_i = 0; local_i < local_m; local_i++) {
        local_y[local_i] = 0.0;
        for (j = 0; j < n; j++) {
            local_y[local_i] += local_A[local_i * n + j] * x[j];
        }
    }
    free(x);
}

int main() {
    int mult_size;
    int mult_rank;
    int m = ELEM, n = ELEM;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mult_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mult_rank);
    const int local_n = n / mult_size;
    const int local_m = m / mult_size;
    double* Vec = new double[local_n];
    double* Res = new double[local_m];
    double* Mat = new double[local_n * local_m];
    for (int i = 0; i < local_n; i++) {
        Vec[i] = i;
        for (int j = 0; j < local_m; j++) {
            Mat[i * local_m + j] = i * local_m + j;
        }
    }
    double start, finish, local_elapsed, elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    Mat_vect_mult(Mat, Vec, Res, local_m, n, local_n, MPI_COMM_WORLD);
    finish = MPI_Wtime();
    local_elapsed = finish - start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (mult_rank == 0)
        printf("Se necesitaron %f segundos con una matriz de %dx%d elementos\n", finish - start, n, m);
    MPI_Finalize();
    return 0;
}