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
void readInputMatrix()
{
}

LocalMatrix *allocateLocalMatrix(int localOrder)
{
    LocalMatrix *temp = (LocalMatrix *)malloc(sizeof(LocalMatrix));
    return temp;
} /* Local_matrix_allocate */

void freeLocalMatrix(LocalMatrix *pointerToMatrix /* in/out */)
{
    free(pointerToMatrix);
} /* Free_local_matrix */

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

void multiplyMatrices(Matrix *first, Matrix *second)
{
}

void Fox(
    int n /* in  */,
    GridInfo *grid /* in  */,
    LocalMatrix *localA /* in  */,
    LocalMatrix *localB /* in  */,
    LocalMatrix *localC /* out */)
{

    LocalMatrix *tempA; /* Storage for the sub-    */
                            /* matrix of A used during */
                            /* the current stage       */
    int stage;
    int bcast_root;
    int n_bar; /* n/sqrt(p)               */
    int source;
    int dest;
    MPI_Status status;

    n_bar = n / grid->gridOrder;
    Set_to_zero(local_C);

    /* Calculate addresses for circular shift of B */
    source = (grid->my_row + 1) % grid->q;
    dest = (grid->my_row + grid->q - 1) % grid->q;

    /* Set aside storage for the broadcast block of A */
    temp_A = Local_matrix_allocate(n_bar);

    for (stage = 0; stage < grid->q; stage++)
    {
        bcast_root = (grid->my_row + stage) % grid->q;
        if (bcast_root == grid->my_col)
        {
            MPI_Bcast(local_A, 1, local_matrix_mpi_t,
                      bcast_root, grid->row_comm);
            Local_matrix_multiply(local_A, local_B,
                                  local_C);
        }
        else
        {
            MPI_Bcast(temp_A, 1, local_matrix_mpi_t,
                      bcast_root, grid->row_comm);
            Local_matrix_multiply(temp_A, local_B,
                                  local_C);
        }
        MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t,
                             dest, 0, source, 0, grid->col_comm, &status);
    } /* for */

} /* Fox */

int main(int argc, char **argv)
{
    int p;
    int my_rank;
    GridInfo grid;
    LocalMatrix *localA;
    LocalMatrix *localB;
    LocalMatrix *localC;
    int matrixOrder;
    int n_bar;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    initialiseGrid(&grid);
    if (my_rank == 0)
    {
        printf("What's the order of the matrices?\n");
        scanf("%d", &matrixOrder);
    }

    MPI_Bcast(&matrixOrder, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n_bar = matrixOrder / grid.gridOrder;

    localA = Local_matrix_allocate(n_bar);
    localA->order = n_bar;
    Read_matrix("Enter A", localA, &grid, n);
    Print_matrix("We read A =", localA, &grid, n);

    localB = Local_matrix_allocate(n_bar);
    localB->order = n_bar;
    Read_matrix("Enter B", localB, &grid, n);
    Print_matrix("We read B =", local_B, &grid, n);

    Build_matrix_type(localA);
    temp_mat = Local_matrix_allocate(n_bar);

    localC = Local_matrix_allocate(n_bar);
    localC->order = n_bar;
    Fox(n, &grid, localA, localB, localC);

    Print_matrix("The product is", localC, &grid, n);

    Free_local_matrix(&localA);
    Free_local_matrix(&localB);
    Free_local_matrix(&localC);

    MPI_Finalize();
}
