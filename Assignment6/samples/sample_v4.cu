/*** 2-dimensional thread block ***/

#include <stdio.h>
#include <stdlib.h>

// a, b c are m x n matrices
__global__  void add(int m, int n, int *a, int *b, int *c) 
{
  int i, j, i_start, i_end, j_start, j_end, kx, ky;
  kx = m / blockDim.x ;
  ky = n / blockDim.y ;
  i_start = threadIdx.x*kx ;  i_end = i_start + kx ;
  j_start = threadIdx.y*ky ;  j_end = j_start + ky ;
  for(i = i_start; i < i_end; i++)
    for(j = j_start; j < j_end; j++)
      c[i*n+j] = a[i*n+j] + b[i*n+j] ;

}

int main(int argc, char **argv)
{
  int m, n, size, i, *a, *b, *c, *d_a, *d_b, *d_c; 
  m = 32; n = 32;
  
  size = m * n * sizeof(int) ;
  
  a = (int *) malloc(size);
  b = (int *) malloc(size);
  c = (int *) malloc(size);
  
  for(i = 0; i < m * n; i++) { // Enter data into a[]& b[]
    a[i] = 1;
    b[i] = 2;
  }

  cudaMalloc( (void **) &d_a, size );
  cudaMalloc( (void **) &d_b, size );
  cudaMalloc( (void **) &d_c, size );
  cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

  dim3 myBlock(4, 4) ;
  add<<<1, myBlock >>>(m, n, d_a, d_b, d_c) ;
  
  cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
  
  for(i = 0; i < m * n; i++) {
    if (i % m == 0 && i != 0) printf("\n");
    printf("%d ", c[i]);
  }
  printf("\n");
  
  free(a); free(b);  free(c);
  cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);

  return 0;
}
