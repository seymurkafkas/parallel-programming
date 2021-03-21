/* Fox's Algorithm for Matrix Multiplication
 *    Author: Seymur Kafkas
 */

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct Matrix
{
    float **entries;
    int dimensions;
} Matrix;

void readInput()
{
}

void multiplyMatrices(Matrix *first, Matrix *second)
{
}

int indexAt(int row, int column, Matrix *m)
{
    return m->dimension * row + column;
}

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int processCount;
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int token;
    // Finalize the MPI environment.
    MPI_Finalize();
}
