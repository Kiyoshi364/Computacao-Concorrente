typedef struct {
	int *arr;
	int len;

	int i;
	pthread_mutex_t lock;
} mut_arr;

mut_arr* newMutArr(int len) {
	mut_arr *marr = malloc(sizeof(*marr));
	if ( !marr ) {
		puts("newMutArr: Out of Memory (mut_arr).");
		exit(1);
	}

	len = len > 0 ? len : 1;

	marr->len = len;
	marr->arr = malloc(sizeof(*marr->arr)*len);
	if ( !(marr->arr) ) {
		puts("newMutArr: Out of Memory (mut_arr->arr).");
		exit(1);
	}

	marr->i = 0;
	pthread_mutex_init(&marr->lock, NULL);

	return marr;
}

void freeMutArr(mut_arr *marr) {
	free(marr->arr);
	pthread_mutex_destroy(&marr->lock);
	free(marr);
}

void lockMutArr(mut_arr *marr) {
	pthread_mutex_lock(&marr->lock);
}

void unlockMutArr(mut_arr *marr) {
	pthread_mutex_unlock(&marr->lock);
}

int incLimitMutArr(mut_arr *marr, int limit) {
	int index = -1;

	limit = 0 < limit && limit < marr->len? limit : marr->len;

	lockMutArr(marr);

	if ( marr->i < limit ) {
		index = marr->i;
		marr->i += 1;
	}

	unlockMutArr(marr);

	return index;
}

int incMutArr(mut_arr *marr) {
	return incLimitMutArr(marr, marr->len);
}

int writeMutArr(mut_arr *marr, int val) {
	int index = -1;

	lockMutArr(marr);

	if ( marr->i < marr->len ) {
		index = marr->i;
		marr->arr[index] = val;
		marr->i += 1;
	}

	unlockMutArr(marr);
	return index;
}

int readLimitMutArr(mut_arr *marr, int limit, int *read) {
	int index = incLimitMutArr(marr, limit);
	*read = marr->arr[index];
	return index;
}

void unsafePrintMutArr(mut_arr *marr) {
	int last = marr->i;

	printf("Len=%d, i=%d\n", marr->len, last);
	for (int i = 0; i < last; i++) {
		printf("%d%c", i, i+1<last ? ' ':'\n');
	}
}

void safePrintMutArr(mut_arr *marr) {
	lockMutArr(marr);
	int last = marr->i;
	unlockMutArr(marr);

	printf("Len=%d, i=%d\n", marr->len, last);
	for (int i = 0; i < last; i++) {
		printf("%d%c", i, i+1<last ? ' ':'\n');
	}
}
