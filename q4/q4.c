#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define n 4

int M[n][n];
int total_sum = 0;
int magic_square = 1;
pthread_mutex_t mutex;

void *sum_rows(void *args)
{
    int sum;

    // Sum all the elements in a row
    for (int i = 0; i < n && magic_square == 1; i++)
    {

        sum = 0;

        for (int j = 0; j < n && magic_square == 1; j++)
        {
            sum += M[i][j];
        }

        // Check if M is not a magic square
        if (sum != total_sum && magic_square == 1)
        {
            pthread_mutex_lock(&mutex);
            magic_square = 0;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void *sum_columns(void *args)
{
    int sum;

    // Sum all the elements in a column
    for (int i = 0; i < n && magic_square == 1; i++)
    {
        sum = 0;

        for (int j = 0; j < n && magic_square == 1; j++)
        {
            sum += M[j][i];
        }

        // Check if M is not a magic square
        if (sum != total_sum && magic_square == 1)
        {
            pthread_mutex_lock(&mutex);
            magic_square = 0;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void *sum_diagonals(void *args)
{
    int sum = 0;

    // Sum all the elements in the main diagonal
    for (int i = 0; i < n && magic_square == 1; i++)
    {
        sum += M[i][i];
    }

    // Check if M is not a magic square
    if (sum != total_sum && magic_square == 1)
    {
        pthread_mutex_lock(&mutex);
        magic_square = 0;
        pthread_mutex_unlock(&mutex);
    }

    sum = 0;

    // Sums all elements in the antidiagonal
    for (int i = 0; i < n && magic_square == 1; i++)
    {
        sum += M[(n - 1) - i][i];
    }

    // Check if M is not a magic square    
    if (sum != total_sum && magic_square == 1)
    {
        pthread_mutex_lock(&mutex);
        magic_square = 0;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    printf("[INPUT] [main]: Enter a %dx%d matrix:\n\n", n, n);

    for (int i = 0; i < n; i++)
    {
        printf("                ");

        for (int j = 0; j < n; j++)
        {
            scanf("%d", &M[i][j]);

            if (i == 0)
            {
                total_sum += M[i][j];
            }
        }
    }

    printf("\n");

    // Initialize the mutex that controls the access to the 'magic_square' variable
    pthread_mutex_init(&mutex, NULL);

    // Create the 'sum_rows', 'sum_columns' and 'sum_diagonals' threads
    int status;
    pthread_t rows_thread, columns_thread, diagonals_thread;

    status = pthread_create(&rows_thread, NULL, sum_rows, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
        exit(-1);
    }

    status = pthread_create(&columns_thread, NULL, sum_columns, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
        exit(-1);
    }

    status = pthread_create(&diagonals_thread, NULL, sum_diagonals, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
        exit(-1);
    }

    // Wait for all the threads to finish their execution
    status = pthread_join(rows_thread, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
        exit(-1);
    }

    status = pthread_join(columns_thread, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
        exit(-1);
    }

    status = pthread_join(diagonals_thread, NULL);

    if (status != 0)
    {
        printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
        exit(-1);
    }

    // Show if M is a magic square or not
    if (magic_square)
        printf("[INFO] [main]: M is a magic square!\n");
    else
        printf("[INFO] [main]: M is not a magic square :(\n");

    // Destroy the mutex that controls the access to the 'magic_square' variable
    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
}