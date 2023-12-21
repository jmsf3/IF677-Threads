#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MATRIX_SIZE 4
#define NUM_THREADS 4

// Defina a estrutura para um vetor esparso
typedef struct {
    int size;
    int* indices;
    double* values;
} SparseVector;

// Defina a estrutura para uma matriz esparsa
typedef struct {
    int rows;
    int cols;
    SparseVector* rows_data;
} SparseMatrix;

// Variáveis globais para as matrizes esparsas e vetor denso
SparseMatrix sparseMatrix1;
SparseMatrix sparseMatrix2;
SparseVector denseVector = {MATRIX_SIZE, NULL, NULL};

// Variáveis globais para o resultado e controle de threads
double* result_multiply_vector;
double** result_multiply_matrix;
pthread_t threads[NUM_THREADS];
int thread_args[NUM_THREADS];

// Função para multiplicação de matriz esparsa por vetor denso (paralela)
void* parallelSparseMatrixVectorMultiply(void* arg) {
    int thread_id = *(int*)arg;

    // Calcular a faixa de linhas a serem processadas por esta thread
    int start_row = (MATRIX_SIZE / NUM_THREADS) * thread_id;
    int end_row = (thread_id == (NUM_THREADS - 1)) ? MATRIX_SIZE : start_row + (MATRIX_SIZE / NUM_THREADS);

    // Multiplicação de matriz esparsa por vetor denso
    for (int i = start_row; i < end_row; i++) {
        result_multiply_vector[i] = 0.0;

        for (int j = 0; j < sparseMatrix1.rows; j++) {
            for (int k = 0; k < sparseMatrix1.rows_data[i].size; ++k) {
                if (sparseMatrix1.rows_data[i].indices[k] == j) {
                    result_multiply_vector[i] += sparseMatrix1.rows_data[i].values[k] * denseVector.values[j];
                }
            }
        }
    }

    pthread_exit(NULL);
}

// Função para multiplicação de duas matrizes esparsas (paralela)
void* parallelSparseMatrixMultiply(void* arg) {
    int thread_id = *(int*)arg;

    // Calcular a faixa de linhas a serem processadas por esta thread
    int start_row = (MATRIX_SIZE / NUM_THREADS) * thread_id;
    int end_row = (thread_id == (NUM_THREADS - 1)) ? MATRIX_SIZE : start_row + (MATRIX_SIZE / NUM_THREADS);

    // Multiplicação de duas matrizes esparsas
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < sparseMatrix2.cols; j++) {
            result_multiply_matrix[i][j] = 0.0;

            for (int k = 0; k < sparseMatrix1.rows; ++k) {
                for (int l = 0; l < sparseMatrix1.rows_data[i].size; ++l) {
                    int col_index = sparseMatrix1.rows_data[i].indices[l];
                    double value1 = sparseMatrix1.rows_data[i].values[l];

                    for (int m = 0; m < sparseMatrix2.rows_data[k].size; ++m) {
                        if (sparseMatrix2.rows_data[k].indices[m] == j && col_index == k) {
                            result_multiply_matrix[i][j] += value1 * sparseMatrix2.rows_data[k].values[m];
                        }
                    }
                }
            }
        }
    }

    pthread_exit(NULL);
}

// Função para multiplicação de matriz esparsa por matriz densa (paralela)
void* parallelSparseMatrixDenseMultiply(void* arg) {
    int thread_id = *(int*)arg;

    // Calcular a faixa de linhas a serem processadas por esta thread
    int start_row = (MATRIX_SIZE / NUM_THREADS) * thread_id;
    int end_row = (thread_id == (NUM_THREADS - 1)) ? MATRIX_SIZE : start_row + (MATRIX_SIZE / NUM_THREADS);

    // Multiplicação de matriz esparsa por matriz densa
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            result_multiply_matrix[i][j] = 0.0;

            for (int k = 0; k < sparseMatrix1.rows; ++k) {
                for (int l = 0; l < sparseMatrix1.rows_data[i].size; ++l) {
                    int col_index = sparseMatrix1.rows_data[i].indices[l];
                    double value1 = sparseMatrix1.rows_data[i].values[l];

                    result_multiply_matrix[i][j] += value1 * denseVector.values[col_index];
                }
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    // Inicialize a primeira matriz esparsa (usuário insere manualmente)
    sparseMatrix1.rows = MATRIX_SIZE;
    sparseMatrix1.cols = MATRIX_SIZE;
    sparseMatrix1.rows_data = (SparseVector*)malloc(MATRIX_SIZE * sizeof(SparseVector));

    printf("Insira os elementos da primeira matriz esparsa:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        sparseMatrix1.rows_data[i].size = 0;  // Inicializa o tamanho do vetor esparso

        printf("Linha %d (separados por espaço): ", i);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double value;
            scanf("%lf", &value);

            if (value != 0.0) {
                sparseMatrix1.rows_data[i].indices = (int*)realloc(sparseMatrix1.rows_data[i].indices, (sparseMatrix1.rows_data[i].size + 1) * sizeof(int));
                sparseMatrix1.rows_data[i].values = (double*)realloc(sparseMatrix1.rows_data[i].values, (sparseMatrix1.rows_data[i].size + 1) * sizeof(double));

                sparseMatrix1.rows_data[i].indices[sparseMatrix1.rows_data[i].size] = j;
                sparseMatrix1.rows_data[i].values[sparseMatrix1.rows_data[i].size] = value;

                sparseMatrix1.rows_data[i].size++;
            }
        }
    }

    // Inicialize a segunda matriz esparsa (usuário insere manualmente)
    sparseMatrix2.rows = MATRIX_SIZE;
    sparseMatrix2.cols = MATRIX_SIZE;
    sparseMatrix2.rows_data = (SparseVector*)malloc(MATRIX_SIZE * sizeof(SparseVector));

    printf("Insira os elementos da segunda matriz esparsa:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        sparseMatrix2.rows_data[i].size = 0;  // Inicializa o tamanho do vetor esparso

        printf("Linha %d (separados por espaço): ", i);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double value;
            scanf("%lf", &value);

            if (value != 0.0) {
                sparseMatrix2.rows_data[i].indices = (int*)realloc(sparseMatrix2.rows_data[i].indices, (sparseMatrix2.rows_data[i].size + 1) * sizeof(int));
                sparseMatrix2.rows_data[i].values = (double*)realloc(sparseMatrix2.rows_data[i].values, (sparseMatrix2.rows_data[i].size + 1) * sizeof(double));

                sparseMatrix2.rows_data[i].indices[sparseMatrix2.rows_data[i].size] = j;
                sparseMatrix2.rows_data[i].values[sparseMatrix2.rows_data[i].size] = value;

                sparseMatrix2.rows_data[i].size++;
            }
        }
    }

    // Inicialize o vetor denso (usuário insere manualmente)
    denseVector.indices = (int*)malloc(MATRIX_SIZE * sizeof(int));
    denseVector.values = (double*)malloc(MATRIX_SIZE * sizeof(double));
    printf("Insira os elementos do vetor denso (separados por espaço): ");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        denseVector.indices[i] = i;
        scanf("%lf", &denseVector.values[i]);
    }

    // Aloque memória para os resultados
    result_multiply_vector = (double*)malloc(MATRIX_SIZE * sizeof(double));
    result_multiply_matrix = (double**)malloc(MATRIX_SIZE * sizeof(double*));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        result_multiply_matrix[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
    }

    // Crie threads para realizar as multiplicações em paralelo
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, parallelSparseMatrixVectorMultiply, (void*)&thread_args[i]);
    }

    // Aguarde até que todas as threads da multiplicação de matriz esparsa por vetor denso terminem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Exiba o resultado da multiplicação de matriz esparsa por vetor denso
    printf("Resultado da multiplicação de matriz esparsa por vetor denso:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        printf("%.2f ", result_multiply_vector[i]);
    }
    printf("\n");

    // Crie threads para realizar as multiplicações em paralelo
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, parallelSparseMatrixMultiply, (void*)&thread_args[i]);
    }

    // Aguarde até que todas as threads da multiplicação de duas matrizes esparsas terminem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Exiba o resultado da multiplicação de duas matrizes esparsas
    printf("Resultado da multiplicação de duas matrizes esparsas:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.2f ", result_multiply_matrix[i][j]);
        }
        printf("\n");
    }

    // Crie threads para realizar as multiplicações em paralelo
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, parallelSparseMatrixDenseMultiply, (void*)&thread_args[i]);
    }

    // Aguarde até que todas as threads da multiplicação de matriz esparsa por matriz densa terminem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Exiba o resultado da multiplicação de matriz esparsa por matriz densa
    printf("Resultado da multiplicação de matriz esparsa por matriz densa:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.2f ", result_multiply_matrix[i][j]);
        }
        printf("\n");
    }

    // Libere a memória alocada
    free(result_multiply_vector);
    free(denseVector.indices);
    free(denseVector.values);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(sparseMatrix1.rows_data[i].indices);
        free(sparseMatrix1.rows_data[i].values);
        free(sparseMatrix2.rows_data[i].indices);
        free(sparseMatrix2.rows_data[i].values);
        free(result_multiply_matrix[i]);
    }
    free(sparseMatrix1.rows_data);
    free(sparseMatrix2.rows_data);
    free(result_multiply_matrix);

    return 0;
}
