// CS5379: Parallel Processing - Assignment 2
// 
// by Tyler JOHNSON and Mert SIDE
// on 202109XX
// 

#include <stdio.h>
#include <mpi.h>
#include <math.h>

void barrier(int pid, int np);
void treeBarrier(int pid, int np);

int main(int argc, char** argv) 
{
  int pid, np;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  /*
  int total = 0;
  for (int i = 0; i < pid * 1000000000; i++) {
    total += pid;
  }
  */

  printf("%d is before barrier\n", pid);

  treeBarrier(pid, np);

  printf("%d is after barrier\n", pid);

  // printf("%d\n", total);

  MPI_Finalize();
  return 0;
}

void barrier(int pid, int np) 
{
  // mtag will be the child of each communcation
  // this should be not required since we have source and dest defined.

  char message = 0;
  MPI_Status status;
  MPI_Request rReq, lReq, pReq;

  int rightChild = (2 * pid) + 2;
  int leftChild = (2 * pid) + 1;
  int parent = (pid - 1) / 2;

  // must wait for children to have entered the barrier before telling the parent we (and our children) have entered
  if (rightChild < np)
  {
    printf("%d is waiting on %d\n", pid, rightChild);
    MPI_Irecv(&message, 1, MPI_CHAR, rightChild, rightChild, MPI_COMM_WORLD, &rReq);
    MPI_Wait(&rReq, &status);
  }
  if (leftChild < np)
  {
    printf("%d is waiting on %d\n", pid, leftChild);
    MPI_Irecv(&message, 1, MPI_CHAR, leftChild, leftChild, MPI_COMM_WORLD, &lReq);

    MPI_Wait(&lReq, &status);
  }

  /*
  if (rightChild < np)
    MPI_Wait(&rReq, &status);
  if (leftChild < np)
    MPI_Wait(&lReq, &status);
  */

  // if not root we must say we have entered the barrier and then wait to be told we can leave
  if (pid != 0) {
    printf("%d is sending to %d\n", pid, parent);
    MPI_Send(&message, 1, MPI_CHAR, parent, pid, MPI_COMM_WORLD);

    //MPI_Wait(&pReq, &status);

    printf("%d is waiting on %d\n", pid, parent);
    MPI_Irecv(&message, 1, MPI_CHAR, parent, pid, MPI_COMM_WORLD, &pReq);
   
    MPI_Wait(&pReq, &status);
  }
  
  // must tell children they can leave the barrier
  if (rightChild < np)
  {
    printf("%d is sending to %d\n", pid, rightChild);
    MPI_Send(&message, 1, MPI_CHAR, rightChild, rightChild, MPI_COMM_WORLD);
  }
  if (leftChild < np)
  {
    printf("%d is sending to %d\n", pid, leftChild);
    MPI_Send(&message, 1, MPI_CHAR, leftChild, leftChild, MPI_COMM_WORLD);
  }
  //MPI_Wait(&rReq, &status);
  //MPI_Wait(&lReq, &status);
}

// ===========================================================================

void treeBarrier(int pid, int np)
{ 
  double logValue = log(np) / log(2);
 
  int logValue_int = logValue;
  // printf("Log Value is %d",logValue_int);
 
  int k = logValue_int;
  
  // TRAPPING PHASE:
  int i = 0;
  int j = 0; 
  int iteration = 0;
  for(i = k-1; i >= 0; i--) {
    iteration = 2 - i;
    //printf("\n iteration %d",2-i);
    
    for(j = pow(2, i); j < pow(2, i + 1); j++) {
      if(pid == j){
        // printf("\n sending msg from %d to %d in iter %d", j, j - (int) pow(2, i), iteration);
        MPI_Send(&iteration, 1, MPI_INT, j-(int)pow(2,i), iteration, MPI_COMM_WORLD);
      }
       
      if(pid == j - (int) pow(2, i)){
        MPI_Status status;
        int data;
        MPI_Recv(&data, 1, MPI_INT, j, iteration, MPI_COMM_WORLD, &status);
        // printf("\n receiving msg from %d to %d and msg is %d", j, j - (int) pow(2, i), data);
      }
    } // end of inner loop
  } // end of outer for

  // RELEASE PHASE:
  i = 0;
  j = 0;
  iteration = 0;
  for(i = 0; i <= k-1; i++) {
    iteration=i;
    // printf(" iteration %d\n", i);
    
    for(j = 0; j <= pow(2,i)-1; j++) {
      //printf("\nValue of i,j is %d , %d",i,j);
      
      if(pid == j) {
        // printf("\n sending msg from %d to %d in iter %d", j, j + (int) pow(2, i), iteration);
        MPI_Send(&iteration, 1, MPI_INT, j + (int) pow(2, i), iteration, MPI_COMM_WORLD);
      }
      
      if(pid == j+(int)pow(2,i)) {
        MPI_Status status;
        int data;
        MPI_Recv(&data,1,MPI_INT,j,iteration,MPI_COMM_WORLD,&status);
        printf("\n receiving message from %d to %d and message is %d\n\n",j,j+(int)pow(2,i),data);
      }
    }//end of inner loop
  }//end of outer for

}
