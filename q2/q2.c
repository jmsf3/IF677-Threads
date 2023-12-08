#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 5 // Número de arquivos
#define P 10 // Número de elementos em cada arquivo
#define T 3 // Número de threads
#define MAX_NUM_PRODUCTS 10

// Array de mutexes para exclusão mútua refinada
pthread_mutex_t mutexArray[P + 1];

// Mutex para exclusão mútua ao ler arquivos
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

// Array para armazenar as ocorrências de cada número
int occurrence[P + 1] = {0};

// Array de flags para saber quais arquivos já foram processados
int processedFiles[N] = {0};

int totalNumbers = 0;  

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
            printf("Erro ao criar arquivo\n");
            exit(1);
        }

        int *randomList = malloc(numProducts * sizeof(int));
        if (randomList == NULL) 
        {
            printf("Erro de alocação de memória\n");
            exit(1);
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

void *readNumbers(void *arg) 
{
    int threadIndex = *((int *) arg);

    for (int fileNumber = threadIndex + 1; fileNumber <= N; fileNumber += T) 
    {
        // Verifica se o arquivo já foi processado por outra thread
        pthread_mutex_lock(&fileMutex);
        if (processedFiles[fileNumber - 1] == 1) 
        {
            pthread_mutex_unlock(&fileMutex);
            continue;
        }

        // Marca o arquivo como processado
        processedFiles[fileNumber - 1] = 1;
        pthread_mutex_unlock(&fileMutex);

        char filename[10];
        sprintf(filename, "%d.in", fileNumber);

        // Mutex para exclusão mútua ao ler arquivos
        pthread_mutex_lock(&fileMutex);

        FILE *file = fopen(filename, "r");
        if (file == NULL) 
        {
            printf("Erro ao abrir o arquivo\n");
            pthread_mutex_unlock(&fileMutex);
            pthread_exit(NULL);
        }

        // Lê os números do arquivo
        int number;
        while (fscanf(file, "%d", &number) == 1) 
        {
            // Adquire o mutex correspondente à posição do array de ocorrências
            pthread_mutex_lock(&mutexArray[number]);

            // Atualiza a quantidade do produto no array de ocorrências
            occurrence[number]++;
            totalNumbers++;  // Incrementa o total de números lidos

            // Libera o mutex correspondente à posição do array de ocorrências
            pthread_mutex_unlock(&mutexArray[number]);
        }

        fclose(file);

        // Libera o mutex após ler o arquivo
        pthread_mutex_unlock(&fileMutex);
    }

    pthread_exit(NULL);
}

int main() 
{
    // Seed
    srand(time(NULL));

    createFiles();

    pthread_t threads[T];
    int threadIndexes[T];  // Índices para identificar cada thread

    // Inicializa os mutexes para cada posição do array de ocorrências
    for (int i = 0; i <= P; i++) 
    {
        pthread_mutex_init(&mutexArray[i], NULL);
    }

    // Cria as threads e verifica erros
    for (int i = 0; i < T; i++) 
    {
        threadIndexes[i] = i;
        if (pthread_create(&threads[i], NULL, readNumbers, (void *)&threadIndexes[i]) != 0) 
        {
            fprintf(stderr, "Erro ao criar a thread\n");
            exit(1);
        }
    }

    // Aguarda as threads terminarem
    for (int i = 0; i < T; i++) 
    {
        if (pthread_join(threads[i], NULL) != 0) 
        {
            fprintf(stderr, "Erro ao aguardar a thread\n");
            exit(1);
        }
    }

    // Imprime o total de números
    printf("Total de números lidos: %d\n", totalNumbers);

    // Imprime as ocorrências relativas ao total de números lidos
    printf("Ocorrência percentual de cada número em relação ao total:\n");
    for (int i = 1; i <= P; i++) 
    {
        double percent = (double) occurrence[i] / totalNumbers * 100;
        printf("Número %d: %.2f%% (Processado %d vezes)\n", i, percent, occurrence[i]);
    }

    // Destrói os mutexes
    for (int i = 0; i <= P; i++) 
    {
        pthread_mutex_destroy(&mutexArray[i]);
    }

    return 0;
}