#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1
#define NUM_TRAINS 10
#define NUM_INTERSECTIONS 5
#define MAX_NUM_TRAINS_PER_INTERSECTION 2
#define TRAVERSAL_TIME_SECONDS 2

int railway[NUM_INTERSECTIONS];
pthread_mutex_t mutex[NUM_INTERSECTIONS];
pthread_cond_t available[NUM_INTERSECTIONS];
 
void *train(void *args)
{
    // Get the thread id from the arguments
    int id = *((int *) args);

    // Start at the first intersection
    int intersection = 0;

    while (TRUE)
    {
        // Lock the mutex of the intersection
        pthread_mutex_lock(&mutex[intersection]);

        // Check if the intersection is available. Otherwise, wait for it to be available
        while (railway[intersection] == MAX_NUM_TRAINS_PER_INTERSECTION)
        {
            pthread_cond_wait(&available[intersection], &mutex[intersection]);
        }
        
        // Update the number of trains in the intersection
        printf("[INFO] [train_%d]: Entering intersection %d!\n", id, intersection + 1);
        railway[intersection]++;

        // Unlock the mutex of the intersection
        pthread_mutex_unlock(&mutex[intersection]);

        // Wait for the train to conclude it's traversal through the intersection
        sleep(TRAVERSAL_TIME_SECONDS);

        // Lock the mutex of the intersection
        pthread_mutex_lock(&mutex[intersection]);

        // Update the number of trains in the intersection
        printf("[INFO] [train_%d]: Leaving intersection %d!\n", id, intersection + 1);
        railway[intersection]--;

        // Announce that the intersection is now available 
        if (railway[intersection] == MAX_NUM_TRAINS_PER_INTERSECTION - 1)
        {
            pthread_cond_signal(&available[intersection]);
        }

        // Unlock the mutex of the intersection
        pthread_mutex_unlock(&mutex[intersection]);

        // Move to the next intersection
        intersection = (intersection + 1) % NUM_INTERSECTIONS;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Create threads to represent the trains
    int *train_id[NUM_TRAINS];
    pthread_t train_thread[NUM_TRAINS];

    // For each intersection, initialize it's mutex and conditional variable
    for (int i = 0; i < NUM_INTERSECTIONS; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
        pthread_cond_init(&available[i], NULL);
    }

    // Initialize the train threads
    for (int i = 0; i < NUM_TRAINS; i++)
    {
        // printf("[INFO] [main]: Initializing train thread %d...\n", i + 1);

        train_id[i] = (int *) malloc(sizeof(int));
        *train_id[i] = i + 1;

        int status = pthread_create(&train_thread[i], NULL, train, (void *) train_id[i]);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    // Join the train threads
    for (int i = 0; i < NUM_TRAINS; i++)
    {
        int status = pthread_join(train_thread[i], NULL);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
            exit(-1);
        }
    }

    // Free the memory that was allocated to store the thread ids
    for (int i = 0; i < NUM_TRAINS; i++)
    {
        free(train_id[i]);
    }

    // For each intersection, destroy it's mutex and conditional variable
    for (int i = 0; i < NUM_INTERSECTIONS; i++)
    {
        pthread_mutex_destroy(&mutex[i]);
        pthread_cond_destroy(&available[i]);
    }

    pthread_exit(NULL);
}