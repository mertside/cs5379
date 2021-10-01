// CS5379: Parallel Processing - Assignment 2
// 
// by Tyler JOHNSON and Mert SIDE
// on 20210930
//
// Description of the solution:
// Implemented a tree barrier that consists of two phases:
//   1. trapping phase,
//   2. release phase.
// Solved in [2*log_2(n)]
//

#include <stdio.h>
#include <mpi.h>
#include <math.h>

void barrier(int pid, int np);
void treeBarrier(int pid, int np);

// ===================================MAIN====================================
int main(int argc, char** argv) 
{
  // printf("main starting... \n\n");
  int pid, np;

  MPI_Init(&argc, &argv);                /* Set up */
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);   /* Local process index */
  MPI_Comm_size(MPI_COMM_WORLD, &np);    /* Totak processes */

  printf("%d is before barrier\n", pid);

  // --- entering barrier ---
  treeBarrier(pid, np);

  printf("%d is after barrier\n", pid);

  MPI_Finalize();                        /* Tear down */
  // printf("\nmain ended. \n\n");
  return 0;
}

// ==================================BARRIER==================================
void treeBarrier(int pid, int np)
{ 
  double logValue = log(np) / log(2);
  int logValue_int = logValue;
  // printf("Log Value is %d\n", logValue_int);
  int k = logValue_int;
  
  // TRAPPING PHASE: ... ... ... ... ... ... ... ... ... ... ... ... ... ...
  int i = 0;
  int j = 0; 
  int iteration = 0;
  for(i = k-1; i >= 0; i--) {
    iteration = 2 - i;
    // printf(" iteration %d\n", 2-i);
    
    for(j = pow(2, i); j < pow(2, i + 1); j++) {
      if(pid == j) {
        // printf("\n sending msg from %d to %d in iter %d\n", 
        //        j, j - (int) pow(2, i), iteration);
        MPI_Send(&iteration, 1, MPI_INT, j - (int) pow(2, i), iteration, 
                 MPI_COMM_WORLD);
      }
       
      if(pid == j - (int) pow(2, i)) {
        MPI_Status status;
        int data;
        MPI_Recv(&data, 1, MPI_INT, j, iteration,
                 MPI_COMM_WORLD, &status);
        // printf("\n receiving msg from %d to %d and msg is %d\n", 
        //        j, j - (int) pow(2, i), data);
      
      }
    
    } // ... end of inner loop ...
  
  } // ... ... end of outer for ... ...

  // RELEASE PHASE: ... ... ... ... ... ... ... ... ... ... ... ... ... ...
  i = 0;
  j = 0;
  iteration = 0;
  for(i = 0; i <= k-1; i++) {
    iteration = i;
    // printf(" iteration %d\n", i);
    
    for(j = 0; j <= pow(2, i) - 1; j++) {
      // printf("Value of i, j is %d , %d\n", i, j);
      
      if(pid == j) {
        // printf("\n sending msg from %d to %d in iter %d\n", 
        //        j, j + (int) pow(2, i), iteration);
        MPI_Send(&iteration, 1, MPI_INT, j + (int) pow(2, i), iteration, 
                 MPI_COMM_WORLD);
      }
      
      if(pid == j + (int) pow(2, i)) {
        MPI_Status status;
        int data;
        MPI_Recv(&data,1,MPI_INT,j,iteration,MPI_COMM_WORLD,&status);
        printf("\n receiving message from %d to %d\n\n",
               j, j + (int) pow(2, i), data);
        // printf("\n receiving message from %d to %d and message is %d\n\n", 
        //        j, j + (int) pow(2, i), data);
      }

    } // ... end of inner loop ...
  
  } // ... ... end of outer for ... ...

}
// ===================================END=====================================
