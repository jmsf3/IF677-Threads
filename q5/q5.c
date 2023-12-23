#define _XOPEN_SOURCE 600

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define n 12

int A[n];
int V[n] = {1, 5, 8, 5, 7, 3, 2, 4, 1, 6, 2, 9};

void merge(int l, int m, int r)
{
    for (int i = l; i < r; i++)
    {
        A[i] = V[i];
    }

    int i = l, j = m;

    for (int k = l; k < r; k++)
    {
        if (j == r)
        {
            V[k] = A[i]; i = i + 1;
        }
        else if (i == m)
        {
            V[k] = A[j]; j = j + 1;
        }
        else if (A[i] <= A[j])
        {
            V[k] = A[i]; i = i + 1;
        }
        else
        {
            V[k] = A[j]; j = j + 1;
        }
    }
}

void *sort(void *args)
{
    // Get the arguments of the sort function
    int l = ((int *) args)[0];
    int r = ((int *) args)[1];

    if ((r - l) >= 2)
    {
        int m = (l + r) / 2;

        // Create two child threads to sort the left and right sides of the array
        pthread_t l_thread;
        pthread_t r_thread;

        // Initialize the left thread
        int l_args[2] = {l, m};
        int l_create_status = pthread_create(&l_thread, NULL, sort, (void *) &l_args);

        if (l_create_status != 0)
        {
            printf("[ERROR] [sort]: pthread_create returned error code %d\n", l_create_status);
            exit(-1);
        }

        // Initialize the right thread
        int r_args[2] = {m, r};
        int r_create_status = pthread_create(&r_thread, NULL, sort, (void *) &r_args);

        if (r_create_status != 0)
        {
            printf("[ERROR] [sort]: pthread_create returned error code %d\n", r_create_status);
            exit(-1);
        }

        // Wait for the child threads to finish sorting the subarrays before merging
        int l_join_status = pthread_join(l_thread, NULL);
        int r_join_status = pthread_join(r_thread, NULL);

        if (l_join_status != 0 || r_join_status != 0)
        {
            int error_code = l_join_status != 0 ? l_join_status : r_join_status;
            printf("[ERROR] [main]: pthread_join returned error code %d\n", error_code);
            exit(-1);
        }
    
        merge(l, m, r);
    }

    // Finish thread execution
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Print unsorted array
    printf("[INFO] [main]: V = (");
    for (int i = 0; i < n; i++)
    {
        if (i == n - 1)
            printf("%d)\n", V[i]);
        else
            printf("%d, ", V[i]);
    }

    // Create a thread to sort the array
    pthread_t thread;

    // Initialize the thread
    int args[2] = {0, n};
    int create_status = pthread_create(&thread, NULL, sort, (void *) &args);

    if (create_status != 0)
    {
        printf("[ERROR] [main]: pthread_create returned error code %d\n", create_status);
        exit(-1);
    }

    // Join the thread
    int join_status = pthread_join(thread, NULL);

    if (join_status != 0)
    {
        printf("[ERROR] [main]: pthread_join returned error code %d\n", join_status);
        exit(-1);
    }
    
    // Print sorted array
    printf("[INFO] [main]: V = (");
    for (int i = 0; i < n; i++)
    {
        if (i == n - 1)
            printf("%d)\n", V[i]);
        else
            printf("%d, ", V[i]);
    }

    pthread_exit(NULL);
}