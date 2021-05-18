#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS  4

/* Variaveis globais */
sem_t sem1;
sem_t sem2;

/* Thread 1 */
void *A (void *t) {

    // Esperar todas as tarefas acabarem
    sem_wait(&sem2);
    sem_wait(&sem2);

    // Fazer tarefa
    puts("Volte sempre!");

    pthread_exit(NULL);
}

/* Thread 2 */
void *B (void *t) {

    // Esperar tarefa 4 acabar
    sem_wait(&sem1);

    // Fazer tarefa
    puts("Fique a vontade.");

    // Avisar que tarefa 2 está feita
    sem_post(&sem2);

    pthread_exit(NULL);
}

/* Thread 3 */
void *C (void *t) {

    // Esperar tarefa 4 acabar
    sem_wait(&sem1);

    // Fazer tarefa
    puts("Sente-se por favor.");

    // Avisar que tarefa 3 está feita
    sem_post(&sem2);

    pthread_exit(NULL);
}

/* Thread 4 */
void *D (void *t) {
    // Fazer tarefa
    puts("Seja bem-vindo!");

    // Avisar que tarefa 4 está feita
    sem_post(&sem1);
    sem_post(&sem1);

    pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
    int i; 
    pthread_t threads[NTHREADS];

    /* Inicilaiza os semaphoros */
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    /* Cria as threads */
    if ( pthread_create(&threads[3], NULL, A, NULL) ) {
        fprintf(stderr, "Erro: não foi possível criar thread A\n");
        return 1;
    }
    if ( pthread_create(&threads[2], NULL, B, NULL) ) {
        fprintf(stderr, "Erro: não foi possível criar thread B\n");
        return 2;
    }
    if ( pthread_create(&threads[1], NULL, C, NULL) ) {
        fprintf(stderr, "Erro: não foi possível criar thread C\n");
        return 3;
    }
    if ( pthread_create(&threads[0], NULL, D, NULL) ) {
        fprintf(stderr, "Erro: não foi possível criar thread D\n");
        return 4;
    }

    /* Espera todas as threads completarem */
    for (i = 0; i < NTHREADS; i++) {
      pthread_join(threads[i], NULL);
    }

    /* Desaloca variaveis e termina */
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}
