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

SparseVector sparseMatrixA[M];
SparseVector sparseMatrixB[N];
double denseMatrixC[N][P];
double denseVectorV[N];

double multiplySparseMatricesResult[M][P];
double multiplySparseMatrixDenseVectorResult[M];
double multiplySparseMatrixDenseMatrixResult[M][P];

void *multiplySparseMatrices(void *args)
{
    int id = *((int *) args);

    for (int i = id; i < M; i += NUM_THREADS)
    {
        for (int j = 0; j < sparseMatrixA[i].size; j++)
        {
            int k = sparseMatrixA[i].index[j];

            for (int m = 0; m < sparseMatrixB[k].size; m++)
            {
                int n = sparseMatrixB[k].index[m];
                multiplySparseMatricesResult[i][n] += sparseMatrixA[i].value[j] * sparseMatrixB[k].value[m];
            }
        }
    }

    pthread_exit(NULL);
}

void *multiplySparseMatrixDenseVector(void *args)
{
    int id = *((int *) args);

    for (int i = id; i < M; i += NUM_THREADS)
    {
        for (int j = 0; j < sparseMatrixA[i].size; j++)
        {
            int k = sparseMatrixA[i].index[j];
            multiplySparseMatrixDenseVectorResult[i] += sparseMatrixA[i].value[j] * denseVectorV[k];
        }
    }

    pthread_exit(NULL);
}

void *multiplySparseMatrixDenseMatrix(void *args)
{
    int id = *((int *) args);

    for (int i = id; i < M; i+= NUM_THREADS)
    {
        for (int j = 0; j < P; j++)
        {
            for (int k = 0; k < sparseMatrixA[i].size; k++)
            {
                int m = sparseMatrixA[i].index[k];
                multiplySparseMatrixDenseMatrixResult[i][j] += sparseMatrixA[i].value[k] * denseMatrixC[m][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main()
{
    // Inicializar a matriz esparsa A
    printf("[INPUT] [main]: Enter a %dx%d matrix (A)\n\n", M, N);

    for (int i = 0; i < M; i++)
    {
        // Printar o recuo da linha
        printf("                ");
    
        // Inicializar o i-ésimo vetor esparso
        sparseMatrixA[i].size = 0;
        sparseMatrixA[i].index = NULL;
        sparseMatrixA[i].value = NULL;

        for (int j = 0; j < N; j++)
        {
            double val;
            scanf("%lf", &val);

            if (val != 0.0)
            {
                int size = sparseMatrixA[i].size;
                int *index = sparseMatrixA[i].index;
                double *value = sparseMatrixA[i].value;

                sparseMatrixA[i].index = (int *) realloc(index, (size + 1) * sizeof(int));
                sparseMatrixA[i].value = (double *) realloc(value, (size + 1) * sizeof(double));

                if (sparseMatrixA[i].index == NULL || sparseMatrixA[i].value == NULL)
                {
                    printf("[ERROR] [main]: failed to allocate memory\n");
                    exit(-1);
                }

                sparseMatrixA[i].index[size] = j;
                sparseMatrixA[i].value[size] = val;

                sparseMatrixA[i].size++;
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
            scanf("%lf", &denseVectorV[i]);
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
            sparseMatrixB[i].size = 0;
            sparseMatrixB[i].index = NULL;
            sparseMatrixB[i].value = NULL;

            for (int j = 0; j < P; j++)
            {
                double val;
                scanf("%lf", &val);

                if (val != 0.0)
                {
                    int size = sparseMatrixB[i].size;
                    int *index = sparseMatrixB[i].index;
                    double *value = sparseMatrixB[i].value;

                    sparseMatrixB[i].index = (int *) realloc(index, (size + 1) * sizeof(int));
                    sparseMatrixB[i].value = (double *) realloc(value, (size + 1) * sizeof(double));

                    if (sparseMatrixB[i].index == NULL || sparseMatrixB[i].value == NULL)
                    {
                        printf("[ERROR] [main]: failed to allocate memory\n");
                        exit(-1);
                    }

                    sparseMatrixB[i].index[size] = j;
                    sparseMatrixB[i].value[size] = val;

                    sparseMatrixB[i].size++;
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
                scanf("%lf", &denseMatrixC[i][j]);
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
    void *functions[3] = {multiplySparseMatrixDenseVector, multiplySparseMatrices, multiplySparseMatrixDenseMatrix};

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
            printf("%.2f ", multiplySparseMatrixDenseVectorResult[i]);
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
                printf("%.2f ", multiplySparseMatricesResult[i][j]);
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
                printf("%.2f ", multiplySparseMatrixDenseMatrixResult[i][j]);
            }
            
            printf("\n");
        }
    }

    // Liberar a memória alocada
    for (int i = 0; i < M; i++)
    {
        free(sparseMatrixA[i].index);
        free(sparseMatrixA[i].value);
    }

    for (int i = 0; i < N; i++)
    {
        free(sparseMatrixB[i].index);
        free(sparseMatrixB[i].value);
    }

    pthread_exit(NULL);
}