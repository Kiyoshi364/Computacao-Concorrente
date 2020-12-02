/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: Incrementar um vetor de forma concorrente */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

// Pode alterar os defines
#define NTHREADS	2		// total de threads a serem criadas
#define VLEN		15		// tamanho total do vetor de int

// cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
	int *vec;
	int len;
} t_Args;

// funcao executada pelas threads
void *task (void *arg) {
	t_Args *input = (t_Args *) arg;

	int *vec = input->vec; // inicio da sua parte do vetor
	int i; // varialvel auxiliar

	// loop para incrementar a sua parte do vetor
	for (i = 0; i < input->len; i++) {
		vec[i] += 1;
	}

	free(arg); // aqui pode liberar a alocacao feita na main

	pthread_exit(NULL);
}

// funcao principal do programa
int main() {
	pthread_t tid_sistema[NTHREADS];	// identificadores das threads no sistema
	int thread;							// variavel auxiliar
	t_Args *arg;						// receberá os argumentos para a thread
	int vec[VLEN];						// vetor que vai ser incrementado
	int i, offset, len, resto;			// variaveis auxiliares

	printf("Vetor antes do incremento:\n");
	for(i = 0; i < VLEN; i++) {
		vec[i] = i;
		printf("%d%c", vec[i], i+1>=VLEN?'\n':' ');
	}

	// Divide os espaços
	offset = 0;
	len = VLEN / NTHREADS;
	resto = VLEN % NTHREADS;

	for(thread=0; thread<NTHREADS; thread++) {
		printf("--Aloca e preenche argumentos para thread %d\n", thread);
		arg = malloc(sizeof(*arg));
		if (arg == NULL) {
			printf("--ERRO: malloc()\n"); exit(-1);
		}
		arg->vec = vec + offset; 
	 	offset += len;
		arg->len = len; 
		if (resto > 0) {
			arg->len += 1;
			offset += 1;
			resto -= 1;
		}
		
		printf("--Cria a thread %d (len = %d)\n", thread, arg->len);
		if (pthread_create(&tid_sistema[thread], NULL, task, (void*) arg)) {
			printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}

	// --espera todas as threads terminarem
	for (thread=0; thread<NTHREADS; thread++) {
		if (pthread_join(tid_sistema[thread], NULL)) {
			printf("--ERRO: pthread_join() \n"); exit(-1); 
		} 
	}

	printf("Vetor depois do incremento:\n");
	for(i = 0; i < VLEN; i++) {
		printf("%d%c", vec[i], i+1>=VLEN?'\n':' ');
	}

	printf("--Thread principal terminou\n");
	pthread_exit(NULL);
}
