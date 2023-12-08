#define _XOPEN_SOURCE 600

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 3
#define M 4
#define Q 5
#define TRUE 1

int queue[Q];
int length = 0;
int head = 0;
int tail = 0;

pthread_mutex_t mutex;
pthread_cond_t empty;
pthread_cond_t full;
pthread_barrier_t barrier;
 
void *robot(void *args)
{
    // Wait for all threads to be initialized
    pthread_barrier_wait(&barrier);

    // Get the thread id from the arguments
    int id = *((int *) args);

    while (TRUE)
    {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        while (length == 0)
        {
            pthread_cond_wait(&full, &mutex);
        }

        // Take a request from the queue
        int request = queue[head];
        length--; head++;

        printf("[INFO] [robot_%d]: Table %d's request granted!\n", id, request);
        sleep(1);

        if (head == Q) 
        {
            head = 0;
        }

        if (length == Q - 1)
        {
            pthread_cond_broadcast(&empty);
        }

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }
    
    pthread_exit(NULL);
}

void *table(void *args)
{
    // Wait for all threads to be initialized
    pthread_barrier_wait(&barrier);

    // Get the thread id from the arguments
    int id = *((int *) args);

    while (TRUE)
    {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        while (length == Q)
        {
            pthread_cond_wait(&empty, &mutex);
        }

        // Add a request to the queue
        queue[tail] = id;
        length++; tail++;

        printf("[INFO] [table_%d]: Request added to the queue!\n", id);
        sleep(1);

        if (tail == Q)
        {
            tail = 0;
        } 
    
        if (length == 1)
        { 
            pthread_cond_broadcast(&full);
        }

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Create N threads to represent the robots and M threads to represent the tables
    int *robot_id[N], *table_id[M];
    pthread_t robot_thread[N], table_thread[M];

    // Initialize the mutex that controls the access to the queue
    pthread_mutex_init(&mutex, NULL);

    // Initialize the conditional variables
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);

    // Initialize a barrier that is only released after all threads have been intialized
    pthread_barrier_init(&barrier, NULL, N + M);

    // Initialize the robot threads
    for (int i = 0; i < N; i++)
    {
        printf("[INFO] [main]: Initializing robot thread %d...\n", i);

        robot_id[i] = (int *) malloc(sizeof(int));
        *robot_id[i] = i;

        int status = pthread_create(&robot_thread[i], NULL, robot, (void *) robot_id[i]);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    // Initialize the table threads
    for (int i = 0; i < M; i++)
    {
        printf("[INFO] [main]: Initializing table thread %d...\n", i);

        table_id[i] = (int *) malloc(sizeof(int));
        *table_id[i] = i;

        int status = pthread_create(&table_thread[i], NULL, table, (void *) table_id[i]);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    // Destroy the barrier after all threads have been initialized
    pthread_barrier_destroy(&barrier);

    // Join the robot threads
    for (int i = 0; i < N; i++)
    {
        int status = pthread_join(robot_thread[i], NULL);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
            exit(-1);
        }
    }

    // Join the table threads
    for (int i = 0; i < M; i++)
    {
        int status = pthread_join(table_thread[i], NULL);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
            exit(-1);
        }
    }

    // Free the memory that was allocated to store the thread ids
    for (int i = 0; i < N; i++)
    {
        free(robot_id[i]);
    }

    for (int i = 0; i < M; i++)
    {
        free(table_id[i]);
    }

    // Destroy the conditional variables
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&full);

    // Destroy the mutex that controls the access to the queue
    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
}