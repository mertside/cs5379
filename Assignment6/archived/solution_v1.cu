// CS5379: Parallel Processing - Assignment 6
// 
// by Tyler JOHNSON and Mert SIDE
// on 20211118
//

/*****************************************************************************
 * Write a CUDA code that parallelizes the sequential pseudo code given below 
 *   so that each thread working on updating a sub-matrix of size n/p x n, 
 *   where p is the total number of threads. Use multiple thread blocks and 
 *   multiple threads in each block. You may assume n divisible by the total 
 *   number of threads.
 * 
 * Input:  
 *         D, n x n matrix with 0 on diagonal, positive values other places
 * Output: 
 *         D, n x n matrix
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>

// ==================================min======================================
/*
__host__ int min(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}
*/

__host__ void sequential_minimize_matrix(int **D, int n)
{
  int i, j, k, *hbuf, *vbuf;

  /*** hbuf[n], vbuf[n]:  local buffers used in the alg. ***/
  hbuf = (int *) malloc(n);
  vbuf = (int *) malloc(n);

  for(k = 0; k < n; k++) { 
    for(i = 0; i < n; i++) { 
      vbuf[i] = D[i][k];
    }
    
    for(j = 0; j < n; j++) { 
      hbuf[j] = D[k][j];
    }
      
    for(i = 0; i < n; i++) {
      for(j = 0; j < n; j++) {
        D[i][j] = min(D[i][j], vbuf[i] + hbuf[j]);
      }
    }
  }
}

__global__  void add(int n, int *a, int *b, int *c) 
{
  int i, i_start, i_end,
  subset_size = n/(gridDim.x*blockDim.x);

  i_start = (blockIdx.x * blockDim.x + threadIdx.x) * subset_size;  
  i_end   = i_start + subset_size;
  
  for(i = i_start; i < i_end; i++)
    c[i] = a[i] + b[i];
}

int main(int argc, char **argv)
{
  int n, size, i, *a, *b, *c, *d_a, *d_b, *d_c; 
  n = 1024;
  
  size = n * sizeof(int) ;
  
  a = (int *) malloc(size);
  b = (int *) malloc(size);
  c = (int *) malloc(size);
  
  for(i = 0; i < n; i++) { // Enter data into a[]& b[]
    a[i] = 1;
    b[i] = 2;
  }

  cudaMalloc( (void **) &d_a, size );
  cudaMalloc( (void **) &d_b, size );
  cudaMalloc( (void **) &d_c, size );
  cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

  add<<<16, 16>>>(n, d_a, d_b, d_c) ;
  
  cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
  
  for(i = 0; i < n; i++) {
    //if (i % m == 0 && i != 0) printf("\n");
    printf("%d ", c[i]);
  }
  printf("\n");
  
  free(a); free(b);  free(c);
  cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);

  return 0;
}
