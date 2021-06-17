/*----------------------------------------------------
 * File:    answer.c
 * Author: Seymur Kafkas
 *-----------------------------------------------------*/
#include <iostream>
#include <mpi.h>

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

    // Finalize the MPI environment.
    MPI_Finalize();
}