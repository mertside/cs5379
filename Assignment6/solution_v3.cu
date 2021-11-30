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

#define N 10

typedef int my_arr[N]; // array width

#define CUDA_RT_CALL(call)                                                    \
{                                                                             \
  cudaError_t cudaStatus = call;                                              \
  if (cudaSuccess != cudaStatus)                                              \
    fprintf(stderr,                                                           \
            "ERROR: CUDA RT call \"%s\" in line %d of file %s failed "        \
            "with "                                                           \
            "%s (%d).\n",                                                     \
            #call, __LINE__, __FILE__,                                        \
            cudaGetErrorString(cudaStatus), cudaStatus);                      \
}

// ================================printD=====================================
void printD(my_arr* d, int n)
{
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      printf("%2d, ", d[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

// ================================myMin===================================
__device__ int d_myMin(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// ================================myMin===================================
__host__ int h_myMin(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// ==============================minimize_matrix============================
__global__ void minimize_matrix(my_arr *D, int n)
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
      vbuf[i] = D[i][k];
    }
    for (int j = 0; j < n; j++) {
      hbuf[j] = D[k][j];
    }

    for (int i = i_start; i < i_end; i++) {
      for (int j = 0; j < n; j++) {
        D[i][j] = d_myMin(D[i][j], vbuf[i] + hbuf[j]);
      }
    }
  }
}

// =================sequential_minimize_matrix============================
__host__ void sequential_minimize_matrix(my_arr *D, int n)
{
  int i, j, k, *hbuf, *vbuf;

  /*** hbuf[n], vbuf[n]:  local buffers used in the alg. ***/
  hbuf = (int *)malloc(n);
  vbuf = (int *)malloc(n);

  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      vbuf[i] = D[i][k];
    }

    for (j = 0; j < n; j++) {
      hbuf[j] = D[k][j];
    }

    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        D[i][j] = h_myMin(D[i][j], vbuf[i] + hbuf[j]);
      }
    }
  }
}

// ==============================main=====================================
int main(int argc, char **argv)
{
  srand(time(NULL));

  int n; 
  n = N;//1024;
  
  my_arr *D, *h_D, *d_D;
  size_t dsize = n * n * sizeof(int);

  // Create D
  //int **D = (int **)malloc(sizeof(int *) * n);
  D = (my_arr *)malloc(dsize);
  for (int i = 0; i < n; i++) {
    //D[i] = (int *)malloc(sizeof(int) * n);

    // Randomize D
    for (int j = 0; j < n; j++) {
      if (i == j) {
        D[i][j] = 0;
      } else {
        D[i][j] = (rand() % 99) + 1;
      }
    }
  }

  printf("(initial) Array:\n");
  printD(D, n);

  // copy D to d_D
  h_D = (my_arr *)malloc(dsize);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      h_D[i][j] = D[i][j];
    }
  }

  // Run sequential
  sequential_minimize_matrix(D,n);
  // Print out
  printf("(sequential) Array:\n");
  printD(D, n);

  // Get number of blocks and threads per block
  int threadsPerBlock = 16;
  int blocks = n / threadsPerBlock;

  // Cuda malloc
  CUDA_RT_CALL(cudaMalloc(&d_D, dsize));

  // memcopies to GPU
  CUDA_RT_CALL(cudaMemcpy(d_D, h_D, dsize, cudaMemcpyHostToDevice));
 
  printf("Kernel Call.\n");
  // Run parallel
  minimize_matrix<<<blocks, threadsPerBlock>>>(d_D, n);
  
  // memcopies back to host
  CUDA_RT_CALL(cudaMemcpy(h_D, d_D, dsize, cudaMemcpyDeviceToHost));
  
  // Print out
  printf("(parallel) Array:\n");
  printD(D, n);

  free(D); free(h_D);
  cudaFree(d_D); 
}
