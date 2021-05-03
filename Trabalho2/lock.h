#ifndef __LOCK__
#define __LOCK__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef unsigned int uint;

typedef struct {
    uint leit_usando;
    uint escr_usando;

    uint leit_esperando;
    uint escr_esperando;

    pthread_mutex_t lock;
    pthread_cond_t leitura;
    pthread_cond_t escrita;
} lock_t;

lock_t* new_lock();
void lock_leitura(lock_t *lock, int id);
void unlock_leitura(lock_t *lock, int id);
void lock_escrita(lock_t *lock, int id);
void unlock_escrita(lock_t *lock, int id);

#endif // __LOCK__
