/* Fox's Algorithm for Matrix Multiplication
 *    Author: Seymur Kafkas
 */

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct
{
    float **entries;
    int dimensions;
} Matrix;

typedef struct
{
    MPI_Comm gridComm;
    MPI_Comm rowComm;
    MPI_Comm columnComm;
    int processCount;
    int gridOrder; //Assumed to be the square root processCount
    int myRank;
    int rowNumber;
    int columnNumber;
} ProcessInfo;
void readInputMatrix()
{
}

void initialiseGrid(
    ProcessInfo *grid /* out */)
{
    int prevRank;
    int dimensions[2];
    int isCircular[2];
    int coordinates[2];
    int freeCoords[2];

    /* Set up Global Grid Information */
    MPI_Comm_size(MPI_COMM_WORLD, &(grid->processCount));
    MPI_Comm_rank(MPI_COMM_WORLD, &prevRank);

    /* We assume p is a perfect square */
    grid->gridOrder = (int)sqrt((double)grid->processCount);
    dimensions[0] = dimensions[1] = grid->gridOrder;

    /* We want a circular shift in second dimension. */
    /* Don't care about first                        */
    isCircular[0] = 1;
    isCircular[1] = 1;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions,
                    isCircular, 1, &(grid->gridComm));
    MPI_Comm_rank(grid->gridComm, &(grid->myRank));
    MPI_Cart_coords(grid->gridComm, grid->myRank, 2,
                    coordinates);
    grid->rowNumber = coordinates[0];
    grid->columnNumber = coordinates[1];

    /* Set up row communicators */
    freeCoords[0] = 0;
    freeCoords[1] = 1;
    MPI_Cart_sub(grid->gridComm, freeCoords,
                 &(grid->rowComm));

    freeCoords[0] = 1;
    freeCoords[1] = 0;

    MPI_Cart_sub(grid->gridComm, freeCoords,
                 &(grid->columnComm));
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
