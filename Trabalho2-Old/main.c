#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../timer.h"

#define THREADS 2
#define TAMBUFFER 100

#define LOG                 if ( 0 )
#define PRINT_ANTES         if ( 0 )
#define PRINT_DEPOIS        if ( 0 )

typedef struct {
    int *vec;
    int tam;
} Arg_t;

#include "buffer.c"

void* task( void *arg ) {
    Buffer_t *buf = (Buffer_t *) arg;
    Arg_t tempVec;

    LOG puts("Nova thread");
    while ( 1 ) {
        bufferRead(buf, &tempVec);

        if ( tempVec.vec == NULL ) {
            break;
        } else if ( tempVec.tam <= 1 ) {
            LOG puts("Trabalho vazio");
            continue;
        }
        LOG puts("Novo trabalho");

        int *vec = tempVec.vec;
        int tam = tempVec.tam;
        int i = 1, j = tam-1;

        // Ordena
        while ( i < j ) {
            while ( vec[i] < vec[0] && i < tam ) {
                i += 1;
            }

            while ( vec[0] < vec[j] && j > 0 ) {
                j -= 1;
            }

            if ( i < j ) {
                int temp = vec[i];
                vec[i] = vec[j];
                vec[j] = temp;
            }
        }

        int temp = vec[0];
        vec[0] = vec[j];
        vec[j] = temp;

        if ( 0 < j ) {
            if ( bufferWrite(buf, vec, j) < 0 ) {
                return (void*) -1;
            }
        }
        if ( i < tam ) {
            if ( bufferWrite(buf, vec+i, tam-i) < 0 ) {
                return (void*) -1;
            }
        }
        LOG puts("Fim trabalho");
    }
    LOG puts("Fim thread");

    return NULL;
}

void readVec( int *v, int tam ) {
    for (int i = 0; i < tam; i++) {
        scanf("%d", v+i);
    }
}

int main(int argc, char **argv) {
	int numThreads, tamVec;
	int tamBuffer;

	double tstart, tfinish, telapsed;

	// Leitura de argumentos
	if (argc < 2) {
		printf("usage: %s <n threads>\n", argv[0]);
        return 0;
	} else {
		numThreads = atoll(argv[1]);
	}
    scanf("%d", &tamVec);

    tamBuffer = tamVec;

    int vec[tamVec];
    readVec(vec, tamVec);

	pthread_t *tids = malloc(sizeof(*tids)*numThreads);
	Buffer_t *buffer = newBuffer(tamBuffer, numThreads);

    bufferWrite(buffer, vec, tamVec);

    PRINT_ANTES {
        for (int i = 0; i < tamVec; i++) {
            printf("%d ", vec[i]);
        }
        puts("");
    }

	GET_TIME(tstart);
    
	// Chamar pthreads
	for (int i = 0; i < numThreads; i++) {

		if (pthread_create(tids + i, NULL, task, (void *) buffer)) {
			printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}

	// Espera pthreads
	for (int i = 0; i < numThreads; i++) {
        void *err;
		if (pthread_join(tids[i], &err)) {
			printf("--ERRO: pthread_join() \n"); exit(-1);
		}
        if ( err == (void*) -1 ) {
            printf("Erro: Buffer muito pequeno\n");
        }
	}

	GET_TIME(tfinish);
	telapsed = tfinish - tstart;

    PRINT_DEPOIS {
        for (int i = 0; i < tamVec; i++) {
            printf("%d ", vec[i]);
        }
        puts("");
    }

	printf("time elapsed:\t %lf \n", telapsed);

    return 0;
}
