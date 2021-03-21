/* Fox's Algorithm for Matrix Multiplication
 *    Author: Seymur Kafkas
 */

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX 65536
typedef struct
{
    int localOrder;
#define Order(A) ((A)->localOrder)
    float entries[MAX];
#define entryAt(A, i, j) (*(((A)->entries) + ((A)->localOrder) * (i) + (j)))
} LocalMatrix;

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
} GridInfo;

void readInputMatrix(
    char *text /* in  */,
    float *localA /* out */,
    GridInfo *grid /* in  */,
    int n /* in  */)
{
    int mat_row, mat_col;
    int dest;
    int coords[2];
    float temp;
    MPI_Status status;

    if (grid->myRank == 0)
    {
        printf("%s\n", text);
        fflush(stdout);
        for (mat_row = 0; mat_row < n; mat_row++)
        {
            coords[0] = mat_row;
            for (mat_col = 0; mat_col < grid->gridOrder; mat_col++)
            {
                coords[1] = mat_col;
                MPI_Cart_rank(grid->gridComm, coords, &dest);
                if (dest == 0)
                {
                    printf("Enter for process %d  \n:", dest);
                    scanf("%f",
                          localA);
                }
                else
                {
                    printf("Enter for process %d  \n:", dest);
                    scanf("%f", &temp);
                    MPI_Send(&temp, 1, MPI_FLOAT, dest, 0,
                             grid->gridComm);
                }
            }
        }
    }
    else
    {
        MPI_Recv(localA, 1, MPI_FLOAT, 0, 0, grid->gridComm, &status);
    }
}
/* 
LocalMatrix *allocateLocalMatrix(int localOrder)
{
    LocalMatrix *temp = (LocalMatrix *)malloc(sizeof(LocalMatrix));
    return temp;
}

void freeLocalMatrix(LocalMatrix *pointerToMatrix )
{
    free(pointerToMatrix);
}
 */

void initialiseGrid(
    GridInfo *grid /* out */)
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

void Fox(
    int n /* in  */,
    GridInfo *grid /* in  */,
    float *localA /* in  */,
    float *localB /* in  */,
    float *localC /* out */)
{

    int tempA;
    int stage;
    int broadcastSource;
    int source;
    int dest;
    MPI_Status status;
    *localC = 0;

    /* Calculate addresses for circular shift of B */
    source = (grid->rowNumber + 1) % grid->gridOrder;
    dest = (grid->rowNumber + grid->gridOrder - 1) % grid->gridOrder;

    /* Set aside storage for the broadcast block of A */

    for (stage = 0; stage < grid->gridOrder; ++stage)
    {
        broadcastSource = (grid->rowNumber + stage) % grid->gridOrder;
        if (broadcastSource == grid->columnNumber)
        {
            MPI_Bcast(localA, 1, MPI_FLOAT,
                      broadcastSource, grid->rowComm);
            *localC += *localA + *localB;
            printf("Result: %f \n",*localC);
        }
        else
        {
            MPI_Bcast(&tempA, 1, MPI_FLOAT,
                      broadcastSource, grid->rowComm);
            *localC += tempA + *localB;
        }
        MPI_Sendrecv_replace(localB, 1, MPI_FLOAT,
                             dest, 0, source, 0, grid->columnComm, &status);
    } /* for */

// printf("Result of %d p, %f \n",grid->myRank,*localC);


} /* Fox */

int main(int argc, char **argv)
{
    int p;
    int my_rank;
    GridInfo grid;
    float localA;
    float localB;
    float localC;
    int matrixOrder;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    initialiseGrid(&grid);
    matrixOrder = grid.gridOrder;

    MPI_Bcast(&matrixOrder, 1, MPI_INT, 0, MPI_COMM_WORLD);

    readInputMatrix("Enter A", &localA, &grid, matrixOrder);

    readInputMatrix("Enter B", &localB, &grid, matrixOrder);



 printf("Entry Process P:%d , A:%f , B:%f \n",grid.myRank,localA,localB);

    Fox(matrixOrder, &grid, &localA, &localB, &localC);

    MPI_Finalize();
}
