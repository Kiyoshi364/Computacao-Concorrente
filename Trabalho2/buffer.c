typedef struct {
    Arg_t *buf;
    int len;
    int init, end;

    int nThreads;

    pthread_cond_t reading;
    pthread_cond_t writing;

    pthread_mutex_t readLock;
    pthread_mutex_t writeLock;
} Buffer_t;

Buffer_t* newBuffer(int len, int nthreads) {
    Buffer_t *b = malloc(sizeof(*b));
    if ( !b ) {
        puts("Sem memória: Buffer_t");
        exit(1);
    }

    b->buf = malloc(sizeof(*b->buf)*len);
    if ( !b ) {
        puts("Sem memória: Buffer_t (b->buf)");
        exit(1);
    }

    b->len = len;
    b->init = 0;
    b->end = 0;
    b->nThreads = nthreads;

    pthread_mutex_init(&(b->readLock), NULL);
    pthread_mutex_init(&(b->writeLock), NULL);

    pthread_cond_init(&(b->reading), NULL);
    pthread_cond_init(&(b->writing), NULL);

    return b;
}

void freeBuffer(Buffer_t *b) {
    free(b->buf);
    free(b);
}

void bufferWrite(Buffer_t *b, int *vec, int tam) {
    LOG puts("quer escrever");
    pthread_mutex_lock(&b->writeLock);

    while ( b->init == (b->end + 1) % b->len ) {
        pthread_cond_wait(&b->writing, &b->writeLock);
    }

    b->buf[b->end].vec = vec;
    b->buf[b->end].tam = tam;
    b->end = (b->end + 1) % b->len;

    pthread_cond_signal(&b->reading);

    pthread_mutex_unlock(&b->writeLock);
    LOG puts("escreveu");
}

void bufferRead(Buffer_t *b, Arg_t *arg) {
    arg->vec = NULL;
    arg->tam = 0;

    LOG puts("quer ler");
    pthread_mutex_lock(&b->readLock);

    b->nThreads -= 1;
    while ( b->init == b->end ) {
        if ( b->nThreads == 0 ) {
            pthread_cond_broadcast(&b->reading);
            LOG puts("avisou que acabou");
            pthread_mutex_unlock(&b->readLock);
            return;
        }
        pthread_cond_wait(&b->reading, &b->readLock);
        if ( b->nThreads == 0 ) {
            LOG puts("notificado que acabou");
            pthread_mutex_unlock(&b->readLock);
            return;
        }
    }
    b->nThreads += 1;

    arg->vec = b->buf[b->init].vec;
    arg->tam = b->buf[b->init].tam;
    b->init = (b->init + 1) % b->len;

    pthread_cond_signal(&b->writing);

    pthread_mutex_unlock(&b->readLock);
    LOG puts("leu");
}
