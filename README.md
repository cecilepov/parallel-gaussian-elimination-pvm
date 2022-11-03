# Parallel gaussian elimination

The purpose of this work is to implement a parallel version of gaussian elimination using PVM library.

The gaussian elimination is an algorithm for solving systems of linear equations in the form A.X = B, with : 
- A is a N*N matrix;
- X and B are 2 vectors of size N.

The original matrix is reduced to a upper triangular form.

The report is organized as follows:
1. How does Gaussian elimination performs [Reminder]
   + equential version