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
void dijkstraMulticore(int SOURCE, int n, int **edge, int *distance);
void printDistance(int* distance, int n);
void printEdge(int** edge, int n);
int min(int x, int y);

int main(int argc, char** argv)
{
  MPI_Init( &argc, &argv );

  int source = 3;
  int n = 6;
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

  printEdge(edge, n);
  printDistance(distance, n);

  dijkstra(source, n, edge, distance);

  printDistance(distance, n);

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

    // collect all leasts to find global least

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

void dijkstraMulticore(int SOURCE, int n, int **edge, int *distance)
{
  int i, j, count, tmp, least, leastPos, *found, pid, np;

  // get MPI information
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

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

    // collect all leasts to find global least

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