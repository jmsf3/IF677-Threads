#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Square matrix N * N
#define N 3
int matrix[N][N];

// Variables
int TOTAL_SUM;
int MAGIC_SQUARE = 1;

// Function to organize the threads that check if its a magic square
void *magic_square(void *args);

// Function to add up all sum_columns
void *sum_columns(void *args);

// Function to add up all sum_row
void *sum_row(void *args);

// Function to add up both sum_diagonal
void *sum_diagonal(void *args);

// Mutex to change the value of the variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(){

    // Variables
    int i, j;

    // Matrix insertion loop
    for(i = 0; i < N; i++){

        if(i == 0)
            TOTAL_SUM = 0;

        for(j = 0; j < N; j++){
            scanf("%d", &matrix[i][j]);
            if(i == 0) TOTAL_SUM += matrix[i][j];
        }
    }
    
    // Creating main thread
    pthread_t magicsquare;

    int status = pthread_create(&magicsquare, NULL, magic_square, NULL);
    if(status){
        printf("ERROR %d: MAIN COULD NOT CREATE THREAD\n", status);
        exit(-1);
    }


    // Waiting for all threads to be executed
    pthread_join(magicsquare, NULL);

    // Print result
    if(MAGIC_SQUARE)
        printf("Magic square\n");
    else
        printf("Not magic square\n");

    // Destroying mutex
    pthread_mutex_destroy(&mutex);

    // Exit thread
    pthread_exit(NULL);
}

void *magic_square(void *args){

    // Create threads to execute in sum_columns, sum_row and sum_diagonal
    pthread_t column, row, diagonal;
    
    // Creating threads
    pthread_create(&column, NULL, sum_columns, NULL);
    pthread_create(&column, NULL, sum_columns, NULL);
    pthread_create(&diagonal, NULL, sum_diagonal, NULL);
    
    // Waiting all threads to end
    pthread_join(column, NULL);
    pthread_join(row, NULL);
    pthread_join(diagonal, NULL);

    // Exit thread
    pthread_exit(NULL);
}

void *sum_columns(void *args){

    int sum;

    // Sums all elements in a column
    for(int i = 0; i < N && MAGIC_SQUARE == 1; i++){
        
        sum = 0;
        for(int j = 0; j < N && MAGIC_SQUARE == 1; j++){
            sum += matrix[j][i];
        }

        // Change the variable
        if(sum != TOTAL_SUM && MAGIC_SQUARE == 1){
            pthread_mutex_lock(&mutex);
            MAGIC_SQUARE = 0;
            pthread_mutex_unlock(&mutex);
        } 
    }

    // Exit thread
    pthread_exit(NULL);
}

void *sum_row(void *args){

    int sum;

    // Sums all elements in a row
    for(int i = 0; i < N && MAGIC_SQUARE == 1; i++){
        
        sum = 0;
        for(int j = 0; j < N && MAGIC_SQUARE == 1; j++){
            sum += matrix[i][j];
        }

        // Change the variable
        if(sum != TOTAL_SUM && MAGIC_SQUARE == 1){
            pthread_mutex_lock(&mutex);
            MAGIC_SQUARE = 0;
            pthread_mutex_unlock(&mutex);
        }
    }

    // Exit thread
    pthread_exit(NULL);
}

void *sum_diagonal(void *args){

    int sum;

    // Sums all elements in main diagonal
    for(int i = 0; i < N && MAGIC_SQUARE == 1; i++)
        sum += matrix[i][i];
    
    // Change the variable
    if(sum != TOTAL_SUM && MAGIC_SQUARE == 1){
        pthread_mutex_lock(&mutex);
        MAGIC_SQUARE = 0;
        pthread_mutex_unlock(&mutex);
    }

    sum = 0;

    // Sums all elements in inverse diagonal
    for(int i = 0; i < N && MAGIC_SQUARE == 1; i++)
        sum += matrix[(N - 1) - i][i];
    
    // Change the variable
    if(sum != TOTAL_SUM && MAGIC_SQUARE == 1){
        pthread_mutex_lock(&mutex);
        MAGIC_SQUARE = 0;
        pthread_mutex_unlock(&mutex);
    }

    // Exit thread
    pthread_exit(NULL);
}
