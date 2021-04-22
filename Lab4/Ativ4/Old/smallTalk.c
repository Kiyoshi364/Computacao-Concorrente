/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 4 */
/* Codigo: Uso de variáveis de condição e suas operações básicas para sincronização por condição */

/*****
 * Thread 1 deve imprimir "tudo bem?"
 * Thread 2 deve imprimir "bom dia!"
 * Thread 3 deve imprimir "até mais!", após as threads 1 e 2
 * Thread 4 deve imprimir "boa tarde!", após as threads 1 e 2
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
  // Fazer tarefa
  printf("tudo bem?\n");

  // Avisar que tarefa 1 está feita, se tarefa 2 também está feita
  pthread_mutex_lock(&x_mutex);
  x |= 1;
  if (x == 3) {
      pthread_cond_broadcast(&x_cond);
  }
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

/* Thread 2 */
void *B (void *t) {
  // Fazer tarefa
  printf("bom dia!\n");

  // Avisar que tarefa 2 está feita, se tarefa 1 também está feita
  pthread_mutex_lock(&x_mutex);
  x |= 2;
  if (x == 3) {
      pthread_cond_broadcast(&x_cond);
  }
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

/* Thread 3 */
void *C (void *t) {
  // Esperar tarefa 1 e 2 acabarem
  pthread_mutex_lock(&x_mutex);
  if (x != 3) { 
     pthread_cond_wait(&x_cond, &x_mutex);
  }
  pthread_mutex_unlock(&x_mutex); 

  // Fazer tarefa
  printf("até mais!\n");

  pthread_exit(NULL);
}

/* Thread 3 */
void *D (void *t) {
  // Esperar tarefa 1 e 2 acabarem
  pthread_mutex_lock(&x_mutex);
  if (x != 3) { 
     pthread_cond_wait(&x_cond, &x_mutex);
  }
  pthread_mutex_unlock(&x_mutex); 

  // Fazer tarefa
  printf("boa tarde!\n");

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
  pthread_create(&threads[3], NULL, A, NULL);
  pthread_create(&threads[2], NULL, B, NULL);
  pthread_create(&threads[1], NULL, C, NULL);
  pthread_create(&threads[0], NULL, D, NULL);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  printf ("\nFIM\n");

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);

  return 0;
}
