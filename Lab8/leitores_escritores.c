#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define LTHREADS   2
#define ETHREADS   2

#define ROUNDS     2

// sem_t escr;
pthread_mutex_t lock_e;
sem_t leit, escr;

int e = 0, l = 0;

void* le(void* vid) {
    int id = *((int *) vid);
    printf("Começando leitor %d.\n", id);
    int i = ROUNDS;
    while ( i-- ) {
        printf("Leitor %d: quer ler\n", id);

        sem_wait(&leit);
        l += 1;
        if ( l == 1 ) sem_wait(&escr);
        sem_post(&leit);

        printf("Leitor %d: está lendo\n", id);

        sem_wait(&leit);
        l -= 1;
        if ( l == 0 ) sem_post(&escr);
        sem_post(&leit);

        printf("Leitor %d: terminou de ler\n", id);
    }
    return NULL;
}

void* escreve(void* vid) {
    int id = *((int *) vid);
    printf("Começando escritor %d.\n", id);
    int i = ROUNDS;
    while ( i-- ) {
        printf("Escritor %d: quer escrever\n", id);

        pthread_mutex_lock(&lock_e);
        e += 1;
        if ( e == 1 ) sem_wait(&leit);
        pthread_mutex_unlock(&lock_e);

        sem_wait(&escr);
        printf("Escritor %d: está escrevendo\n", id);
        sem_post(&escr);

        pthread_mutex_lock(&lock_e);
        e -= 1;
        if ( e == 0 ) sem_post(&leit);
        pthread_mutex_unlock(&lock_e);

        printf("Escritor %d: terminou de escrever\n", id);
    }
    return NULL;
}

int main() {
    pthread_t le_tid[LTHREADS];
    pthread_t es_tid[ETHREADS];

    int le_id[LTHREADS];
    int es_id[ETHREADS];

    pthread_mutex_init(&lock_e, NULL);
    sem_init(&escr, 0, 1);
    sem_init(&leit, 0, 1);

    for(int i = 0; i < LTHREADS; i++) {
        le_id[i] = i;
        if (pthread_create(&le_tid[i], NULL, le, le_id+i)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    for(int i = 0; i < ETHREADS; i++) {
        es_id[i] = i;
        if (pthread_create(&es_tid[i], NULL, escreve, es_id+i)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    pthread_exit(NULL);
}
