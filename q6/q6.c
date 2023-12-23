#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define M 2
#define N 2
#define P 2
#define NUM_THREADS 4

typedef struct
{
    int size;
    int *index;
    double *value;
} SparseVector;

SparseVector s_matrix_a[M];
SparseVector s_matrix_b[N];
double d_matrix_c[N][P];
double d_vector_v[N];

double multiply_sparse_matrices_result[M][P];
double multiply_sparse_matrix_dense_vector_result[M];
double multiply_sparse_matrix_dense_matrix_result[M][P];

void *multiply_sparse_matrices(void *args)
{
    int id = *((int *) args);

    for (int i = id; i < M; i += NUM_THREADS)
    {
        for (int j = 0; j < s_matrix_a[i].size; j++)
        {
            int k = s_matrix_a[i].index[j];

            for (int m = 0; m < s_matrix_b[k].size; m++)
            {
                int n = s_matrix_b[k].index[m];
                multiply_sparse_matrices_result[i][n] += s_matrix_a[i].value[j] * s_matrix_b[k].value[m];
            }
        }
    }

    pthread_exit(NULL);
}

void *multiply_sparse_matrix_dense_vector(void *args)
{
    int id = *((int *) args);

    for (int i = id; i < M; i += NUM_THREADS)
    {
        for (int j = 0; j < s_matrix_a[i].size; j++)
        {
            int k = s_matrix_a[i].index[j];
            multiply_sparse_matrix_dense_vector_result[i] += s_matrix_a[i].value[j] * d_vector_v[k];
        }
    }

    pthread_exit(NULL);
}

void *multiply_sparse_matrix_dense_matrix(void *args)
{
    int id = *((int *) args);

    for (int i = id; i < M; i+= NUM_THREADS)
    {
        for (int j = 0; j < P; j++)
        {
            for (int k = 0; k < s_matrix_a[i].size; k++)
            {
                int m = s_matrix_a[i].index[k];
                multiply_sparse_matrix_dense_matrix_result[i][j] += s_matrix_a[i].value[k] * d_matrix_c[m][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Inicializar a matriz esparsa A
    printf("[INPUT] [main]: Enter a %dx%d matrix (A)\n\n", M, N);

    for (int i = 0; i < M; i++)
    {
        // Printar o recuo da linha
        printf("                ");
    
        // Inicializar o i-ésimo vetor esparso
        s_matrix_a[i].size = 0;
        s_matrix_a[i].index = NULL;
        s_matrix_a[i].value = NULL;

        for (int j = 0; j < N; j++)
        {
            double val;
            scanf("%lf", &val);

            if (val != 0.0)
            {
                int size = s_matrix_a[i].size;
                int *index = s_matrix_a[i].index;
                double *value = s_matrix_a[i].value;

                s_matrix_a[i].index = (int *) realloc(index, (size + 1) * sizeof(int));
                s_matrix_a[i].value = (double *) realloc(value, (size + 1) * sizeof(double));

                if (s_matrix_a[i].index == NULL || s_matrix_a[i].value == NULL)
                {
                    printf("[ERROR] [main]: failed to allocate memory\n");
                    exit(-1);
                }

                s_matrix_a[i].index[size] = j;
                s_matrix_a[i].value[size] = val;

                s_matrix_a[i].size++;
            }
        }
    }

    printf("\n");

    // Menu
    int operation = -1;
    printf("[INPUT] [main]: What operation would you like to do?\n\n");

    printf("                1. Multiply A by a dense vector V\n");
    printf("                2. Multiply A by a sparse matrix B\n");
    printf("                3. Multiply A by a dense matrix C\n\n");

    printf("                "); 
    scanf("%d", &operation); 
    printf("\n");
    
    if (operation == 1) // Multiplicação por vetor denso
    {
        printf("[INPUT] [main]: Enter a %d-dimensional vector (V)\n\n", N);

        // Printar o recuo da linha
        printf("                ");

        // Inicializar o vetor denso
        for (int i = 0; i < N; i++)
        {
            scanf("%lf", &d_vector_v[i]);
        }

        printf("\n");
    }
    else if (operation == 2) // Multiplicação por matriz esparsa
    {
        printf("[INPUT] [main]: Enter a %dx%d matrix (B)\n\n", N, P);

        for (int i = 0; i < N; i++)
        {
            // Printar o recuo da linha
            printf("                ");

            // Inicializar o i-ésimo vetor esparso
            s_matrix_b[i].size = 0;
            s_matrix_b[i].index = NULL;
            s_matrix_b[i].value = NULL;

            for (int j = 0; j < P; j++)
            {
                double val;
                scanf("%lf", &val);

                if (val != 0.0)
                {
                    int size = s_matrix_b[i].size;
                    int *index = s_matrix_b[i].index;
                    double *value = s_matrix_b[i].value;

                    s_matrix_b[i].index = (int *) realloc(index, (size + 1) * sizeof(int));
                    s_matrix_b[i].value = (double *) realloc(value, (size + 1) * sizeof(double));

                    if (s_matrix_b[i].index == NULL || s_matrix_b[i].value == NULL)
                    {
                        printf("[ERROR] [main]: failed to allocate memory\n");
                        exit(-1);
                    }

                    s_matrix_b[i].index[size] = j;
                    s_matrix_b[i].value[size] = val;

                    s_matrix_b[i].size++;
                }
            }
        }
        
        printf("\n");
    }
    else if (operation == 3) // Multiplicação por matriz densa
    {
        printf("[INPUT] [main]: Enter a %dx%d matrix (C)\n\n", N, P);

        // Inicializar a matriz densa
        for (int i = 0; i < N; i++)
        {
            // Printar o recuo da linha
            printf("                ");

            for (int j = 0; j < P; j++)
            {
                scanf("%lf", &d_matrix_c[i][j]);
            }
        }

        printf("\n");
    }
    else // Operação inválida
    {
        printf("[ERROR] [main]: %d is an invalid operation\n", operation);
        exit(-1);
    }
    
    // Criar as threads para a realização da operação escolhida
    int thread_id[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    void *functions[3] = {multiply_sparse_matrix_dense_vector, multiply_sparse_matrices, multiply_sparse_matrix_dense_matrix};

    // Inicializar as threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_id[i] = i;
        int status = pthread_create(&threads[i], NULL, functions[operation - 1], (void *) &thread_id[i]);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    // Aguardar a finalização da execução das threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        int status = pthread_join(threads[i], NULL);

        if (status != 0)
        {
            printf("[ERROR] [main]: pthread_join returned error code %d\n", status);
            exit(-1);
        }
    }   

    // Exibir os resultados
    if (operation == 1)
    {
        printf("[INFO] [main]: A x V \n\n");

        // Printar o recuo da linha
        printf("               ");

        for (int i = 0; i < M; i++)
        {
            printf("%.2f ", multiply_sparse_matrix_dense_vector_result[i]);
        }

        printf("\n");
    }
    else if (operation == 2)
    {
        printf("[INFO] [main]: A x B \n\n");

        for (int i = 0; i < M; i++)
        {
            // Printar o recuo da linha
            printf("               ");

            for (int j = 0; j < P; j++)
            {
                printf("%.2f ", multiply_sparse_matrices_result[i][j]);
            }

            printf("\n");
        }
    }
    else if (operation == 3)
    {
        printf("[INFO] [main]: A x C \n\n");

        for (int i = 0; i < M; i++)
        {
            // Printar o recuo da linha
            printf("               ");

            for (int j = 0; j < P; j++)
            {
                printf("%.2f ", multiply_sparse_matrix_dense_matrix_result[i][j]);
            }
            
            printf("\n");
        }
    }

    // Liberar a memória alocada
    for (int i = 0; i < M; i++)
    {
        free(s_matrix_a[i].index);
        free(s_matrix_a[i].value);
    }

    for (int i = 0; i < N; i++)
    {
        free(s_matrix_b[i].index);
        free(s_matrix_b[i].value);
    }

    pthread_exit(NULL);
}