#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5 // Número de arquivos
#define P 5 // Número de produtos
#define T 3 // Número de threads
#define MAX_NUM_PRODUCTS 10 // Número máximo de produtos por arquivo

// Array de mutexes para exclusão mútua refinada
pthread_mutex_t mutexArray[P + 1];

// Array para armazenar a frequência de cada produto
int freqProducts[P + 1] = {0};

// Número total de produtos lidos
int totalProducts = 0;  

void generateRandomList(int *list, int size) 
{
    for (int i = 0; i < size; i++) 
    {
        list[i] = rand() % (P + 1); 
    }
}

void createFiles() 
{
    for (int x = 1; x <= N; x++) 
    {
        int numProducts = rand() % MAX_NUM_PRODUCTS + 1;

        // Definir o caminho do arquivo
        char filename[10];
        sprintf(filename, "%d.in", x);

        // Criar o arquivo
        FILE *file = fopen(filename, "w");

        if (file == NULL) 
        {
            printf("[ERROR] [createFiles]: failed to create file %s\n", filename);
            exit(-1);
        }

        // Gerar uma lista de números aleatórios
        int *randomList = malloc(numProducts * sizeof(int));

        if (randomList == NULL) 
        {
            printf("[ERROR] [createFiles]: failed to allocate memory\n");
            exit(-1);
        }

        generateRandomList(randomList, numProducts);

        for (int i = 0; i < numProducts; i++) 
        {
            fprintf(file, "%d\n", randomList[i]);
        }

        free(randomList);
        fclose(file);
    }
}

void *readFiles(void *args) 
{
    int threadIndex = *((int *) args);

    for (int fileNumber = threadIndex + 1; fileNumber <= N; fileNumber += T) 
    {
        char filename[10];
        sprintf(filename, "%d.in", fileNumber);

        FILE *file = fopen(filename, "r");

        if (file == NULL) 
        {
            printf("[ERROR] [readFiles]: failed to open file %s\n", filename);
            exit(-1);
        }

        // Lê os produtos dos arquivos
        int product;

        while (fscanf(file, "%d", &product) != EOF) 
        {
            // Trava o mutex correspondente à posição do array de frequência
            pthread_mutex_lock(&mutexArray[product]);

            // Atualiza a frequência do produto lido
            freqProducts[product]++;
            totalProducts++;

            // Libera o mutex correspondente à posição do array de frequência
            pthread_mutex_unlock(&mutexArray[product]);
        }

        fclose(file);
    }

    pthread_exit(NULL);
}

int main() 
{
    // Seed
    srand(time(NULL));

    // Cria os arquivos
    createFiles();

    // Cria as threads para ler os arquivos
    pthread_t threads[T];
    int threadIndexes[T];

    // Inicializa os mutexes para cada posição do array de frequência
    for (int i = 0; i <= P; i++) 
    {
        pthread_mutex_init(&mutexArray[i], NULL);
    }

    // Inicializa as threads
    for (int i = 0; i < T; i++) 
    {
        threadIndexes[i] = i;

        if (pthread_create(&threads[i], NULL, readFiles, (void *)&threadIndexes[i]) != 0) 
        {
            printf("[ERROR] [main]: failed to create thread %d\n", i);
            exit(-1);
        }
    }

    // Aguarda a finalização da execução de cada thread
    for (int i = 0; i < T; i++) 
    {
        if (pthread_join(threads[i], NULL) != 0) 
        {
            printf("[ERROR] [main]: failed to join thread %d\n", i);
            exit(-1);
        }
    }

    // Imprime o total de produtos lidos
    printf("[INFO] [main]: totalProducts = %d\n", totalProducts);

    // Imprime a frequência de cada produto em relação ao total de produtos lidos
    for (int i = 1; i <= P; i++) 
    {
        double percentage = (100.0 * freqProducts[i]) / totalProducts;
        printf("[INFO] [main]: freqProducts[%d] / totalProducts = %.2f%%\n", i, percentage);
    }

    // Destrói os mutexes
    for (int i = 0; i <= P; i++) 
    {
        pthread_mutex_destroy(&mutexArray[i]);
    }

    return 0;
}