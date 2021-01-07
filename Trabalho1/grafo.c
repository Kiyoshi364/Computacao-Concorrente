typedef struct _clist {
	int val;
	struct _clist *next;
} clist;

clist* newClist() {
	clist *l = malloc(sizeof(*l));
	if ( !l ) {
		puts("newClist: Out of Memory.");
		exit(1);
	}
	l->next = l;
	return l;
}

void freeClist(clist *l) {
	clist *now = l->next;
	clist *prev = l;

	while ( now != l ) {
		prev = now;
		now = now->next;
		free(prev);
	}

	free(l);
}

clist* findClist(clist *l, int val) {
	clist *now = l->next;
	clist *prev = l;

	while ( now != l && now->val < val ) {
		prev = now;
		now = now->next;
	}

	return prev;
}

void pushInClist(clist *l, int val) {
	clist *prev = findClist(l, val);

	clist *new = newClist();
	new->val = val;
	new->next = prev->next;

	prev->next = new;
}

void printClist(clist *l) {
	clist *now = l->next;

	if ( now == l) {
		putchar('\n');
	}

	while ( now != l ) {
		printf("%d%s", now->val+1,
				now->next==l ? "\n" : ", ");
		now = now->next;
	}
}

typedef struct _grafo {
	int V, A;
	clist **adjs;
} grafo;

grafo* newGrafo(const int vertices) {
	grafo *g = malloc(sizeof(*g));
	if ( !g ) {
		puts("newGrafo: Out of Memory (grafo).");
		exit(1);
	}

	g->V = vertices;
	g->A = 0;
	g->adjs = malloc(sizeof(*(g->adjs))*vertices);
	if ( !(g->adjs) ) {
		puts("newGrafo: Out of Memory (g->adjs).");
		exit(1);
	}

	for (int i = 0; i < vertices; i++) {
		g->adjs[i] = newClist();
	}

	return g;
}

void freeGrafo(grafo *g) {
	for (int i = 0; i < g->V; i++) {
		freeClist(g->adjs[i]);
	}

	free(g);
}

void addAresta(grafo *g, int de, int para) {
	pushInClist(g->adjs[de], para);
	g->A += 1;
}

void printGrafo(grafo *g) {
	printf("V=%d, A=%d\n", g->V, g->A);
	for (int i = 0; i < g->V; i++) {
		printf("%d: ", i+1);
		printClist(g->adjs[i]);
	}
	putchar('\n');
}
