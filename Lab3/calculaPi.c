#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "timer.h"

#define DEBUG 0

typedef struct {
	int seed, len;
} Arg_t;

void* task(void *arg) {
	int seed = ((Arg_t *) arg)->seed;
	int len = ((Arg_t *) arg)->len;
	double *result = malloc(sizeof(*result));
	*result = 0;

	for (int i = seed + len; i > seed;  i--) {
		int sinal = (i&1)*2-1;
		double div = (double) (i*2 - 1) * sinal;

		*result += 4/div;
	}

	pthread_exit((void *) result);
}

int main (int argc, char **argv) {
	long long int numThreads, elem;
	double nearPi = 0;

	int offset, len, resto;
	pthread_t *tids;
	Arg_t *args;
	double *ret;

	double tstart, tfinish, tinit, telapsed;

	GET_TIME(tstart);

	// Leitura de argumentos
	if (argc < 3) {
		printf("usage: %s <n elementos> <n threads>\n", argv[0]);
	} else {
		elem = atoll(argv[1]);
		numThreads = atoll(argv[2]);
	}

	GET_TIME(tfinish);
	tinit = tfinish - tstart;

	GET_TIME(tstart);

	// Inicializa variáveis de concorrência
	tids = malloc(sizeof(*tids)*numThreads);

	offset = 0;
	len = (elem / numThreads);
	resto = elem % numThreads;

	args = malloc(sizeof(*args)*numThreads);

	// Chamar pthreads
	for (int i = 0; i < numThreads; i++) {
		args[i].seed = offset;
		offset += len;
		args[i].len = len;

		if (resto > 0) {
			args[i].len += 1;
			offset += 1;
			resto -= 1;
		}

		if (DEBUG)
			printf("--Cria a thread %d (seed = %d, len = %d)\n", i, args[i].seed, args[i].len);
		if (pthread_create(tids + i, NULL, task, (void*) (args + i))) {
			printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}

	// Espera pthreads
	for (int i = numThreads-1; i >= 0; i--) {
		if (pthread_join(tids[i], (void **) &ret)) {
			printf("--ERRO: pthread_join() \n"); exit(-1);
		}
		nearPi += *ret;
		free(ret);
	}

	// Libera variáveis de concorrência
	free(args);

	GET_TIME(tfinish);
	telapsed = tfinish - tstart;

	printf("Nosso PI:\t%.15lf\n", nearPi);
	printf("Maths PI:\t%.15lf\n", M_PI);

	printf("\n\ntime init:\t %lf \n", tinit);
	printf("time elapsed:\t %lf \n", telapsed);
	printf("time total:\t %lf \n", tinit + telapsed);

	return 0;
}
