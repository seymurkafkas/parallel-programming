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

    for (int i = 0; i < 4; i++)
    {

        printf("Process %d \n", i);
        for (int turn = 0; turn < 7; turn++)
        {

            if (!shouldProcessIdleAtTurn(i, turn, 7))
            {
                printf(" %d Not Idle", turn);
            }
            else
            {
                printf(" %d Idle", turn);
            }

            if (shouldReceiveEntries(i, turn, 7))
            {
                printf(" %d Receive ", turn);
            }
            if (shouldSendEntries(i, turn, 7))
            {
                printf(" %d Send ", turn);
            }
            printf("\n");
        }
    }
}