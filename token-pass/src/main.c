#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void passToken(int rank, int processCount, int *token)
{

    if (rank == 0)
    {
        srand(time(NULL));
        *token = rand();
        printf("Token sent by the master process: %d \n", *token);

        MPI_Send(token, 1, MPI_INT, (rank + 1) % processCount, 0, MPI_COMM_WORLD);
        MPI_Recv(token, 1, MPI_INT, processCount > 1 ? processCount - 1 : 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Token received by the master process: %d \n", *token);
    }
    else
    {

        MPI_Recv(token, 1, MPI_INT, (rank - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token\n", rank);
        fflush(stdout);
        MPI_Send(token, 1, MPI_INT, (rank + 1) % processCount, 0, MPI_COMM_WORLD);
        printf("Process %d sent token\n", rank);
    }
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
    passToken(rank, processCount, &token);

    // Finalize the MPI environment.
    MPI_Finalize();
}
