void* task(void *arg) {
	int id = *( (int *) arg );

	// Passo 1
	while ( done->i < jobPool->len ) {
		int jcnt = jobCount;
		int	node;

		// Passo 1.a
		if ( readLimitMutArr(jobPool, jcnt, &node) < 0 ) {
			continue;
		}

		// Passo 1.b.i
		DEBUG printf("Worker %d: Running node %d\n", id, node+1);
		DOTASK doTask(node, times[node]);
		DEBUG printf("Worker %d: Finished node %d\n", id, node+1);

		// Passo 1.b.ii
		retCodes[node] = times[node];

		// Passo 1.b.iii
		if ( writeMutArr(done, node) < 0 ) {
			printf("Erro ao marcar tarefa %d como terminada."
				"Encerrando thread %d\n.", node, id); break;
		}
	}

	return NULL;
}

int* topoSortConc(grafo *g, int nthreads) {
	int graus[g->V];
	int end = 0;

	pthread_t tids[nthreads];
	int ids[nthreads];

	int *queue = malloc(sizeof(*queue)*(g->V));
	if ( !queue ) {
		puts("topoSort: Out of Memory (queue)"); exit(1);
	}

	// Passo 1
	jobPool = newMutArr(g->V);
	jobCount = 0;

	retCodes = malloc(sizeof(*retCodes)*(g->V));
	if ( !retCodes ) {
		puts("topoSort: Out of Memory (retCodes)"); exit(1);
	}
	done = newMutArr(g->V);


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
			jobPool->arr[jobCount] = i;
			jobCount += 1;
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
	while ( end < jobCount ) {

		// Passo 6.a e Passo 6.b
		while ( end >= done->i ) { ; }

		// Passo 6.c
		int node = done->arr[end];

		queue[end] = node;
		end += 1;

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
				jobPool->arr[jobCount] = val;
				jobCount += 1;
			}

			now = now->next;
		}

		DEBUG {
			printf("End of Loop: end=%d, jobCount=%d, done=%d\n", end, jobCount, done->i);
			printf("jobPool: ");
			for (int i = 0; i < g->V; i++) {
				if ( i == jobCount ) printf("| ");
				printf("%d ", 1+jobPool->arr[i]);
			}
			printf("\ndone: ");
			for (int i = 0; i < g->V; i++) {
				if ( i == done->i ) printf("| ");
				printf("%d ", 1+done->arr[i]);
			}
			printf("\ngraus: ");
			for (int i = 0; i < g->V; i++) {
				printf("%d ", graus[i]);
			}
			puts("\n");
		}
	}

	// Checar Ciclos
	if ( end < g->V ) {
		printf("TopoSort: Queue is Empty Cicle Found\n");
		printGrafo(g);
		exit(0);
	}

	// Limpar as threads
	for (int i = 0; i < nthreads; i++) {
		if (pthread_join(tids[i], NULL)) {
			printf("--ERRO: pthread_join()\n"); exit(-1);
		}
	}

	return queue;
}
