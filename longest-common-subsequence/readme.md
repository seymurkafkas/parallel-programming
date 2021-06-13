To compile:

While in /src,

    mpicc main.c -o main.out


To run:

    mpiexec -n [lengthOfString +1] main.out


Warning: The entered string must have a length that is 1 lower than the number of processing units.