#define _XOPEN_SOURCE 600

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define n 4
#define P 5

double A[n][n] = {{10, -1, 2, 0}, {-1, 11, -1, 3}, {2, -1, 10, -1}, {0, 3, -1, 8}};
double prev_x[n] = {1, 1, 1, 1};
double x[n] = {1, 1, 1, 1};
double b[n] = {6, 25, -11, 15};
int N;

pthread_barrier_t barrier;

void *jacobi(void *args)
{
    // Wait for all threads to be initialized
    pthread_barrier_wait(&barrier);

    // Get the thread id from the arguments
    int id = *((int *) args);

    // Jacobi's method
    for (int k = 0; k < P; k++)
    {
        for (int i = id; i < n; i += N)
        {
            double sigma = 0;

            for (int j = 0; j < n; j++)
            {
                if (j != i)
                {
                    sigma += A[i][j] * prev_x[j];
                }
            }
            
            x[i] = (b[i] - sigma) / A[i][i];
            // printf("[INFO] [jacobi_%d]: For k = %d, x%d = %.2f\n", id, k, i, x[i]);
        }

        // Wait for the other threads to proceed
        pthread_barrier_wait(&barrier);

        for (int i = id; i < n; i += N)
        {            
            prev_x[i] = x[i];
        }

        // Wait for the other threads to proceed
        pthread_barrier_wait(&barrier);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Parameter that determines the number of threads to be created
    printf("[INPUT] [main]: How many threads do you wish to create? ");
    scanf("%d", &N);

    // Initialize a barrier that is only released after all threads have been initialized
    pthread_barrier_init(&barrier, NULL, N);

    // Create N Jacobi threads to divide the method's sequential execution in many parts
    int *jacobi_id[N];
    pthread_t jacobi_thread[N];

    // Initialize the Jacobi threads
    for (int i = 0; i < N; i++)
    {
        printf("[INFO] [main]: Initializing Jacobi thread %d...\n", i);

        jacobi_id[i] = (int *) malloc(sizeof(int));
        *jacobi_id[i] = i;

        int status = pthread_create(&jacobi_thread[i], NULL, jacobi, (void *) jacobi_id[i]);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    // Join the Jacobi threads
    for (int i = 0; i < N; i++)
    {
        int status = pthread_join(jacobi_thread[i], NULL);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
            exit(-1);
        }
    }

    // Free the memory that was allocated to store the thread ids
    for (int i = 0; i < N; i++)
    {
        free(jacobi_id[i]);
    }

    // Show the result
    printf("[INFO] [main]: ");
    for (int i = 0; i < n; i++)
    {
        printf("x%d = %.2f ", i + 1, x[i]);
    }
    printf("\n");

    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
}