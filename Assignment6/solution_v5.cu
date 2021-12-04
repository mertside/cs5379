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
#include <time.h>
#include <cuda_runtime.h>

// ================================printD=====================================
__host__ void printD(int *d, int n)
{
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      printf("%2d, ", d[i * n + j]);
    }
    printf("\n");
  }
  printf("\n");
}

// ================================myMin===================================
__host__ int h_myMin(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// =================sequential_minimize_matrix============================
__host__ void sequential_minimize_matrix(int *D, int n)
{
  int i, j, k, *hbuf, *vbuf;

  /*** hbuf[n], vbuf[n]:  local buffers used in the alg. ***/

  hbuf = (int *)malloc(n);
  vbuf = (int *)malloc(n);

  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      vbuf[i] = D[i * n + k];
    }

    for (j = 0; j < n; j++) {
      hbuf[j] = D[k * n + j];
    }

    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        D[i * n + j] = h_myMin(D[i * n + j], vbuf[i] + hbuf[j]);
      }
    }
  }
}

// ================================myMin===================================
__device__ int d_myMin(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// ==============================minimize_matrix============================
__global__ void minimize_matrix(int *D, int n)
{
  int p = gridDim.x * blockDim.x;

  int *vbuf = (int *)malloc(sizeof(int) * (n / p));
  int *hbuf = (int *)malloc(sizeof(int) * n);

  // Current thread start
  int i_start = (blockIdx.x * blockDim.x + threadIdx.x) * (n / p);
  // Next thread start
  int i_end = (blockIdx.x * blockDim.x + threadIdx.x + 1) * (n / p);

  for (int k = 0; k < n; k++) {
    for (int i = i_start; i < i_end; i++) {
      vbuf[i] = D[i * n + k];
    }
    for (int j = 0; j < n; j++) {
      hbuf[j] = D[k * n + k];
    }

    for (int i = i_start; i < i_end; i++) {
      for (int j = 0; j < n; j++) {
        D[i * n + j] = d_myMin(D[i * n + j], vbuf[i] + hbuf[j]);
      }
    }
  }
}

// ==============================main=====================================
int main(int argc, char **argv)
{
  srand(time(NULL));

  int n = 10;
  
  int *D, *h_D, *d_D;

  // Cuda malloc
  cudaMalloc(&d_D, sizeof(int) * n * n);

  // Create D
  D = (int *)malloc(sizeof(int) * n * n);
  // Randomize D
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == j) {
        D[i * n + j] = 0;
      } else {
        D[i * n + j] = (rand() % 99) + 1;
      }
    }
  }

  printf("(initial) Array:\n");
  printD(D, n);

  // copy D to h_D
  h_D = (int *)malloc(sizeof(int) * n * n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      h_D[i * n + j] = D[i * n + j];
    }
  }

  // Run sequential
  sequential_minimize_matrix(D, n);

  // Print out
  printf("(sequential) Array:\n");
  printD(D, n);

  
  // Get number of blocks and threads per block
  int threadsPerBlock = 16;
  int blocks = n / threadsPerBlock;

  // memcopies to GPU
  cudaMemcpy(d_D, h_D, sizeof(int) * n * n, cudaMemcpyHostToDevice);
  
  // Run parallel
  minimize_matrix<<<blocks, threadsPerBlock>>>(d_D, n);
  
  // memcopies back to host
  cudaMemcpy(h_D, d_D, sizeof(int) * n * n, cudaMemcpyDeviceToHost);
  
  // Print out
  printf("(parallel) Array:\n");
  printD(D, n);

  // Free
  //cudaFree(d_D);
  //free(D);
  //free(h_D);

  return 0;
}
