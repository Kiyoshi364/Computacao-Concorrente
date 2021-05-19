#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../lock.h"

typedef struct {
    int id;
} arg_t;

lock_t *lock;

void* escritor(void* a) {
    arg_t *arg = (arg_t *) a;
    int id = arg->id;
    while (1) {
        lock_escrita(lock, id);
        sleep(1);
        unlock_escrita(lock, id);
        sleep(5);
    }

    pthread_exit(NULL);
}

void* leitor(void* a) {
    arg_t *arg = (arg_t *) a;
    int id = arg->id;
    while (1) {
        lock_leitura(lock, id);
        sleep(1);
        unlock_leitura(lock, id);
        sleep(5);
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if ( argc < 2 ) {
        printf("usage: %s <nthreads>\n", argv[0]);
        return 0;
    }

    int nthreads = atol(argv[1]);

    // Inicializar um monte de coisa
    pthread_t *tescr;
    pthread_t *tleit;
    arg_t *args;

    tescr = malloc(sizeof(*tescr)*nthreads);
    if ( !tescr ) {
        fprintf(stderr, "Erro malloc: tescr\n");
        return 1;
    }

    tleit = malloc(sizeof(*tleit)*nthreads);
    if ( !tleit ) {
        fprintf(stderr, "Erro malloc: tleit\n");
        return 1;
    }

    args = malloc(sizeof(*args)*nthreads);
    if ( !args ) {
        fprintf(stderr, "Erro malloc: args\n");
        return 1;
    }

    lock_t LOCK;
    init_lock(&LOCK);
    lock = &LOCK;

    // Chamar as threads
    for ( int i = 0; i < nthreads; i += 1 ) {
        args[i].id = i;

        if ( pthread_create(&tescr[i], NULL, escritor, &args[i]) ) {
            fprintf(stderr, "Erro pthread-create escritor: i = %d\n", i);
        }
        if ( pthread_create(&tleit[i], NULL, leitor, &args[i]) ) {
            fprintf(stderr, "Erro pthread-create leitor: i = %d\n", i);
        }
    }

    while (1) ;

    return 0;
}
