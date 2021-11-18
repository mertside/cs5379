/*** 16 threads on one thread block ***/

#include <stdio.h>
#include <stdlib.h>

__global__  void add(int n, int *a, int *b, int *c) {
  int i, i_start, i_end, subset_size = n/blockDim.x ;
  // blockDim.x: threads per block
  // i_end = ( (i_end<n) ? i_end : n ) ;
  i_start = threadIdx.x* subset_size ;
  i_end = i_start + subset_size ;
  for(i=i_start; i<i_end; i++)
    c[i] = a[i] + b[i] ;

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

  add<<<1, 16>>>(n, d_a, d_b, d_c);
  cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
  
  for(i=0;i<n; i++) {
    printf("%d ", c[i]);
  }
  printf("\n");
  
  free(a); free(b);  free(c);
  cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);

  return 0;
}
