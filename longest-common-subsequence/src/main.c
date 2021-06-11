/* The Longest Common Subsequence
        Author: Seymur Kafkas
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct
{
    int elementCount;
    int *bufferAddress;
} MessageBuffer;

typedef struct
{
    MessageBuffer buffer;
    MPI_Datatype typeOfData;
    int sourceProcessRank;
    int tag;
    MPI_Comm communicator;
    MPI_Status *statusOutputPtr;
} ReceiveParameters;

typedef struct
{
    MessageBuffer buffer;
    MPI_Datatype typeOfData;
    int destinationProcessRank;
    int tag;
    MPI_Comm communicator;
} SendParameters;

typedef struct
{
    int rank;
    int *localSubMatrix;
    const char *firstString;
    char characterInString;

} LocalData;

int getLocalIndexForProcess(int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    return diagonalTurnNumber - processRank;
}

bool shouldReceiveEntries(int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    if (processRank == 0)
    {
        return false;
    }

    if (diagonalTurnNumber - processRank < (diagonalPassCount + 1) / 2 && diagonalTurnNumber - processRank >= 1)
    {
        return true;
    }
    return false;
}

void receiveMessageFromProcess(ReceiveParameters *params)
{
    printf(" Receiving from %d\n",params->sourceProcessRank);
    MPI_Recv(params->buffer.bufferAddress, params->buffer.elementCount, params->typeOfData, params->sourceProcessRank, params->tag, params->communicator, params->statusOutputPtr);
}

void sendMessageToProcess(SendParameters *params)
{
    MPI_Send(params->buffer.bufferAddress, params->buffer.elementCount, params->typeOfData, params->destinationProcessRank, params->tag, params->communicator);
}

ReceiveParameters getReceiveParameters(int *receiveBuffer, int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    ReceiveParameters result;
    result.sourceProcessRank = processRank - 1;
    result.statusOutputPtr = MPI_STATUS_IGNORE;
    result.tag = 0;
    result.typeOfData = MPI_INT; 
    result.buffer.bufferAddress = receiveBuffer;
    result.buffer.elementCount = 2;
    result.communicator = MPI_COMM_WORLD;
    return result;
}

SendParameters getSendParameters(int *localSubMatrix, int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    SendParameters result;
    result.destinationProcessRank = processRank + 1;
    result.tag = 0;
    result.typeOfData = MPI_INT; 
    result.buffer.bufferAddress = localSubMatrix + getLocalIndexForProcess(processRank, diagonalTurnNumber, diagonalPassCount) - 2;
    result.buffer.elementCount = 2;
    result.communicator = MPI_COMM_WORLD;
    return result;
}

void sendEntriesToNextProcess(int *localSubMatrix, int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    SendParameters parameters = getSendParameters(localSubMatrix, processRank, diagonalTurnNumber, diagonalPassCount);
    sendMessageToProcess(&parameters);
}

void receiveEntriesFromPrevProcess(int *outReceiveBuffer, int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    ReceiveParameters parameters = getReceiveParameters(outReceiveBuffer, processRank, diagonalTurnNumber, diagonalPassCount);
    receiveMessageFromProcess(&parameters);
}

bool shouldSendEntries(int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    if (processRank == (diagonalPassCount + 1) / 2 - 1)
    {
        return false;
    }
    if (diagonalTurnNumber - processRank <= (diagonalPassCount + 1) / 2 && diagonalTurnNumber - processRank >= 2)
    {
        return true;
    }
    return false;
}

int *allocateMemoryForSubMatrix(int lengthOfString)
{
    int *pointerToAllocatedSpace = (int *)malloc(lengthOfString * sizeof(int));
    return pointerToAllocatedSpace;
}

int max(int first, int second)
{
    return first > second ? first : second;
}

void computeLocalValueIfApplicable(int *receiveBuffer, int processRank, int *localSubMatrix, int index, const char *firstString, char comparedChar)
{
    if (processRank == 0 || index == 0)
    {
        localSubMatrix[index] = 0;
    }
    else
    {
        if (firstString[index] == comparedChar)
        {
            int receivedDiagonal = receiveBuffer[0]; //getMessage();
        }
        else
        {
            int upperSubResult = localSubMatrix[index - 1];
            int leftSubResult = receiveBuffer[1]; //getMessage();
            localSubMatrix[index] = max(leftSubResult, upperSubResult);
        }
    }
}

bool shouldProcessIdleAtTurn(int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    if (diagonalTurnNumber <= processRank + (diagonalPassCount + 1) / 2 && diagonalTurnNumber >= processRank)
    {
        return false;
    }
    return true;
}

void fillLocalSubMatrix(int processRank, int diagonalPassCount, int *localSubMatrix, const char *first, const char *second)
{
    for (int diagonalTurn = 0; diagonalTurn < diagonalPassCount; diagonalTurn++)
    {
        int receiveBuffer[2];
        if (!shouldProcessIdleAtTurn(processRank, diagonalTurn, diagonalPassCount))
        {
            int localIndex = getLocalIndexForProcess(processRank, diagonalTurn, diagonalPassCount);
            if (shouldSendEntries(processRank, diagonalTurn, diagonalPassCount))
            {
                sendEntriesToNextProcess(localSubMatrix, processRank, diagonalTurn, diagonalPassCount);
            }
            if (shouldReceiveEntries(processRank, diagonalTurn, diagonalPassCount))
            {
                receiveEntriesFromPrevProcess(receiveBuffer, processRank, diagonalTurn, diagonalPassCount);
            }
            computeLocalValueIfApplicable(receiveBuffer, processRank, localSubMatrix, localIndex, first, second[processRank]);
        }
    }
}

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    const char *first = "hello";
    const char *second = "holla";
    int lengthOfString = 5;
    // Get the number of processes
    int processCount;
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *localSubMatrix = allocateMemoryForSubMatrix(lengthOfString);

    int diagonalPassCount = 2 * lengthOfString - 1;

    printf("%d \n",rank);
    fillLocalSubMatrix(rank, diagonalPassCount, localSubMatrix, first, second);

    // Finalize the MPI environment.
    MPI_Finalize();
    
}