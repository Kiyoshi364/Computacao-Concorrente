/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: Multiplicar Matrizes de forma concorrente */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "timer.h"

#define DEBUG			0
#define PRINT_SAIDA		0

#define INDEX(x, y, l)	((x) + ((y)*(l)))

int *A, *B, *C;

typedef struct {
	int start, len, linha;
} Arg_t;

void* task(void *arg) {
	int start = ((Arg_t *) arg)->start;
	int len = ((Arg_t *) arg)->len;
	int linha = ((Arg_t *) arg)->linha;

	for (int i = 0; i < len; i++) {
		int x = (start + i) % linha;
		int	y = (start + i) / linha;

		for (int k = 0; k < linha; k++) {
			C[start+i] += A[INDEX(k, y, linha)] * B[INDEX(x, k, linha)];
		}
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	int numThreads, linhas, colunas;

	int offset, len, resto;
	pthread_t *tids;
	Arg_t *args;

	double tstart, tfinish, tinit, telapsed, tclean;

	GET_TIME(tstart);

	printf("Número de threads: ");
	scanf("%d", &numThreads);

	printf("Linhas e colunas: ");
	scanf("%d %d", &linhas, &colunas);

	// Alloca espaço para matrizes
	A = malloc(sizeof(*A)*linhas*colunas);
	B = malloc(sizeof(*B)*linhas*colunas);
	C = malloc(sizeof(*C)*linhas*colunas);

	// Inicializa A, B e C
	for (int i = 0; i < linhas * colunas; i++) {
		A[i] = i;
		B[i] = linhas * colunas - i;
		C[i] = 0;
	}

	GET_TIME(tfinish);

	tinit = tfinish - tstart;

	GET_TIME(tstart);

	// Inicializa variáveis de concorrência
	tids = malloc(sizeof(*tids)*numThreads);

	offset = 0;
	len = (linhas / numThreads) * colunas;
	resto = linhas % numThreads;

	args = malloc(sizeof(*args)*numThreads);

	// Chamar pthreads
	for (int i = 0; i < numThreads; i++) {
		args[i].start = offset;
		offset += len;
		args[i].len = len;
		args[i].linha = colunas;

		if (resto > 0) {
			args[i].len += colunas;
			offset += colunas;
			resto -= 1;
		}

		if (DEBUG)
			printf("--Cria a thread %d (start = %d, len = %d)\n", i, args[i].start, args[i].len);
		if (pthread_create(tids + i, NULL, task, (void*) (args + i))) {
			printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}

	// Espera pthreads
	for (int i = 0; i < numThreads; i++) {
		if (pthread_join(tids[i], NULL)) {
			printf("--ERRO: pthread_join() \n"); exit(-1);
		}
	}

	// Libera variáveis de concorrência
	free(args);

	GET_TIME(tfinish);

	telapsed = tfinish - tstart;

	GET_TIME(tstart);

	if (PRINT_SAIDA)
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			printf("%d%c", C[INDEX(j, i, colunas)], j+1>=colunas?'\n':'\t');
		}
	}

	// Libera espaço
	free(A);
	free(B);
	free(C);

	GET_TIME(tfinish);

	tclean = tfinish - tstart;

	printf("\n\ntime init:\t %lf \n", tinit);
	printf("time elapsed:\t %lf \n", telapsed);
	printf("time clean up:\t %lf \n", tclean);
	printf("time total:\t %lf \n", tinit + telapsed + tclean);

	return 0;
}
