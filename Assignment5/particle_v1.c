// CS5379: Parallel Processing - Assignment 5
// by Tyler JOHNSON and Mert SIDE
// on 20211104

/* Assignmemt 5:
 *   Parallelize the force calculation for n particles so that
 *   the computation loads on all processes are balanced, and
 *   commuication cost has a complexity of O(n log_2 p) for
 *   n particles and p processors.
 */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SGN(X) (((X) < 0.0) ? (-1.0) : (1.0))
//#define N     123456

#define C1 1.23456
#define C2 6.54321
#define N 8

// ================================calc-force=================================
/*
 * Input: n, x[n]. Note that x[i] \not= x[j] for different i, j.
 * Output: f[n].
 */
void calc_force(int n, double *x, double *f)
{
    int i, j;
    double diff, tmp;
    for (i = 0; i < n; i++)
        f[i] = 0.0;
    for (i = 1; i < n; i++)
    {
        for (j = 0; j < i; j++)
        {
            diff = x[i] - x[j];
            tmp = 1.0 / diff;
            tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
            f[i] += tmp;
            f[j] -= tmp;
        }
    }
}

// ===========================parallel_calc_force================================
/*
 * Input: n, x[n]. Note that x[i] \not= x[j] for different i, j.
 * Output: f[n].
 */
void parallel_calc_force(int n, double *x, double *f)
{
    int pid, np;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // Clear f
    int i, j;
    double diff, tmp;
    for (i = 0; i < n; i++)
        f[i] = 0.0;

    // Break up array into equal chunks, in the bottom half of the array, for
    // each PID (check README for more info)
    for (i = (pid * n) / (2 * np); i < ((pid + 1) * n) / (2 * np); i++)
    {
        // Used to see what i values a PID is working on (debug)
        // printf("PID: %d, i: %d\n", pid, i);

        // Let a PID work on their lower half chunk
        for (j = 0; j < i; j++)
        {
            diff = x[i] - x[j];
            tmp = 1.0 / diff;
            tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
            f[i] += tmp;
            f[j] -= tmp;
        }

        // Save i for for-loop condition
        int i2 = i;

        // Set i value to the upper half mirrored chunk
        i = n - i - 1;

        // Used to see what i values a PID is working on (debug)
        // printf("PID: %d, i: %d\n", pid, i);

        // Let a PID work on their upper half chunk
        for (j = 0; j < i; j++)
        {
            diff = x[i] - x[j];
            tmp = 1.0 / diff;
            tmp = C1 / (diff * diff * diff) - C2 * SGN(diff) / (diff * diff);
            f[i] += tmp;
            f[j] -= tmp;
        }

        // Recove i value for for-loop condition
        i = i2;
    }

    if (pid == 0)
    {
        // Receive all f's and add them together on PID 0
        MPI_Status status;
        double *temp = (double *)malloc(sizeof(double) * n);

        for (i = 1; i < np; i++)
        {
            MPI_Recv(temp, n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            for (j = 0; j < n; j++)
            {
                f[j] += temp[j];
            }
        }
    }
    else
    {
        // Send f to PID 0
        MPI_Send(f, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

// ===================================MAIN====================================
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    // get mpi stuff
    int pid, np;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // int n = np * 32 ; // number of particles

    srand(time(NULL));

    double *x = (double *)malloc(sizeof(double) * N);
    double *f = (double *)malloc(sizeof(double) * N);

    for (int i = 0; i < N; i++)
    {
        int good = 0;
        double temp;
        while (good == 0)
        {
            good = 1;
            temp = (rand() % 99) + 1;
            for (int j = 0; j < i; j++)
            {
                if (x[j] == temp)
                {
                    good = 0;
                }
            }
        }
        x[i] = temp;
        if (pid == 0)
        {
            printf("%lf\n", x[i]);
        }
    }

    // printf("\nCalculating Serial\n\n");
    calc_force(N, x, f);

    if (pid == 0)
    {
        printf("\nSerial\n");
        for (int i = 0; i < N; i++)
        {
            printf("%lf\n", f[i]);
        }
    }

    // printf("\nCalculating Parallel\n\n");
    parallel_calc_force(N, x, f);

    if (pid == 0)
    {
        printf("\nParallel\n");
        for (int i = 0; i < N; i++)
        {
            printf("%lf\n", f[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
// ===================================END=====================================
