## Compile

While in /src,

    mpicc main.c -o main.out


## Run
    mpiexec -n [lengthOfString +1] main.out

Warning: The entered string must have a length that is 1 lower than the number of processing units.

Example: 

    mpiexec -n 9 main.out

    Enter the first string (With length 8):
    HOLAXYZD
    Enter the second string (With length 8):
    HANKZYDM
    The result is 4