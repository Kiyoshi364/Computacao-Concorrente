void* task(void *arg) {
	int id = *( (int *) arg );

	// Passo 1
	while ( 1 ) {

		// Passo 1.a
		DEBUG printf("Worker %d: Quer ler\n", id);
		int	index = readBlockingMutQueue(jobPool);
		DEBUG printf("Worker %d: Leu\n", id);
		int node = jobPool->arr[index];

		if ( index < 0 || node < 0 ) {
			break;
		}

		// Passo 1.b.i
		DEBUG printf("Worker %d: Running node %d\n", id, node+1);
		DOTASK doTask(node, times[node]);
		DEBUG printf("Worker %d: Finished node %d\n", id, node+1);

		// Passo 1.b.ii
		retCodes[node] = times[node];

		// Passo 1.b.iii
		DEBUG printf("Worker %d: Quer escrever\n", id);
		if ( writeMutQueue(done, node) < 0 ) {
			printf("Erro ao marcar tarefa %d como terminada."
				"Encerrando thread %d\n.", node, id); break;
		}
		DEBUG printf("Worker %d: Escreveu\n", id);
	}

	DEBUG printf("Worker %d: Left\n", id);

	return NULL;
}

int* topoSortConc(grafo *g, int nthreads) {
	int graus[g->V];

	pthread_t tids[nthreads];
	int ids[nthreads];

	// Passo 1
	jobPool = newMutQueue(g->V);

	retCodes = malloc(sizeof(*retCodes)*(g->V));
	if ( !retCodes ) {
		puts("topoSort: Out of Memory (retCodes)"); exit(1);
	}
	done = newMutQueue(g->V);


	// Passo 2
	for (int i = 0; i < g->V; i++) {
		graus[i] = 0;
	}

	// Calcular graus
	for (int i = 0; i < g->V; i++) {
		// Pega lista de adjacencias
		clist *adj = (g->adjs)[i];

		// Pega o primeiro adjacente
		clist *now = adj->next;

		// Para cada Vertice Adjacente (now)
		while ( now != adj ) {
			graus[ now->val ] += 1;
			now = now->next;
		}
	}

	// Passo 3
	// Fila e Fila Ordenados são o mesmo vetor

	// Passo 4
	// Adicionar vertices iniciais
	for (int i = 0; i < g->V; i++) {
		if ( graus[i] == 0 ) {
			int r = writeMutQueue(jobPool, i);
			if ( r < 0 ) {
				printf("Passo 4: writeMutQueue(jobPool, i) retornou %d\n", r);
			}
		}
	}

	// Passo 5
	// Criar Threads
	for (int i = 0; i < nthreads; i++) {
		ids[i] = i+1;
		if ( pthread_create(&tids[i], NULL, task, (void *) (ids+i)) ) {
			printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}


	// Passo 6
	// Remover vertices e Atualizar Fila
	while ( done->r < jobPool->w ) {

		// Passo 6.a, Passo 6.b e Passo 6.c
		int index = readBlockingMutQueue(done);
		if ( index < 0 ) {
			printf("Passo 6.a: readBlokingMutQueue(done) retornou %d\n", index);
			break;
		}

		int node = done->arr[index];

		// Passo 6.d
		// Pega lista de adjacencias
		clist *adj = (g->adjs)[ node ];

		// Pega o primeiro adjacente
		clist *now = adj->next;

		// Para cada Vertice Adjacente (now)
		while ( now != adj ) {
			int val = now->val;

			// Passo 6.d.i
			graus[val] -= 1;

			// Passo 6.d.ii
			if ( graus[val] == 0 ) {
				int r = writeMutQueue(jobPool, val);
				if ( r < 0 ) {
					printf("Passo 6.d.ii: writeMutQueue(jobPool, val) retornou %d\n", r);
				}
			}

			now = now->next;
		}

		DEBUG {
			printf("End of Loop:\n");
			printf("jobPool: ");
			for (int i = 0; i < g->V; i++) {
				if ( i == jobPool->r || i == jobPool->w ) printf("| ");
				printf("%d ", 1+jobPool->arr[i]);
			}
			printf("\ndone: ");
			for (int i = 0; i < g->V; i++) {
				if ( i == done->r || i == done->w ) printf("| ");
				printf("%d ", 1+done->arr[i]);
			}
			printf("\ngraus: ");
			for (int i = 0; i < g->V; i++) {
				printf("%d ", graus[i]);
			}
			puts("\n");
		}
	}

	while ( writeMutQueue(jobPool, -1) > 0 ) ;
	broadcastReadMutQueue(jobPool);

	DEBUG puts("Main finished!");

	// Checar Ciclos
	if ( done->r < g->V ) {
		printf("TopoSort: Queue is Empty Cicle Found\n");
		printGrafo(g);
		printf("jobPool: ");
		for (int i = 0; i < g->V; i++) {
			if ( i == jobPool->r || i == jobPool->w ) printf("| ");
			printf("%d ", 1+jobPool->arr[i]);
		}
		printf("\ndone: ");
		for (int i = 0; i < g->V; i++) {
			if ( i == done->r || i == done->w ) printf("| ");
			printf("%d ", 1+done->arr[i]);
		}
		printf("\ngraus: ");
		for (int i = 0; i < g->V; i++) {
			printf("%d ", graus[i]);
		}
		puts("\n");
		exit(0);
	}

	// Limpar as threads
	for (int i = 0; i < nthreads; i++) {
		DEBUG printf("Limpado Thread %d\n", i+1);

		if (pthread_join(tids[i], NULL)) {
			printf("--ERRO: pthread_join()\n"); exit(-1);
		}
	}

	return done->arr;
}
