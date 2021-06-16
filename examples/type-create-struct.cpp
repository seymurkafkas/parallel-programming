#include <iostream>
#include <mpi.h>

int MPI_Type_create_struct(
    int count,
    int array_of_blocklengths[],
    MPI_Aint array_of_displacements[],
    MPI_Datatype array_of_types[],
    MPI_Datatype *newtype);

int MPI_Get_address(const void *inputLocationPointer, MPI_Aint *outAddress);

int MPI_Type_commit(MPI_Datatype *datatype);

int main()
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Get the number of processes
    int processCount;
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype myCustomType;
    int arrayToBeSent[100];
    int arrayOfBlockLengths = {1, 1, 1};
    MPI_Aint arrayOfDisplacements = {0, 8, 16};
    MPI_Datatype arrayOfTypes = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};

    MPI_Type_create_struct(
        3,
        arrayOfBlockLengths,
        arrayOfDisplacements,
        arrayOfTypes,
        myCustomType);

    MPI_Type_commit(&myCustomType);

    // Finalize the MPI environment.
    MPI_Finalize();
}
