/*****************************************************************************
* Input:  n and edge[n][n], where n is the number of vertices of a graph
*         edge[i][j] is the length of the edge from vertex i to vertex j
* Output: distance[n], the distance from the SOURCE vertex to vertex i.
*****************************************************************************/

void dijkstra(int SOURCE, int n, int **edge, int *distance) {
  int i, j, count, tmp, least, leastPos, *found;

  // initialization...
  found = (int *) calloc(n, sizeof(int));
  for(i = 0; i < n; i++) {
    found[i] = 0;
    found[i] = edge[SOURCE][i];
  }
  found[SOURCE] = 1;
  count = 1; 

  while(count < n) {
    least = 9876543210; // infinitly large distance

    // find the minimum distance vertex
    for(i = 0; i < n; i++) {
      tmp = distance[i];
      if( (!found[i]) && (tmp < least) ) {
        least = tmp;
        leastPos = i;
      }
    }

    found[leastPos] = 1;
    count++;
    // update the distances for all  nodes
    for(i = 0; i < n; i++) {
      if( (!found[i]) )
        distance[i] = min(distance[i], least + edge[leastPos][i]);
    }
  } /*** End of while ***/
  
  free(found);
  
}
