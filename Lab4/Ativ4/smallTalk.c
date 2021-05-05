/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 4 */
/* Codigo: Uso de variáveis de condição e suas operações básicas para sincronização por condição */

/*****
 * Thread 1 deve imprimir "Volte sempre!", após as threads 2, 3 e 4
 * Thread 2 deve imprimir "Fique a vontade.", após a thread 4
 * Thread 3 deve imprimir "Sente-se por favor.", após a thread 4
 * Thread 4 deve imprimir "Seja bem-vindo!"
 *****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  4

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

/* Thread 1 */
void *A (void *t) {

    // Esperar todas as tarefas acabarem
    pthread_mutex_lock(&x_mutex);
    while (x < 7) {
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);

    // Fazer tarefa
    puts("Volte sempre!");

    pthread_exit(NULL);
}

/* Thread 2 */
void *B (void *t) {

    // Esperar tarefa 4 acabarem
    pthread_mutex_lock(&x_mutex);
    // Não tem problema ser if, pois o 1o broadcast garante que x == 4
    if (x | 4 != 4) {
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);

    // Fazer tarefa
    puts("Fique a vontade.");

    // Avisar que tarefa 2 está feita
    pthread_mutex_lock(&x_mutex);
    x |= 2;
    // Se tarefa 3 também está feita, avisa para todo mundo (no caso so o A)
    if (x == 7) {
        // Também poderia ser signal
        pthread_cond_broadcast(&x_cond);
    }
    pthread_mutex_unlock(&x_mutex); 

    pthread_exit(NULL);
}

/* Thread 3 */
void *C (void *t) {

    // Esperar tarefa 4 acabarem
    pthread_mutex_lock(&x_mutex);
    // Não tem problema ser if, pois o 1o broadcast garante que x == 4
    if (x | 4 != 4) {
       pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); 

    // Fazer tarefa
    puts("Sente-se por favor.");

    // Avisar que tarefa 3 está feita
    pthread_mutex_lock(&x_mutex);
    x |= 1;
    // Se tarefa 2 também está feita, avisa para todo mundo (no caso so o A)
    if (x == 7) {
        // Também poderia ser signal
        pthread_cond_broadcast(&x_cond);
    }
    pthread_mutex_unlock(&x_mutex); 

    pthread_exit(NULL);
}

/* Thread 4 */
void *D (void *t) {
    // Fazer tarefa
    puts("Seja bem-vindo!");

    // Avisar que tarefa 4 está feita
    pthread_mutex_lock(&x_mutex);
    x |= 4;
    pthread_cond_broadcast(&x_cond);
    pthread_mutex_unlock(&x_mutex); 

    pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
    int i; 
    pthread_t threads[NTHREADS];

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);

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
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);

    return 0;
}
