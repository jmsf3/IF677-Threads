#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5 // Número de arquivos
#define P 5 // Número de produtos
#define T 3 // Número de threads
#define MAX_NUM_PRODUCTS 10 // Número máximo de produtos por arquivo

// Array de mutexes para exclusão mútua refinada
pthread_mutex_t mutexArray[P + 1];

// Array para armazenar as ocorrências de cada produto
int occurrence[P + 1] = {0};

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
            printf("Erro ao criar arquivo\n");
            exit(-1);
        }

        int *randomList = malloc(numProducts * sizeof(int));
        if (randomList == NULL) 
        {
            printf("Erro de alocação de memória\n");
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

void *readNumbers(void *arg) 
{
    int threadIndex = *(int *)arg;

    for (int fileNumber = threadIndex + 1; fileNumber <= N; fileNumber += T) 
    {
        char filename[10];
        sprintf(filename, "%d.in", fileNumber);

        FILE *file = fopen(filename, "r");
        if (file == NULL) 
        {
            ("Erro ao abrir o arquivo\n");
            exit(-1);
        }

        // Lê os produtos do arquivo
        int number;
        while (fscanf(file, "%d", &number) == 1) 
        {
            // Adquire o mutex correspondente à posição do array de ocorrências
            pthread_mutex_lock(&mutexArray[number]);

            // Atualiza a quantidade do produto no array de ocorrências
            occurrence[number]++;
            totalProducts++;  // Incrementa o total de produtos lidos

            // Libera o mutex correspondente à posição do array de ocorrências
            pthread_mutex_unlock(&mutexArray[number]);
        }

        fclose(file);
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
            exit(-1);
        }
    }

    // Aguarda as threads terminarem
    for (int i = 0; i < T; i++) 
    {
        if (pthread_join(threads[i], NULL) != 0) 
        {
            fprintf(stderr, "Erro ao aguardar a thread\n");
            exit(-1);
        }
    }

    // Imprime o total de números
    printf("Total de produtos lidos: %d\n", totalProducts);

    // Imprime as ocorrências relativas ao total de números lidos
    printf("Ocorrência percentual de cada produto em relação ao total:\n");
    for (int i = 1; i <= P; i++) 
    {
        double percent = (double)occurrence[i] / totalProducts * 100;
        printf("Produto %d: %.2f%% (Processado %d vezes)\n", i, percent, occurrence[i]);
    }

    // Destrói os mutexes
    for (int i = 0; i <= P; i++) 
    {
        pthread_mutex_destroy(&mutexArray[i]);
    }

    return 0;
}