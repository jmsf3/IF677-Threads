#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4

// NxN matrix
int matrix[N][N];

// Global variables
int TOTAL_SUM = 0;
int MAGIC_SQUARE = 1;

// Function that organizes the threads that check if the matrix is a magic square
void *magic_square(void *args);

// Function that calculates the sum of each row
void *sum_rows(void *args);

// Function that calculates the sum of each column
void *sum_columns(void *args);

// Function that calculates the sum of each diagonal
void *sum_diagonals(void *args);

// Mutex that controls the access to the MAGIC_SQUARE global variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    // Variables
    int i, j;

    // Matrix input
    printf("[INPUT] [main]: Enter a %dx%d matrix:\n\n", N, N);

    for (i = 0; i < N; i++)
    {
        printf("                ");

        for (j = 0; j < N; j++)
        {
            scanf("%d", &matrix[i][j]);

            if (i == 0)
            {
                TOTAL_SUM += matrix[i][j];
            }
        }
    }
    printf("\n");

    // Create the magic square thread
    pthread_t magic_square_thread;
    int status = pthread_create(&magic_square_thread, NULL, magic_square, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
        exit(-1);
    }

    // Wait for all the threads to finish their execution
    pthread_join(magic_square_thread, NULL);

    // Print the result
    if (MAGIC_SQUARE)
        printf("[INFO] [main]: It's a magic square!\n");
    else
        printf("[INFO] [main]: It's not a magic square :(\n");

    // Destroy the mutex
    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
}

void *magic_square(void *args)
{
    // Create 3 threads to execute the 'sum_columns', 'sum_rows' and 'sum_diagonals' functions
    pthread_t column, row, diagonal;

    // Create threads
    pthread_create(&column, NULL, sum_columns, NULL);
    pthread_create(&column, NULL, sum_columns, NULL);
    pthread_create(&diagonal, NULL, sum_diagonals, NULL);

    // Wait for all the threads to finish their execution
    pthread_join(column, NULL);
    pthread_join(row, NULL);
    pthread_join(diagonal, NULL);

    pthread_exit(NULL);
}

void *sum_columns(void *args)
{
    int sum;

    // Sum all the elements in a column
    for (int i = 0; i < N && MAGIC_SQUARE == 1; i++)
    {
        sum = 0;

        for (int j = 0; j < N && MAGIC_SQUARE == 1; j++)
        {
            sum += matrix[j][i];
        }

        // Check if the matrix is not a magic square
        if (sum != TOTAL_SUM && MAGIC_SQUARE == 1)
        {
            pthread_mutex_lock(&mutex);
            MAGIC_SQUARE = 0;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void *sum_rows(void *args)
{
    int sum;

    // Sum all the elements in a row
    for (int i = 0; i < N && MAGIC_SQUARE == 1; i++)
    {

        sum = 0;

        for (int j = 0; j < N && MAGIC_SQUARE == 1; j++)
        {
            sum += matrix[i][j];
        }

        // Check if the matrix is not a magic square
        if (sum != TOTAL_SUM && MAGIC_SQUARE == 1)
        {
            pthread_mutex_lock(&mutex);
            MAGIC_SQUARE = 0;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void *sum_diagonals(void *args)
{
    int sum = 0;

    // Sum all the elements in the main diagonal
    for (int i = 0; i < N && MAGIC_SQUARE == 1; i++)
    {
        sum += matrix[i][i];
    }

    // Check if the matrix is not a magic square
    if (sum != TOTAL_SUM && MAGIC_SQUARE == 1)
    {
        pthread_mutex_lock(&mutex);
        MAGIC_SQUARE = 0;
        pthread_mutex_unlock(&mutex);
    }

    sum = 0;

    // Sums all elements in the antidiagonal
    for (int i = 0; i < N && MAGIC_SQUARE == 1; i++)
    {
        sum += matrix[(N - 1) - i][i];
    }

    // Check if the matrix is not a magic square    
    if (sum != TOTAL_SUM && MAGIC_SQUARE == 1)
    {
        pthread_mutex_lock(&mutex);
        MAGIC_SQUARE = 0;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}