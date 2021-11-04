// CS5379: Parallel Processing - Assignment 5
// by Tyler JOHNSON and Mert SIDE
// on 20211104

/* Assignmemt 5:
 *   Parallelize the force calculation for n particles so that
 *   the computation loads on all processes are balanced, and
 *   commuication cost has a complexity of O(n log_2 p) for
 *   n particles and p processors.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#define SGN(X)    ( ((X)<0.0) ? (-1.0) : (1.0) )
//#define C1    1.23456
//#define C2    6.54321
//#define N     123456

#define C1    1
#define C2    6
#define N     8

// ================================calc-force=================================
/*
 * Input: n, x[n]. Note that x[i] \not= x[j] for different i, j.
 * Output: f[n].
 */
void calc_force(int n, double *x, double *f)
{
  int i, j;
  double diff, tmp;
  for(i = 0; i < n; i++) 
    f[i] = 0.0;
  for(i = 1; i < n; i++) {
    for(j = 0; j < i; j++) {
      diff = x[i] - x[j];
      // tmp = 1.0 / diff;
      tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
      f[i] += tmp;
      f[j] -= tmp;
    }
  }
}

// ===========================parallel_calc_force================================
/*
 * Input: n, x[n]. Note that x[i] \not= x[j] for different i, j.
 * Output: f[n].
 */
void parallel_calc_force(int n, double *x, double *f)
{
  int pid, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  int i, j;
  double diff, tmp;
  for(i = 0; i < n; i++)
    f[i] = 0.0;
 
  /*
  for(k = pid * n/(2 * np); k < ((pid + 1) * n) / (2 * np); k++) {
    int bottom = 1; 
    for(i = k; i < n; i++) {
      for(j = 0; j < i; j++) {
        diff = x[i] - x[j];
        // tmp = 1.0 / diff;
        tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
        f[i] += tmp;
        f[j] -= tmp;
      }
    }
  }
  */

  for(i = pid * n/(2 * np); i < ((pid + 1) * n) / (2 * np); i++) {
    for(j = 0; j < i; j++) { // bottom
      diff = x[i] - x[j];
      // tmp = 1.0 / diff;
      tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
      f[i] += tmp;
      f[j] -= tmp;
    }
    i = n - i;
    for(j = 0; j < i; j++) { // top
      diff = x[i] - x[j];
      // tmp = 1.0 / diff;
      tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
      f[i] += tmp;
      f[j] -= tmp;
    }
  }

  // TODO: combine all f at pid 0
}


// ===================================MAIN====================================
int main(int argc, char** argv)
{
  MPI_Init( &argc, &argv );

  // get mpi stuff
  int pid, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  //int n = np * 32 ; // number of particles

  srand(time(NULL));

  double *x = (double*) malloc(sizeof(double) * N);
  double *f = (double*) malloc(sizeof(double) * N);

  for (int i = 0; i < N; i++)
  { 
     x[i] = ((rand()%100) + 1);
     printf("%lf\n", x[i]);
  }

  printf("\nCalculating\n\n");
  calc_force(N, x, f);

  for (int i = 0; i < N; i++)
  {
     f[i] = (rand() % 100) + 1;
     printf("%lf\n", f[i]);
  }

  MPI_Finalize();
  return 0;
}
// ===================================END=====================================
