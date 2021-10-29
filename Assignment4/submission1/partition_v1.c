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
#include <math.h>

int** partition(int** d, int n, int p);
int min(int x, int y);
void printD(int** d, int n);

// ===================================MAIN====================================
int main(int argc, char** argv)
{
  MPI_Init( &argc, &argv );

  // get mpi stuff
  int pid, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  int n = (int)sqrt((double)np);

  if ((n * n) != np)
  {
    return 1;
  }

  // define d0 and d
  int **d0 = (int**)malloc(sizeof(int*) * n);
  int **d;
  for (int i = 0; i < n; i++)
  {
    d0[i] = (int*)malloc(sizeof(int) * n);
  }

  // rand d
  srand(time(NULL));

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (i == j)
        d0[i][j] = 0;
      else
        d0[i][j] = (rand() % 100) + 1;
    }
  }

  // partition
  
  if(pid == 0)
    printD(d0, n);
  d = partition(d0, n, np); 

  if(pid == 0)
  {
    printf("------\n");
    printD(d, n);
  }
  
  // clean
  /*
  for (int i = 0; i < n; i++)
  {
    free(d[i]);
    free(d0[i]);
  }
  free(d);
  free(d0);
  */

  MPI_Finalize();
  return 0;
}

// =============================PARTITION PARALLEL=============================
int** partition(int** d0, int n, int p)
{
  int i, j, k, pid;
  MPI_Status status;

  // alloc d0
  int **d = (int**) malloc(sizeof(int*) * n);
  for (int i = 0; i < n; i++)
  {
    d[i] = (int*)malloc(sizeof(int) * n);
  }

  // get MPI information
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);

  // get position from pid
  i = pid / n;
  j = pid % n;

  for(k = 0; k < n; k++) 
  {
    // Calculate local 
    d[i][j] = min(d0[i][j], d0[i][k] + d0[k][j]);
    // Rebuild d0
    if(pid == 0)
    {
      // Gather all data
      for(int row = 0; row < n; row++)
      {
        for (int col = 0; col < n; col++)
        {
          if (!(row == 0 && col == 0))
          {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, (row * n) + col, 0, MPI_COMM_WORLD, &status);
            d[row][col] = temp;
          }
        }
      }

      for(int row = 0; row < n; row++)
      {
        for(int col = 0; col < n; col++)
        {
          d0[row][col] = d[row][col];
        }
      }

      //printD(d)

      // Send data
      for(int row = 0; row < n; row++) // send row
      {
        for(int dest = 1; dest < p; dest++) // to dest pid
        {
          MPI_Send(d0[row], n, MPI_INT, dest, row, MPI_COMM_WORLD);
        }
      }
    }
    else // ! pid 0
    {
      // Send local data
      MPI_Send(&d[i][j], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

      // Recv data from 0
      for(int row = 0; row < n; row++)
      {
        MPI_Recv(d0[row], n, MPI_INT, 0, row, MPI_COMM_WORLD, &status);
      }
    }
  } // END for k

  return d;
}

// ==================================min======================================
int min(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// ================================printD===================================
void printD(int** d, int n)
{
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      printf("%2d, ", d[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}
// ===================================END=====================================
