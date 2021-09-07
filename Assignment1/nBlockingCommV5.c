// CS5379: Parallel Processing - Assignment 1
// 
// by Tyler JOHNSON and Mert SIDE
// on 20210907
// 
// Description: Move generation of i=0->50 to PID 1 in order to allow parallel
// generation of the data. This also removed the sending and recieved for the
// first 50 rows of the data. Before the recieving was blocking on PID 1.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define generate_data(i,j)  (i)+(j)*(j)

int main(int argc, char **argv)
{
  int i, j, pid, np, mtag, count;
  double t0, t1;
  int data[100][100], row_sum[100];
  MPI_Status status; /* struct MPI_Status: MPI_SOURCE, MPI_TAG, MPI_ERROR */
  MPI_Request req_s, req_r;

  MPI_Init(&argc, &argv);              /* Set up */
  MPI_Comm_rank(MPI_COMM_WORLD, &pid); /* Local process index */
  MPI_Comm_size(MPI_COMM_WORLD, &np);  /* Totak processes */
  
  char name[80];
  int length;
  MPI_Get_processor_name(name, &length);

  printf("\nHello, MPI! Rank: %d \nTotal: %d \nMachine: %s\n", pid, np, name);

  if(pid == 0) {			/*** pid == 0 ***/
    // generate data[] from 0 to 49
    for(i = 0; i < 50; i++)
      for(j = 0; j < 100; j++)
        data[i][j] = i + j * j;
    
    // send data[] from 0 to 49
    mtag = 1;
    MPI_Isend(data, 5000, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);
    
    // generate data[] from 50 to 99
    for(i = 50; i < 100; i++)
      for(j = 0; j < 100; j++)
        data[i][j] = generate_data(i,j);

    /*** receive computed row_sums from  pid 1 ***/
    mtag = 3 ;
    //for(mtag = 2; mtag < mtag+50; mtag++) 
    MPI_Recv(row_sum, 25, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);

    // sum data[] from 50 to 99
    for(i = 50; i < 100; i++) {
      row_sum[i] = 0;
      for(j=0; j<100; j++)
         row_sum[i] += data[i][j];
    }

    MPI_Wait(&req_s, &status);

    /*** receive computed row_sums from  pid 1 ***/
    //mtag = 3 ;
    //for(mtag = 2; mtag < mtag+50; mtag++) 
    //MPI_Recv(row_sum, 25, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);

    mtag = 2 ;
    MPI_Recv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);
    
    printf("\n");
    // now we have all the sum from 0 to 99
    for(i = 0; i < 100; i++) { 
      printf(" %d  ", row_sum[i]);
      if(i % 5 == 4) printf("\n");
    }
    printf("\n");

  } else {				/*** pid == 1 ***/
    // receive data[] from 0 to 49
    mtag = 1 ;
    MPI_Recv(data, 5000, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);

    mtag = 3;
    // sum data[] from 0 to 49
    for(i = 0; i < 25; i++) {
      row_sum[i] = 0;
      for(j = 0; j < 100; j++)
         row_sum[i] += data[i][j];
      //MPI_Isend(row_sum, 1, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);
      //mtag++;
    }
    MPI_Isend(row_sum, 25, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);

    // sum data[] from 0 to 49
    for(i = 25; i < 50; i++) {
      row_sum[i] = 0;
      for(j = 0; j < 100; j++)
         row_sum[i] += data[i][j];
      //MPI_Isend(row_sum, 1, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);
      //mtag++;
    }
    
    MPI_Wait(&req_r, &status);

    /*** Send computed row_sums to pid 0 ***/
    mtag = 2;
    MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD);
         
  }


  MPI_Finalize();			/* Tear down */

  return 1;
} /****************** End of function main() ********************/
