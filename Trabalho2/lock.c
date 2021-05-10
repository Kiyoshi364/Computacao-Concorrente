#include "lock.h"

// Funções auxiliares
// internal significa que não vai ser exportada para ser linkada
#define internal static

internal
void lockA(lock_t *lock) {
    pthread_mutex_lock(&(lock->lock));
}

internal
void unlockA(lock_t *lock) {
    pthread_mutex_unlock(&(lock->lock));
}

internal
void block_leitura(lock_t *lock) {
    pthread_cond_wait(&(lock->leitura), &(lock->lock));
}

internal
void block_escrita(lock_t *lock) {
    pthread_cond_wait(&(lock->escrita), &(lock->lock));
}

internal
void broadcast_leitores(lock_t *lock) {
    pthread_cond_broadcast(&(lock->leitura));
}

internal
void signal_escritor(lock_t *lock) {
    pthread_cond_signal(&(lock->escrita));
}

#undef internal

// Funções da lib

lock_t* new_lock() {
    lock_t *lock = malloc(sizeof(*lock));
    if ( !lock ) {
        fprintf(stderr, "erro: new_lock malloc\n");
        exit(1);
    }

    lock->leit_usando = 0;
    lock->escr_usando = 0;

    lock->leit_esperando = 0;
    lock->escr_esperando = 0;

    pthread_mutex_init(&(lock->lock), NULL);
    pthread_cond_init(&(lock->leitura), NULL);
    pthread_cond_init(&(lock->escrita), NULL);

    return lock;
}

void lock_leitura(lock_t *lock, int id) {
    lockA(lock);

    printf("leit %d: quer ler\n", id);

    lock->leit_esperando += 1;
    while ( lock->escr_esperando > 0 || lock->escr_usando ) {
        printf("leit %d: bloqueado\n", id);
        block_leitura(lock);
        printf("leit %d: desbloqueado\n", id);
    }
    lock->leit_esperando -= 1;

    lock->leit_usando += 1;

    printf("leit %d: esta lendo\n", id);

    unlockA(lock);
}

void unlock_leitura(lock_t *lock, int id) {
    lockA(lock);

    printf("leit %d: terminou de ler\n", id);

    lock->leit_usando -= 1;

    if ( lock->leit_usando <= 0 ) {
        if ( lock->escr_esperando > 0 ) {
            printf("leit %d: sinaliza escritor\n", id);
            signal_escritor(lock);
        } else {
            printf("leit %d: broadcast leitores\n", id);
            broadcast_leitores(lock);
        }
    }

    unlockA(lock);
}

void lock_escrita(lock_t *lock, int id) {
    lockA(lock);

    printf("escr %d: quer escrever\n", id);

    lock->escr_esperando += 1;
    while ( lock->leit_usando || lock->escr_usando ) {
        printf("escr %d: bloqueado\n", id);
        block_escrita(lock);
        printf("escr %d: desbloqueado\n", id);
    }
    lock->escr_esperando -= 1;

    lock->escr_usando += 1;

    printf("escr %d: esta escrevendo\n", id);

    unlockA(lock);
}

void unlock_escrita(lock_t *lock, int id) {
    lockA(lock);

    printf("escr %d: terminou de escrever\n", id);

    lock->escr_usando -= 1;

    if ( lock->escr_esperando > 0 ) {
        printf("escr %d: sinaliza escritor\n", id);
        signal_escritor(lock);
    } else {
        printf("escr %d: broadcast leitores\n", id);
        broadcast_leitores(lock);
    }

    unlockA(lock);
}
