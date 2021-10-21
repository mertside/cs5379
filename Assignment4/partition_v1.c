// CS5379: Parallel Processing - Assignment 4
// 
// by Tyler JOHNSON and Mert SIDE
// on 20211019
//

/*****************************************************************************
 * Input:  D0, n x n matrix with 0 on diagonal,
 *             positive values other places
 * Output: D,  n x n matrix
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// ===================================MAIN====================================
int main(int argc, char** argv)
{
  MPI_Init( &argc, &argv );

  int pid;
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);

  // TODO: define data: D0, D
  int *d = (int*) malloc(sizeof(int));
  int *d0 = (int*) malloc(sizeof(int));

  partition(); 
  
  free(d);
  free(d0);

  MPI_Finalize();
  return 0;
}

// =============================DIJKSTRA PARALLEL=============================
void partition(/* TODO */)
{
  int i, j, k;

  // get MPI information
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  for(k = 0; k < n; k++) {
    for(i = 0; i < n; i++) {
      for(j = 0; j < n; j++) {
        D[i][j] = min(D0[i][j], D0[i][k] + D0[k][j]);
      } // END for j
    } // END for i

    for(i = 0; i < n; i++) {
      for(j = 0; j < n; j++) {
        D0[i][j] = D[i][j];
      } // END for j
    } // END for i

  } // END for k

}

// ==================================min======================================
int min(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// ===================================END=====================================
