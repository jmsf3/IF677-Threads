#include <iostream>

const int MATRIX_SIZE = 4;

// Defina a estrutura para um vetor esparso
struct SparseVector {
    int size;
    int* indices;
    double* values;
};

// Variáveis globais para a matriz esparsa e vetor denso
double** sparseMatrix;
SparseVector denseVector = {MATRIX_SIZE, new int[MATRIX_SIZE]{0, 1, 2, 3}, new double[MATRIX_SIZE]{1.0, 2.0, 3.0, 4.0}};

// Função para multiplicação de matriz esparsa por vetor denso
void sparseMatrixVectorMultiply(double* result) {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        result[i] = 0.0;

        for (int j = 0; j < denseVector.size; ++j) {
            result[i] += sparseMatrix[i][j] * denseVector.values[j];
        }
    }
}

int main() {
    // Inicialize a matriz esparsa (exemplo)
    sparseMatrix = new double*[MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        sparseMatrix[i] = new double[MATRIX_SIZE];
    }

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            sparseMatrix[i][j] = (i == j) ? 2.0 : -1.0;
        }
    }

    double* result = new double[MATRIX_SIZE];

    // Chame a função de multiplicação
    sparseMatrixVectorMultiply(result);

    // Exiba o resultado
    std::cout << "Resultado da multiplicação:" << std::endl;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        std::cout << result[i] << " ";
    }

    // Libere a memória alocada
    delete[] result;
    delete[] denseVector.indices;
    delete[] denseVector.values;

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        delete[] sparseMatrix[i];
    }
    delete[] sparseMatrix;

    return 0;
}
