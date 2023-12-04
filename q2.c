#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TRUE 1
#define MAX 10

struct thread 
{
    FILE *file;
    pthread_mutex_t *mutexArray;
    int *productCount;
    int *fileIndex;
    int N;
};

void *processFile(void *arg) {
    //converte o ponteiro de volta para struct
    struct thread *data = (struct thread *)arg;

    int product;

    while(TRUE) 
    {
        //Trava o mutex que controla o acesso aos arquivos
        pthread_mutex_lock(&data->mutexArray[data->N]);

        //Pega o indice do arquivo a ser lido
        int index = *data->fileIndex;

        (*data->fileIndex)++;

        //Libera o mutex
        pthread_mutex_unlock(&data->mutexArray[data->N]);

        //Verificar se todos os arquivos ja foram lidos
        if(index >= data->N)
            break;

        FILE *file = data->file[index];
        char buffer[100];
        while(fgets(buffer, sizeof(buffer), file) != NULL)
        {
            product = atoi(buffer);
            pthread_mutex_lock(&data->mutexArray[product]);
            data->productCount[product]++;
            pthread_mutex_unlock(&data->mutexArray[product]);
        }

        fclose(file);

    }

    pthread_exit(NULL);
}


int main()
{
    int N; //Arquivos
    int T; //Threads
    int P; //Produtos

    //Inicialiar array de mutex e o de arquivos
    pthread_mutex_t mutexArray[MAX + 1];
    FILE *fileArray[MAX + 1];
    for(int i = 0; i <= MAX; i++) 
    {
        pthread_mutex_init(&mutexArray[i], NULL);
    }

    int countProducts[MAX] = {0};

    //Inicialixa arrar de nomes dos arquivos
    char fileNames[MAX][10];
    for(int i = 1; i <= MAX; i++)
    {
        sprintf(fileNames[i-1], "%d.in", i);
    }

    //Array de threads
    pthread_t threads[T];

    // Inicializar a estrutura de dados para cada thread
    struct thread thread_data[T];
    for (int i = 0; i < T; ++i) {
        thread_data[i].file = fileArray;
        thread_data[i].mutexArray = mutexArray;
        thread_data[i].productCount = countProducts;
        thread_data[i].fileIndex = &i; // Cada thread inicia com um índice diferente
        thread_data[i].N = N;

        // Abrir os arquivos e adicionar ao array
        for (int j = 0; j < N; ++j) {
            FILE *file = fopen(fileNames[j], "r");
            if (file == NULL) {
                perror("Erro ao abrir o arquivo");
                return 1;
            }
            fileArray[j] = file;
        }

        // Criar a thread e verificar por erros
        int status = pthread_create(&threads[i], NULL, processFile, (void *)&thread_data[i]);
        if(status != 0)
        {
            printf("ERROR: pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    //Aguardar a conclusão de todas as threads
    for (int i = 0; i < T; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Total de produtos lidos: %d\n", P);
    printf("Percentual de cada tipo de produto vendido:\n");

    for (int i = 0; i < P; ++i) {
        int count = countProducts[i];
        float percentage = (float)count / (float)P * 100;
        printf("Produto %d: %.2f%%\n", i, percentage);
    }

    //Fechar todos os arquivos
    for (int i = 0; i < N; ++i) {
        fclose(fileArray[i]);
    }

    //Destruir mutexes
    for (int i = 0; i <= MAX; ++i) {
        pthread_mutex_destroy(&mutexArray[i]);
    }

    return 0;

}