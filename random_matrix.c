#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

// Compiler : gcc -Wall -o rd_matrix random_matrix.c


int mkdir(const char *pathname, mode_t mode);

void save_file(int n, int*matrix)
{
        //create directory
        /*
        struct stat st = {0};

        if (stat("/matrices", &st) == -1) 
        {
            mkdir("/matrices", 0700);
        }
        */
        
        char str_n[10];
        sprintf(str_n, "%d", n);
        
        char filename[100] = "matrix_";
        strcat (filename, str_n);
        strcat (filename,"x");
        strcat (filename, str_n);
        strcat (filename, ".txt");
        FILE *f = fopen (filename, "w");
        int i,j;
        

        if (f == NULL) 
        { 
                perror ("matrix_save : fopen "); 
        }
        else
        {
                for(i=0; i<n; i++) 
                {
                        for(j=0; j<n; j++)
                        {
                                fprintf(f,"%d ", *(matrix+j+i*n)); // n car nb colonnes
                        }
                        fprintf(f, "\n");
                }
        }
}
        
        


int random_matrix(int n)
{
        int* memoireAllouee = NULL; 
        
        memoireAllouee = (int *)malloc(sizeof(int)*n*n);
        if (memoireAllouee == NULL) // On verifie si la memoire a ete allouee
        {

                exit(0); // Erreur : on arrete tout !
        }
        
        //int rd_matrix[n][n];
        int i, j;

        srand(time(NULL));
        for(i = 0; i<n; i++)
        {
                for(j = 0; j<n; j++)
                {
                        if (i != j) { *(memoireAllouee+j+i*n) = rand()%10;   } // nombre de 0 a 9
                        else        { *(memoireAllouee+j+i*n) = (rand()%9)+1;} // nombre de 1 a 9
                        
                }
        }
        save_file(n,memoireAllouee);
        free(memoireAllouee);

        return 0;
}




int main(int argc, char *argv[] )
{
        int i;
        for (i = 1; i<513; i = i*2)
        {
                printf("%d \n",i);
                random_matrix(i);
                
        }
}
