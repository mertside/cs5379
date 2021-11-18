/*** n threads on one thread block ***/

#include <stdio.h>
#include <stdlib.h>

__global__  void add(int *a, int *b, int *c) {
  c[threadIdx.x] = a[threadIdx.x]+b[threadIdx.x];
}

int main(int argc, char **argv)
{
  int n, size, i, *a, *b, *c, *d_a, *d_b, *d_c; 
  n = 1024 ;
  size = n*sizeof(int) ;
  a = (int *) malloc(size);
  b = (int *) malloc(size);
  c = (int *) malloc(size);
  for(i=0;i<n; i++) { // Enter data into a[]& b[]
    a[i] = 1;
    b[i] = 2;
  }
  cudaMalloc( (void **) &d_a, size );
  cudaMalloc( (void **) &d_b, size );
  cudaMalloc( (void **) &d_c, size );
  cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

  add<<<1, n>>>(d_a, d_b, d_c) ;
  cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
  
  for(i=0;i<n; i++) {
    printf("%d ", c[i]);
  }
  printf("\n");
  
  free(a); free(b);  free(c);
  cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);

  return 0;
}
