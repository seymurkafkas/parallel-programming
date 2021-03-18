#include <mpi.h>
#include <stdio.h>

typedef struct Matrix
{
    float **entries;
    int dimensions;
} Matrix;

int indexAt(int row, int column, Matrix *m)
{
    return m->dimension * row + column;
}

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Finalize the MPI environment.
    MPI_Finalize();
}
