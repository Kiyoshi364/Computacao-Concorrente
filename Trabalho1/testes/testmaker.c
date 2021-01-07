#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../grafo.c"

int randomBool() {
	int r = rand();

	return r <= RAND_MAX / 2;
}

int randomValidNode(grafo *g, int node) {
	int rnode, valid = 0;

	while ( !valid ) {

		valid = 1;
		rnode = rand() % node;

		clist *adj = (g->adjs)[ node ];
		clist *now = adj->next;

		while ( now != adj ) {

			if ( now->val == rnode ) {
				valid = 0;
				break;
			}

			now = now->next;
		}
	}

	return rnode;
}

grafo* randomGrafo(int V, int A) {
	grafo *g = newGrafo(V);

	int v = 1, a = 0;

	for (; v < V && a < A; v++) {

		int linksPerNode = (A - a) / (V - v),
			rem = (A - a) % (V - v);

		int i = 0;
		for (; i < linksPerNode && i < v; i++) {
			int rnode = randomValidNode(g, v);

			addAresta(g, v, rnode);

			a += 1;
		}

		if ( i < v && rem != 0 && randomBool() ) {
			int rnode = randomValidNode(g, v);

			addAresta(g, v, rnode);

			a += 1;
		}
	}

	return g;
}

void printGrafoFormatted(grafo *g, int time) {
	int i = 0, j = 0;

	printf("%d %d\n", g->V, g->A);
	for (; i < g->V && j < g->A; i++) {
		printf("%d", time);

		clist *adj = (g->adjs)[i];
		clist *now = adj->next;

		while ( now != adj ) {

			printf(" %d", now->val);
			j += 1;

			now = now->next;
		}

		putchar('\n');
	}

	if ( i != g->V || j != g->A ) {
		printf("PANIC: i = %d, j = %d\n", i, j);
	}
}

int main() {
	int V = 100, A = V*3;
	int seed = time(NULL);
	int time = 0;

	srand(seed);

	grafo *g = randomGrafo(V, A);
	printGrafoFormatted(g, time);
	freeGrafo(g);

	return 0;
}
