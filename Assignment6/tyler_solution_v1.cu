#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int min(int x, int y)
{
    if (x < y)
        return x;
    else
        return y;
}

__global__ void minimize_matrix(int **D, int n)
{
    int p = gridDim.x * blockDim.x;

    int *vbuf = (int *)malloc(sizeof(int) * (n / p));
    int *hbuf = (int *)malloc(sizeof(int) * n);

    // Current thread start
    int i_start = (blockIdx.x * blockDim.x + theadIdx.x) * (n / p);
    // Next thread start
    int i_end = (blockIdx.x * blockDim.x + theadIdx.x + 1) * (n / p);

    for (int k = 0; k < n; k++)
    {
        for (int i = i_start; i < i_end; i++)
        {
            vbuf[i] = D[i][k];
        }
        for (int j = 0; j < n; j++)
        {
            hbuf[j] = D[k][j];
        }

        for (int i = i_start; i < i_end; i++)
        {
            for (int j = 0; j < n; j++)
            {
                D[i][j] = min(D[i][j], vbuf[i] + hbuf[j]);
            }
        }
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    int n = 1024;

    // Create D
    int **D = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; i++)
    {
        D[i] = (int *)malloc(sizeof(int) * n);

        // Randomize D
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                D[i][j] = 0;
            }
            else
            {
                D[i][j] = (rand() % 99) + 1;
            }
        }
    }

    // Run sequential
    // Print out

    // Get number of blocks and threads per block
    int threadsPerBlock = 16;
    int blocks = n / threadsPerBlock;

    // Run parallel
    minimize_matrix<<<blocks, threadsPerBlock>>>(D);
    // Print out
}