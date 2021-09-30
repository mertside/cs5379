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

  printf("%d is before barrier\n", pid);

  barrier(pid, np);

  printf("%d is after barrier\n", pid);

  MPI_Finalize();
  return 0;
}

void barrier(int pid, int np) 
{
  int message = 0;
  int mtag = 0;
  MPI_Status status;
  MPI_Request rReq, lReq, pReq;

  int rightChild = (2 * pid) + 2;
  int leftChild = (2 * pid) + 1;
  int parent = (pid - 1) / 2;

  // if root
  if (pid == 0) {
    // wait to recv from children
    MPI_Recv(&message, 1, MPI_INT, rightChild, mtag, MPI_COMM_WORLD, &status);
    MPI_Recv(&message, 1, MPI_INT, leftChild, mtag, MPI_COMM_WORLD, &status);
  
  } else {
    // wait to recv from children
    if (rightChild < np)
      MPI_Recv(&message, 1, MPI_INT, rightChild, mtag, MPI_COMM_WORLD, &status);
    if (leftChild < np)
      MPI_Recv(&message, 1, MPI_INT, leftChild, mtag, MPI_COMM_WORLD, &status);

    // send to parent
    MPI_Send(&message, 1, MPI_INT, parent, mtag, MPI_COMM_WORLD);
    // MPI_Isend(&message, 1, MPI_INT, parent, mtag, MPI_COMM_WORLD, &pReq);

    // wait to recv from parent
    MPI_Recv(&message, 1, MPI_INT, parent, mtag, MPI_COMM_WORLD, &status);
   
   }
   
   // send to children
   if (rightChild < np)
     MPI_Send(&message, 1, MPI_INT, rightChild, mtag, MPI_COMM_WORLD);
     // MPI_Isend(&message, 1, MPI_INT, rightChild, mtag, MPI_COMM_WORLD, &rReq);
   if (leftChild < np)
     MPI_Send(&message, 1, MPI_INT, leftChild, mtag, MPI_COMM_WORLD);
     // MPI_Isend(&message, 1, MPI_INT, leftChild, mtag, MPI_COMM_WORLD, &lReq);

