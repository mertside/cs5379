// CS5379: Parallel Processing - Assignment 2
// 
// by Tyler JOHNSON and Mert SIDE
// on 202109XX
// 

#include <stdio.h>
#include <mpi.h>

void barrier(int pid, int np);

int main(int argc, char** argv) 
{
  int pid, np;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);


  int total = 0;
  for (int i = 0; i < pid * 1000000000; i++) {
    total += pid;
  }

  printf("%d is before barrier\n", pid);

  barrier(pid, np);

  printf("%d is after barrier\n", pid);

  printf("%d\n", total);

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

/*
0 is before barrier
0 is waiting on 2
0 is waiting on 1
2 is before barrier
2 is sending to 0
2 is waiting on 0
1 is before barrier
  Here we have an issue. 1 does not complete waiting on 3 before sending to 0 that it is ready.
1 is waiting on 3
1 is sending to 0
1 is waiting on 0
1 is waiting on 3
1 is after barrier
0 is waiting on 2
0 is waiting on 1
0 is after barrier
3 is before barrier
3 is sending to 1
3 is waiting on 1
3 is after barrier
2 is after barrier
*/