
/*  Gauss Parallel using PVM 3.4 
    - uses sibling() to determine the nb of spawned tasks (xpvm and pvm> ok)
    - uses group for token ring communication
    
    Stela CARNEIRO ESPINDOLA et Cecile POV
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include "pvm3.h"

#define GRPNAME "tokenring"



void matrix_load ( char nom[], double *a_p, int N, int NPROC, int me, int tids[]) 
{
	FILE *f;
	int i,j, l=0;
	int p;
	int msgtag = 4;

	// Initialize the data matrix
	double* data = (double*) malloc(N*sizeof(double));

	// Assign rows to each processor
	if(me == 0){
		if ((f = fopen (nom, "r")) == NULL) { 
				perror ("matrix_load : fopen "); 
		} 
	}
		

	for (i=0; i<N; i++) 
	{
		p = i%NPROC; // num du processeur a qui on doit envoyer la ligne i
		if(me == 0)
		{
			for (j=0; j<N; j++) 
			{
				fscanf (f, "%lf", (data+j));  
			}	

			if (p == 0) 
			{
				memcpy((a_p+l*N), data, N*sizeof(double)); 
				l++;
			}
			else
			{

				pvm_initsend( PvmDataDefault ); // met a 0 le buffer d'envoi
				pvm_pkdouble(data,N, 1 ); // met dans le paquet
				pvm_send(tids[p],msgtag); //envoi a p

			}
		}
		else
		{
			if(me == p)
			{
				pvm_recv(tids[0], msgtag); // recoir du processeur 0 
				pvm_upkdouble( a_p+l*N, N, 1 ); // recoir la ligne
				l++;
			}
		}
	}

	if(me == 0){
	  fclose(f);
  	}
}


void matrix_save_simple ( char nom[], double *tab, int N ) {
  FILE *f;
  int i,j;

  if ((f = fopen (nom, "w")) == NULL) { perror ("matrix_save : fopen "); } 
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      fprintf (f, "%8.2f ", *(tab+i*N+j) );
    }
    fprintf (f, "\n");
  }
  fclose (f);
}



void matrix_save ( char nom[], double *a_p, int N, int NPROC, int me, int tids[])  {
  FILE *f;
  int i,j, l = 0;// l = ligne deja lu
  int msgtag = 4;
  int p;
  // Initialize the data matrix
	double* data = (double*) malloc(N*sizeof(double));

  if(me == 0) // si je suis 0 j'ecris
  {

  	if ((f = fopen (nom, "w")) == NULL) { perror ("matrix_save : fopen "); }
  	
  } 
  

  	for (i=0; i<N; i++) 
	{
		p = i%NPROC; // numéro du processeur a qui on doit envoyer la ligne i
		if(me == 0)
		{
			//si c'est ma ligne, j'ecris dans le fichier
			if (p != 0)
			{
				pvm_recv(tids[p], msgtag); 
				pvm_upkdouble(data, N, 1 );
			}
			else
			{
				memcpy(data,(a_p+l*N),N*sizeof(double));
				l++;
			}			
			
			for (j=0; j<N; j++) 
			{
				fprintf (f, "%8.2f ", *(data+j) );

			}	
			fprintf (f, "\n");
			
		
		}
		else
		{
			if (me == p)
			{
			
				memcpy(data,(a_p+l*N),N*sizeof(double));
				l++;
				pvm_initsend( PvmDataDefault ); 
				pvm_pkdouble(data,N, 1 ); 
				pvm_send(tids[0],msgtag); //send data to processor 0
			}
		}
	}
	
	
	if(me == 0)
  	{	
  		fclose (f);
  	}

}



/**
* Calcule l'elimination de Gauss d'une matrice
* a_p : tableau local
* N : taille de la matrice d'entree
* NPROC : nombre de processeurs
* me : numero du processeur qui execute le programme
* tids[] tableau contenant les tids
*/
void gauss ( double * a_p, int N, int NPROC, int me , int tids[])
{
        int i,j,k, pk, pi;
        double pivot;
        double a[N]; // ligne pivot
        double akj, akk;
        int msgtag = 5;

        for ( k=0; k<N-1; k++ )
        {

	        pk = k%NPROC; // numero du processeur qui a la ligne pivot k

	        // Recuperer akk
	        if (me == pk) // si je suis le processeur qui a le pivot
	        {
		        memcpy(&a[k],(a_p+k+(k/NPROC)*N),(N-k)*sizeof(double)); // copie la ligne pivot dans le buffer de donnees k/___ : la bonne ligne-pivot dans le processeur pk
		        pvm_initsend( PvmDataDefault ); // met a 0 le buffer d'envoi
		        pvm_pkdouble(&a[k],(N-k), 1 ); // copie les (N-k) dernieres colonnes a partir de la colonne d'indice k
		        pvm_bcast(GRPNAME,msgtag);  // broadcast : echange total
	        }
	        else // si je ne l'ai pas
	        {
		        pvm_recv(tids[pk], msgtag);
		        pvm_upkdouble( &a[k], N-k, 1 );
	        }


	        for (i=k+1; i<N; i++) // on itere sur les lignes i en dessous de la ligne pivot
	        {
		         pi = i%NPROC;
		         if (me == pi) // si je suis le processeur qui a la ligne i
	           	 {

	           	 	akk = a[k];
	           	 	pivot =  *(a_p+k+(i/NPROC)*N)/akk; // la bonne ligne que l'on traite dans le processeur pi (qui n'a pas la ligne pivot)

	           	 	for (j=k; j<N; j++) // pour chaque colonne de la ligne i
		          	{
		          		akj = a[j];
				        *(a_p+j+(i/NPROC)*N) = *(a_p+j+(i/NPROC)*N) - (pivot*akj);
			        }
	           	 }
	        }
        }
}




/* Simple example passes a token around a ring */
void dowork( int me, int tids[], int nproc, char* file_name, int N) 
{
     int token;
     int src, dest;
     int count  = 1;
     int stride = 1;
     int msgtag = 4;

	 double * a_p = (double *) malloc((N/nproc)*N*sizeof(double)); //Initialize matrix


     matrix_load(file_name, a_p, N, nproc, me, tids);
     gauss(a_p, N, nproc, me, tids);
     matrix_save("output_cecile.txt", a_p, N, nproc, me, tids);
}






void main(int argc, char ** argv) 
{
    int NPROC = 8; //= atoi(argv[3]);		/* default nb of proc */
    int mytid;                  /* my task id */
    int *tids;                  /* array of task id */
    int me;                     /* my process number */
    int i;
	int N = atoi(argv[1]);
	char* file_name = argv[2];
	
	
    /* enroll in pvm */
    mytid = pvm_mytid();
	
    /* determine the size of my sibling list */
    NPROC = pvm_siblings(&tids); 
    printf(" NPROC: %d\n",NPROC);
    /* WARNING: tids are in order of spawning, which is different from
       the task index JOINING the group */

    me = pvm_joingroup( GRPNAME ); /* me: task index in the group */
	printf("me: %d \n",me);
    pvm_barrier( GRPNAME, NPROC );
    pvm_freezegroup ( GRPNAME, NPROC );
    for ( i = 0; i < NPROC; i++) {
    	tids[i] = pvm_gettid ( GRPNAME, i); 
    }


/*--------------------------------------------------------------------------*/
/*           all the tasks are equivalent at that point                     */

     dowork( me, tids, NPROC, file_name, N );
	 
     pvm_lvgroup( GRPNAME );
     pvm_exit();
     
}




