#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 5

int V[N] = {3, 4, 2, 1, 0};
int A[N] = {3, 4, 2, 1, 0};

void *sort(void *args);
void merge(int l, int r);

int main()
{
    // Variables
    int i, tmp;

    // Printing the unsorted array
    printf("[INFO] [main]: Unsorted array | "); for (i = 0; i < N; i++) printf("%d ", V[i]); printf("\n");

    // Creating the main mergesort thread
    pthread_t mergeSort;

    // Limits info
    int limits[2] = {0, N - 1};

    // Creating the thread
    tmp = pthread_create(&mergeSort, NULL, sort, (void *) limits);

    if (tmp)
    {
        printf("[ERROR] [main]: pthread_join returned error code %d\n", tmp);
        exit(-1);
    }

    // Wait for the thread to be executed
    pthread_join(mergeSort, NULL);

    // Printing the sorted array
    printf("[INFO] [main]:   Sorted array | "); for (i = 0; i < N; i++) printf("%d ", V[i]); printf("\n");

    pthread_exit(NULL);
}

// Merge function
void merge(int l, int r)
{
    int i, j, k, m;
    for (i = l; i <= r; i++) A[i] = V[i];

    m = floor((l + r) / 2);
    i = l;
    j = m + 1;

    for (k = l; k <= r; k++)
    {
        if (i == m + 1)
        {
            V[k] = A[j];
            j = j + 1;
        }
        else if (j == r + 1)
        {
            V[k] = A[i];
            i = i + 1;
        }
        else if (A[i] <= A[j])
        {
            V[k] = A[i];
            i = i + 1;
        }
        else
        {
            V[k] = A[j];
            j = j + 1;
        }
    }
}

void *sort(void *args)
{
    int l, r, *limits, m;

    // Getting the limits
    limits = ((int *) args);
    l = limits[0];
    r = limits[1];

    // If l == r
    if (l == r)
        pthread_exit(NULL);

    // Middle element of the array
    m = floor((l + r) / 2);

    // Left Thread sort{l -> m} - Right Thread {m + 1 -> r}
    pthread_t left, right;

    // New limits
    int leftLimits[2] = {l, m};
    int rightLimits[2] = {m + 1, r};

    // Creating the new threads
    pthread_create(&left, NULL, sort, (void *) leftLimits);
    pthread_create(&right, NULL, sort, (void *) rightLimits);

    // Wait for the left side to finish
    pthread_join(left, NULL);

    // Wait for the right side to finish
    pthread_join(right, NULL);

    // Call the merge thread
    merge(l, r);

    // Exit thread
    pthread_exit(NULL);
}
