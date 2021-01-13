typedef struct {
	int *arr;
	int len;

	int r;
	int w;

	pthread_mutex_t rlock;
	pthread_mutex_t wlock;

	pthread_cond_t rcond;
} mut_queue;

mut_queue* newMutQueue(int len) {
	mut_queue *mq = malloc(sizeof(*mq));
	if ( !mq ) {
		puts("newMutQueue: Out of Memory (mut_queue).");
		exit(1);
	}

	len = len > 0 ? len : 1;

	mq->len = len;
	mq->arr = malloc(sizeof(*mq->arr)*len);
	if ( !(mq->arr) ) {
		puts("newMutQueue: Out of Memory (mut_queue->arr).");
		exit(1);
	}

	mq->r = 0;
	mq->w = 0;

	pthread_mutex_init(&mq->rlock, NULL);
	pthread_mutex_init(&mq->wlock, NULL);

	pthread_cond_init(&mq->rcond, NULL);

	return mq;
}

void freeMutQueue(mut_queue *mq) {
	free(mq->arr);
	pthread_mutex_destroy(&mq->rlock);
	pthread_mutex_destroy(&mq->wlock);
	free(mq);
}

void lockReadMutQueue(mut_queue *mq) {
	pthread_mutex_lock(&mq->rlock);
}

void unlockReadMutQueue(mut_queue *mq) {
	pthread_mutex_unlock(&mq->rlock);
}

void waitReadMutQueue(mut_queue *mq) {
	pthread_cond_wait(&mq->rcond, &mq->rlock);
}

void signalReadMutQueue(mut_queue *mq) {
	pthread_cond_signal(&mq->rcond);
}

void broadcastReadMutQueue(mut_queue *mq) {
	pthread_cond_broadcast(&mq->rcond);
}

int readMutQueue(mut_queue *mq) {
	int index = -1;

	lockReadMutQueue(mq);

	if ( mq->r < mq->w ) {
		index = mq->r;
		mq->r += 1;
	}

	unlockReadMutQueue(mq);

	return index;
}

int readBlockingMutQueue(mut_queue *mq) {
	int index = -1;

	lockReadMutQueue(mq);

	while ( !(mq->r < mq->w) ) {
		waitReadMutQueue(mq);

		// Se não tiver mais nada para ler
		if ( mq->r >= mq->len ) {
			unlockReadMutQueue(mq);
		   	return -1;
		}
	}

	index = mq->r;
	mq->r += 1;

	signalReadMutQueue(mq);

	unlockReadMutQueue(mq);

	return index;
}

void lockWriteMutQueue(mut_queue *mq) {
	pthread_mutex_lock(&mq->wlock);
}

void unlockWriteMutQueue(mut_queue *mq) {
	pthread_mutex_unlock(&mq->wlock);
}

int writeMutQueue(mut_queue *mq, int val) {
	int index = -1;

	lockWriteMutQueue(mq);
	lockReadMutQueue(mq);

	if ( mq->w < mq->len ) {
		index = mq->w;
		mq->arr[index] = val;
		mq->w += 1;
	}


	signalReadMutQueue(mq);

	unlockReadMutQueue(mq);
	unlockWriteMutQueue(mq);

	return index;
}

void unsafePrintMutQueue(mut_queue *mq) {
	int lastr = mq->r;
	int lastw = mq->w;

	printf("Len=%d, r=%d, w=%d\n", mq->len, lastr, lastw);
	for (int i = 0; i < lastw; i++) {
		printf("%d%c", i, i+1<lastw ? ' ':'\n');
	}
}

void safePrintMutQueue(mut_queue *mq) {
	lockReadMutQueue(mq);
	int lastr = mq->r;
	unlockReadMutQueue(mq);
	lockWriteMutQueue(mq);
	int lastw = mq->w;
	unlockWriteMutQueue(mq);

	printf("Len=%d, r=%d, w=%d\n", mq->len, lastr, lastw);
	for (int i = 0; i < lastw; i++) {
		printf("%d%c", i, i+1<lastw ? ' ':'\n');
	}
}
