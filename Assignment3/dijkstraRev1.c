/*****************************************************************************
* Input:  n and edge[n][n], where n is the number of vertices of a graph
*         edge[i][j] is the length of the edge from vertex i to vertex j
* Output: distance[n], the distance from the SOURCE vertex to vertex i.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void dijkstra(int SOURCE, int n, int **edge, int *distance);
void dijkstraParallel(int SOURCE, int n, int **edge, int *distance);
void printDistance(int* distance, int n);
void printEdge(int** edge, int n);
int min(int x, int y);

int main(int argc, char** argv)
{
  MPI_Init( &argc, &argv );

  int source = 3;
  int n = 20;
  int **edge = (int**)malloc(sizeof(int*) * n);
  int *distance = (int*)malloc(sizeof(int) * n);
  
  // init edge and distance
  srand(time(NULL));

  for (int i = 0; i < n; i++)
  {
    distance[i] = 987654321;

    edge[i] = (int*)malloc(sizeof(int) * n);
    for (int j = 0; j < n; j++)
    {
      edge[i][j] = (rand() % 10) + 1;
    }
  }

  //printEdge(edge, n);

  // Get non-parallel distances
  /*
  printDistance(distance, n);
  dijkstra(source, n, edge, distance);
  printDistance(distance, n);
  */

  // Reset distances
  for (int i =0; i < n; i++)
  {
    distance[i] = 987654321;
  }

  // Get parallel distances
  printDistance(distance, n);
  dijkstraParallel(source, n, edge, distance);
  printDistance(distance, n);

  // Free memory
  free(distance);
  /*
  for (int i = 0; i < n; i++)
  {
    free(edge[i]);
  }
  free(edge);
  */

  MPI_Finalize();
  return 0;
}

void dijkstra(int SOURCE, int n, int **edge, int *distance)
{
  int i, j, count, tmp, least, leastPos, *found;

  // initialization...
  found = (int *) calloc(n, sizeof(int));
  for(i = 0; i < n; i++) 
  {
    found[i] = 0;
    distance[i] = edge[SOURCE][i];
  }
  found[SOURCE] = 1;
  count = 1; 

  while(count < n) 
  {
    least = 987654321; // infinitly large distance

    // find the minimum distance vertex
    for(i = 0; i < n; i++) 
    {
      tmp = distance[i];
      if( (!found[i]) && (tmp < least) ) 
      {
        least = tmp;
        leastPos = i;
      }
    }

    found[leastPos] = 1;
    count++;
    // update the distances for all  nodes
    for(i = 0; i < n; i++) 
    {
      if( (!found[i]) )
      {
        distance[i] = min(distance[i], least + edge[leastPos][i]);
      }
    }
  } /*** End of while ***/

  free(found);
}

void dijkstraParallel(int SOURCE, int n, int **edge, int *distance)
{
  int i, j, count, tmp, least, leastPos, *found, pid, np, *allLeasts, *allLeastPos;
  MPI_Status status;

  // get MPI information
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  // find process chunk
  int start = (pid * n) / np;
  int end = (((pid + 1) * n) / np) - 1;

  // initialization...
  allLeasts = (int*)malloc(sizeof(int) * np);
  allLeastPos = (int*)malloc(sizeof(int) * np);

  found = (int *) calloc(n, sizeof(int));
  for(i = 0; i < n; i++) 
  {
    found[i] = 0;
    distance[i] = edge[SOURCE][i];
  }
  found[SOURCE] = 1;
  count = 1; 

  while(count < n) 
  {
    least = 987654321; // infinitly large distance

    // find the minimum distance vertex
    for(i = start; i < end; i++) 
    {
      tmp = distance[i];
      if( (!found[i]) && (tmp < least) ) 
      {
        least = tmp;
        leastPos = i;
      }
    }

    // collect all leasts and leastPos to find global least and leastPos

    // send own least and leastPos to all other processes

    for (int i = 0; i < np; i++)
    {
      if (i != pid)
      {
        MPI_Send(&least, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Send least to process i
        printf("%d: Sent least to %d\n", pid, i);
        MPI_Send(&leastPos, 1, MPI_INT, i, 1, MPI_COMM_WORLD); // Send leastPos to process i
        printf("%d: Sent leastPos to %d\n", pid, i);
      }
    }

    // collect leasts and leastPos from all other processes
    for (int i = 0; i < np; i++)
    {
      if (i != pid)
      {
        MPI_Recv(&(allLeasts[i]), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status); // Recv least from process i and put in ith position in allLeasts
        printf("%d: Recv least from %d\n", pid, i);
        MPI_Recv(&(allLeastPos[i]), 1, MPI_INT, i, i, MPI_COMM_WORLD, &status); // Recv leastPos from process i and put in ith position in allLeastPos
        printf("%d: Recv least from %d\n", pid, i);
      }
    }

    // append own least and leastPos to the list
    allLeasts[pid] = least;
    allLeastPos[pid] = leastPos;

    // get least of allLeasts
    int leastAllLeastPos = 0;
    for (int i = 1; i < np; i++) 
    {
      if(allLeasts[leastAllLeastPos] > allLeasts[i])
      {
        leastAllLeastPos = i;
      }
    }

    // save global least and least pos
    least = allLeasts[leastAllLeastPos];
    leastPos = allLeastPos[leastAllLeastPos];

    found[leastPos] = 1;
    count++;
    // update the distances for all  nodes
    for(i = start; i < end; i++) 
    {
      if( (!found[i]) )
      {
        distance[i] = min(distance[i], least + edge[leastPos][i]);
      }
    }
  } /*** End of while ***/
  free(allLeasts);
  free(found); 
}

void printDistance(int* distance, int n)
{
  printf("[");
  for (int i = 0; i < n; i++)
  {
    printf("%d, ", distance[i]);
  }
  printf("]\n");
}

void printEdge(int** edge, int n)
{
  for (int i = 0; i < n; i++)
  {
    printf("[");
    for (int j = 0; j < n; j++)
    {
      printf("%d, ", edge[i][j]);
    }
    printf("]\n");
  }
}

int min(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}