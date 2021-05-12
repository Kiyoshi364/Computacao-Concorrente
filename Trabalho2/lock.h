#ifndef __LOCK__
#define __LOCK__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef unsigned int uint;

typedef struct {
    // Quantos leitores/escritores estão usando o recurso
    uint leit_usando;
    uint escr_usando;

    // Quantos leitores/escritores estão na fila de espera
    uint leit_esperando;
    uint escr_esperando;

    // Mutex para alterar os contadores
    pthread_mutex_t lock;

    // Fila de espera para leitores/escritores
    pthread_cond_t leitura;
    pthread_cond_t escrita;
} lock_t;

void init_lock(lock_t*);
void lock_leitura(lock_t *lock, int id);
void unlock_leitura(lock_t *lock, int id);
void lock_escrita(lock_t *lock, int id);
void unlock_escrita(lock_t *lock, int id);

#endif // __LOCK__
