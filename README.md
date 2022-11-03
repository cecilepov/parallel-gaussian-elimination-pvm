# Parallel implementation of Gaussian elimination

The purpose of this work is to implement a parallel version of gaussian elimination using PVM library.

The gaussian elimination is an algorithm used to solve systems of linear equations in the form A.X = B, with: 
- A is a N*N matrix;
- X and B are 2 vectors of size N.

The original matrix is reduced to a upper triangular form.

## Report

The report covers the following points:
1. We reminds how does a  Gaussian elimination performs, then purpose a sequential version of the algorithm
2. We design a parallel algorithm by analyzing the sequential algorithm and determining which operations:
    - can be carried out simultaneously, so that their can be distributed through different processors
    - must be sequential
3. We implement our parallel algorithm using C and PVM library. The approach is validated using XPVM.
4. We evaluate the performance of our algorithm, by considering executing time, speedup and efficiency coefficient.
   We also run our algorithm in 2 different networks, and try to explain the performance difference by comparing their hardware architecture. 


## Compiling and running the code 

Please follow the intructions here https://netlib.org/pvm3/.
to install, configure and compile your PVM. 