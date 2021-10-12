/*****************************************************************************
* Input:  n and edge[n][n], where n is the number of vertices of a graph
*         edge[i][j] is the length of the edge from vertex i to vertex j
* Output: distance[n], the distance from the SOURCE vertex to vertex i.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

struct LeastGroup {
  int least;
  int leastPos;
};

void dijkstra(int SOURCE, int n, int **edge, int *distance);
void dijkstraParallel(int SOURCE, int n, int **edge, int *distance);
struct LeastGroup getLeastOfAllLeasts(struct LeastGroup group, int pid, int np);
void printDistance(int* distance, int n);
int min(int x, int y);

int main(int argc, char** argv)
{
  MPI_Init( &argc, &argv );

  int pid;
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);

  int source = 3;
  int n = 40;
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
      edge[i][j] = (rand() % 100) + 1;
    }
  }

  // Get non-parallel distances
  dijkstra(source, n, edge, distance);
  printf("Static:   (%d) ", pid);
  printDistance(distance, n);

  // Reset distances
  for (int i =0; i < n; i++)
  {
    distance[i] = 987654321;
  }

  // Get parallel distances
  dijkstraParallel(source, n, edge, distance);
  printf("Parallel: (%d) ", pid);
  printDistance(distance, n);

  // Free memory
  free(distance);
  for (int i = 0; i < n; i++)
  {
    free(edge[i]);
  }
  free(edge);

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
    for(i = start; i <= end; i++) 
    {
      tmp = distance[i];
      if( (!found[i]) && (tmp < least) ) 
      {
        least = tmp;
        leastPos = i;
      }
    }

    // Send to pid 0 least and leastPos
    struct LeastGroup send, recv;
    send.least = least;
    send.leastPos = leastPos;

    // Recv from pid 0 the least of all leasts and leastPos
    recv = getLeastOfAllLeasts(send, pid, np);

    least = recv.least;
    leastPos = recv.leastPos;

    found[leastPos] = 1;
    count++;
    // update the distances for all  nodes
    for(i = start; i <= end; i++) 
    {
      if( (!found[i]) )
      {
        distance[i] = min(distance[i], least + edge[leastPos][i]);
      }
    }
  } /*** End of while ***/

  // Combine calculated distances
  MPI_Status status;

  if (pid == 0)
  {
    // Create the correct distance array in pid 0
    for (int i = end + 1; i < n; i++)
    {
      // Find which pid provides the result
      int providerPID = 0;
      for (int j = 1; j < np; j++)
      {
        int start = (j * n) / np;
        int end = (((j + 1) * n) / np) - 1;

        if (i >= start && i <= end)
        {
          providerPID = j;
          break;
        }
      }

      // Recv the correct result from the providerPID
      MPI_Recv(&(distance[i]), 1, MPI_INT, providerPID, i, MPI_COMM_WORLD, &status);
    }

    // Send back the correct distance array to all other pids
    for (int i = 1; i < np; i++)
    {
      MPI_Send(distance, n, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
  }
  else
  {
    // Send to pid 0 calculated values for its chunk
    for (int i = start; i <= end; i++)
    {
      MPI_Send(&(distance[i]), 1, MPI_INT, 0, i, MPI_COMM_WORLD);
    }

    // Recv the entire correct array from pid 0
    MPI_Recv(distance, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
  }

  free(allLeasts);
  free(found); 
}

struct LeastGroup getLeastOfAllLeasts(struct LeastGroup group, int pid, int np)
{
  MPI_Status status;

  struct LeastGroup* groups = (struct LeastGroup*)malloc(sizeof(struct LeastGroup) * np);

  if (pid == 0)
  {
    // Add pid0s group to the list
    groups[0] = group;

    // Get all other pids groups
    for (int i = 1; i < np; i++)
    {
      int least, leastPos;
      MPI_Recv(&least, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status); // Recv least from process i and put in ith position in groups
      MPI_Recv(&leastPos, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status); // Recv least from process i and put in ith position in groups

      struct LeastGroup newGroup;
      newGroup.least = least;
      newGroup.leastPos = leastPos; 
      groups[i] = newGroup;
    }

    // Get the least group of all leasts
    int leastOfLeastPos = 0;
    for (int i = 1; i < np; i++)
    {
      if (groups[i].least < groups[leastOfLeastPos].least)
      {
        leastOfLeastPos = i;
      }
    }

    // Send the least group back
    for (int i = 1; i < np; i++)
    {
      MPI_Send(&(groups[leastOfLeastPos].least), 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&(groups[leastOfLeastPos].leastPos), 1, MPI_INT, i, 1, MPI_COMM_WORLD);
    }

    // Return to pid 0 the least group
    return groups[leastOfLeastPos];
  }
  else
  {
    // Send to pid 0 its group
    MPI_Send(&(group.least), 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // Send least to process 0
    MPI_Send(&(group.leastPos), 1, MPI_INT, 0, 1, MPI_COMM_WORLD); // Send leastPos to process 0

    // Recv the least group from pid 0 
    int least, leastPos;
    MPI_Recv(&least, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); // Recv least from process 0 and put in group
    MPI_Recv(&leastPos, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // Recv least from process 0 and put in group

    // Return the least group
    struct LeastGroup newGroup;
    newGroup.least = least;
    newGroup.leastPos = leastPos;
    return newGroup;
  }
}

void printDistance(int* distance, int n)
{
  printf("[");
  for (int i = 0; i < n; i++)
  {
    printf("%2d, ", distance[i]);
  }
  printf("]\n");
}

int min(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}