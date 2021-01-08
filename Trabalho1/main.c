#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

typedef unsigned int uint;

#define TAMBUFFER 100

#define IS_SPACE(x)	( (x) == ' ' || (x) == '\n' )
#define IS_NUM(x)	( '0' <= (x) && (x) <= '9' )

#define DEBUG if (0)
#define DOTASK if (0)
#define PRINTOUT if (0)

#include "grafo.c"
#include "helper.c"

#include <pthread.h>
#include "timer.h"

#include "mut_arr.c"

// Variáveis Globais
mut_arr *jobPool;
int jobCount;
mut_arr *done;
int *retCodes;

uint *times;

#include "concVer.c"

#include "seqVer.c"

int main(int argc, char **argv) {
	int V, A;
	int nthreads = 0;
	int *indexes;

	if ( argc > 1 ) {
		nthreads = atoll(argv[1]);
	}

	double tstart, tfinish, telapsed;

	// Ler entrada
	grafo *graf = lerEntrada(&V, &A, &times);

	GET_TIME(tstart);

	// Organizar
	if ( nthreads > 0 ) {
		indexes = topoSortConc(graf, nthreads);
	} else {
		indexes = topoSort(graf);
	}

	GET_TIME(tfinish);
	telapsed = tfinish - tstart;

	// Mostrar saída
	PRINTOUT {
		printf("Resultado Final: ");
		for (int i = 0; i < V; i++) {
			// printf("node: %d, %ds\n", indexes[i]+1, times[ indexes[i] ]);
			printf("%d%c", indexes[i]+1, i+1<V?' ':'\n');
		}
	}

	printf("\ntime elapsed:\t %lf \n", telapsed);

	free(indexes);

	free(times);
	freeGrafo(graf);

	return 0;
}
