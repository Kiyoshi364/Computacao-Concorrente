#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define IN_FILE         "in.in"
// #define USE_STD_OUT
#define OUT_FILE        "out.out"
#define CORRECT_FILE    "out.in"

#include "help.c"

typedef struct {
    int id;
} arg_t;


// Variáveis globais
int *vec;
int len;

unsigned int dones = 0;
unsigned int waiting = 0;
pthread_mutex_t lock;
pthread_cond_t cond;

void barreira() {
    pthread_mutex_lock(&lock);
    waiting += 1;
    if ( waiting + dones < len ) {
        pthread_cond_wait(&cond, &lock);
    } else {
        waiting = 0;
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&lock);
}

void done() {
    pthread_mutex_lock(&lock);
    dones += 1;
    if ( waiting + dones >= len ) {
        waiting = 0;
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&lock);
}

void* task(void *a) {
    arg_t *arg = (arg_t *) a;
    int offset = arg->id;
    int stepBack = 1;

    while ( offset - stepBack >= 0 ) {
        // Passo 1: read and sum
        int novaSoma = vec[offset] + vec[offset - stepBack];

        // Passo 1.5: synch
        barreira();

        // Passo 2: write
        vec[offset] = novaSoma;

        // Passo 2.5: synch
        barreira();

        stepBack *= 2;
    }

    // Incrementar a contagem de terminados
    done();

    pthread_exit( NULL );
    return 0;
}

int main() {

    welcome();

    // Ler entrada
    FILE *fin = openReadFile(IN_FILE);
    len = readVec(&vec, fin);
    fclose(fin);

    pthread_t tid[len];
    arg_t args[len];

    for ( int i = 0; i < len; i++ ) {
        args[i].id = i;

        if ( pthread_create(&tid[i], NULL, task, (void *) (args+i)) ) {
            fprintf(stderr, "Erro: pthread create: %d\n", i);
            exit(1);
        }
    }

    for ( int i = 0; i < len; i++ ) {
        if ( pthread_join(tid[i], NULL) ) {
            fprintf(stderr, "Erro: pthread join: %d\n", i);
            exit(1);
        }
    }

    // Escrever a saída
#ifdef USE_STD_OUT
    writeVec(vec, len, stdout);
#else
    FILE *fout = openWriteFile(OUT_FILE);
    writeVec(vec, len, fout);
    fclose(fout);
#endif

    // Testar se a solução concorrente está certa
    FILE *fright = openReadFile(CORRECT_FILE);
    int ok = check(vec, len, fright);
    fclose(fright);

    if ( ok ) {
        puts("Tudo certo!");
    }

    free(vec);

    return 0;
}
