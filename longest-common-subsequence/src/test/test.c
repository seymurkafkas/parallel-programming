#include <stdbool.h>
#include <stdio.h>

bool shouldProcessIdleAtTurn(int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    if (diagonalTurnNumber <= processRank + (diagonalPassCount + 1) / 2 && diagonalTurnNumber >= processRank)
    {
        return false;
    }
    return true;
}

int getLocalIndexForProcess(int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    return diagonalTurnNumber - processRank;
}

int testSendIndex(int *localSubMatrix, int processRank, int diagonalTurnNumber, int diagonalPassCount)
{
    return getLocalIndexForProcess(processRank, diagonalTurnNumber, diagonalPassCount) - 2;
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

int main()
{
    int stringLength = 5;
    int processCount = stringLength + 1;
    int totalTurn = stringLength * 2 + 1;

    for (int i = 0; i < processCount; i++)
    {
        printf("Process %d \n", i);
        for (int turn = 0; turn < totalTurn; turn++)
        {

            if (!shouldProcessIdleAtTurn(i, turn, totalTurn))
            {
                printf(" %d Not Idle", turn);
            }
            else
            {
                printf(" %d Idle", turn);
            }

            if (shouldReceiveEntries(i, turn, totalTurn))
            {
                printf(" %d Receive ", turn);
            }
            if (shouldSendEntries(i, turn, totalTurn))
            {
                printf(" %d Send from Index: %d", turn, testSendIndex(NULL, i, turn, totalTurn));
            }
            printf("\n");
        }
    }
}