typedef struct {
    Arg_t *buf;
    int len;
    int init, end;

    int rThreads;
    int wThreads;

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

    b->rThreads = nthreads;
    b->wThreads = nthreads;

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

int bufferWrite(Buffer_t *b, int *vec, int tam) {
    LOG puts("quer escrever");
    pthread_mutex_lock(&b->writeLock);

    b->wThreads -= 1;
    while ( b->init == (b->end + 1) % b->len ) {
        if ( b->wThreads == 0 ) {
            pthread_cond_broadcast(&b->writing);
            LOG puts("avisou que buffer está cheio");
            pthread_mutex_unlock(&b->writeLock);
            return -1;
        }
        pthread_cond_wait(&b->writing, &b->writeLock);
        if ( b->wThreads == 0 ) {
            LOG puts("notificado que buffer está cheio");
            pthread_mutex_unlock(&b->writeLock);
            return -1;
        }
    }
    b->wThreads += 1;

    b->buf[b->end].vec = vec;
    b->buf[b->end].tam = tam;
    b->end = (b->end + 1) % b->len;

    pthread_cond_signal(&b->reading);

    pthread_mutex_unlock(&b->writeLock);
    LOG puts("escreveu");
    return 0;
}

void bufferRead(Buffer_t *b, Arg_t *arg) {
    arg->vec = NULL;
    arg->tam = 0;

    LOG puts("quer ler");
    pthread_mutex_lock(&b->readLock);

    b->rThreads -= 1;
    while ( b->init == b->end ) {
        if ( b->rThreads == 0 ) {
            pthread_cond_broadcast(&b->reading);
            LOG puts("avisou que acabou");
            pthread_mutex_unlock(&b->readLock);
            return;
        }
        pthread_cond_wait(&b->reading, &b->readLock);
        if ( b->rThreads == 0 ) {
            LOG puts("notificado que acabou");
            pthread_mutex_unlock(&b->readLock);
            return;
        }
    }
    b->rThreads += 1;

    arg->vec = b->buf[b->init].vec;
    arg->tam = b->buf[b->init].tam;
    b->init = (b->init + 1) % b->len;

    pthread_cond_signal(&b->writing);

    pthread_mutex_unlock(&b->readLock);
    LOG puts("leu");
}
