#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5
#define T 2
#define P 5
#define Q 5

int product[P + 1];
pthread_mutex_t mutex[P + 1];

void create_files()
{
    // Seed
    srand(time(NULL));

    for (int x = 1; x <= N; x++)
    {
        // Generate a random number between 1 and Q to define the number of products in the file
        int num_products = rand() % Q + 1;

        // Define the file's path
        char path[10];
        sprintf(path, "%d.in", x);

        // Create the file
        FILE *file = fopen(path, "w");

        if (file == NULL)
        {
            printf("[ERROR] [create_files]: failed to create file %s\n", path);
            exit(-1);
        }

        // Write in the file
        for (int i = 0; i < num_products; i++)
        {
            // Generate a random number between 0 and P
            int p = rand() % (P + 1);

            // Write the product's number in the file
            fprintf(file, "%d\n", p);
        }

        // Close the file
        fclose(file);
    }
}

void *read_files(void *args)
{
    // Get the thread id from the arguments
    int id = *((int *) args);

    for (int x = id; x <= N; x += T)
    {
        // Define the file's path
        char path[10];
        sprintf(path, "%d.in", x);

        // Open the file
        FILE *file = fopen(path, "r");

        if (file == NULL)
        {
            printf("[ERROR] [thread_%d]: failed to open file %s\n", id, path);
            exit(-1);
        }

        // Read the file
        int p;
        
        while (fscanf(file, "%d", &p) != EOF)
        {
            // Lock the mutex
            pthread_mutex_lock(&mutex[p]);

            // Increment the number of products 'p'
            product[p]++;
            // printf("[INFO] [thread_%d]: Incremented product[%d]\n", id, p);

            // Unlock the mutex
            pthread_mutex_unlock(&mutex[p]);
        }

        // Close the file
        fclose(file); 
    }

    pthread_exit(NULL);
} 

int main(int argc, char *argv[])
{
    // Generate input files
    create_files();

    // Create T threads to read the input files
    int *thread_id[T];
    pthread_t thread[T];

    // Initialize the mutexes that control the access to product's array
    for (int i = 0; i <= P; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
    }

    // Initialize the threads
    for (int i = 0; i < T; i++)
    {
        // printf("[INFO] [main]: Initializing thread %d...\n", i + 1);

        thread_id[i] = (int *) malloc(sizeof(int));
        *thread_id[i] = i + 1;

        int status = pthread_create(&thread[i], NULL, read_files, (void *) thread_id[i]);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    // Join the threads
    for (int i = 0; i < T; i++)
    {
        // printf("[INFO] [main]: Waiting for thread %d...\n", i + 1);
        int status = pthread_join(thread[i], NULL);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
            exit(-1);
        }
    }

    // Calculate the total number of products
    int num_products = 0;

    for (int i = 0; i <= P; i++)
    {
        num_products += product[i]; 
    }
    
    printf("[INFO] [main]: num_products = %d\n", num_products);

    // Show the percentual for each product
    for (int i = 0; i <= P; i++)
    {
        printf("[INFO] [main]: product[%d] / num_products = %.2f\n", i, (product[i] * 1.0) / num_products);
    }

    // Free the memory that was allocated to store the thread ids
    for (int i = 0; i < T; i++)
    {
        free(thread_id[i]);
    }

    // Destroy the mutexes that control the access to product's array
    for (int i = 0; i <= P; i++)
    {
        pthread_mutex_destroy(&mutex[i]);
    }

    pthread_exit(NULL);
}