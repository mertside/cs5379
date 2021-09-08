/*
Assignment 1

Tyler Johnson
Mert Side

Description: Move generation of i=0->50 to PID 1 in order to allow parallel
generation of the data. This also removed the sending and recieved for the
first 50 rows of the data. Before the recieving was blocking on PID 1.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define generate_data(i,j)  (i)+(j)*(j)

unsigned long long startTime;

int main( int argc, char **argv)
{
  int i, j, pid, np, mtag, count;
  double t0, t1;
  int data[100][100], row_sum[100] ;
  MPI_Status status;
  MPI_Request req_s, req_r;


  MPI_Init( &argc, &argv );
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if(pid == 0) {
    for(i=50; i<100; i++)
      for(j=0; j<100; j++)
        data[i][j] = generate_data(i,j) ;

    for(i=50; i<100; i++) {
      row_sum[i] = 0 ;
      for(j=0; j<100; j++)
         row_sum[i] += data[i][j] ;
    }
   /*** receive computed row_sums from  pid 1 ***/
    mtag = 2 ;
    MPI_Recv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status) ;
    for(i=0; i<100; i++) {
       printf(" %d  ", row_sum[i]) ;
       if(i % 5 == 4) printf("\n");
    }
  }
  else { /*** pid == 1 ***/
    for(i=0; i<50; i++)
      for(j=0; j<100; j++)
        data[i][j] = i+ j*j ;

    for(i=0; i<50; i++) {
      row_sum[i] = 0 ;
      for(j=0; j<100; j++)
         row_sum[i] += data[i][j] ;
    }


   /*** Send computed row_sums to pid 0 ***/
    mtag = 2 ;
    MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD) ;
  }

  MPI_Finalize();

  return 0;
} /****************** End of function main() ********************/
