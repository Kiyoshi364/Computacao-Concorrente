#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

typedef unsigned int uint;

#define TAMBUFFER 100

#define IS_SPACE(x)	( (x) == ' ' || (x) == '\n' )
#define IS_NUM(x)	( '0' <= (x) && (x) <= '9' )

#define DEBUG if (0)
#define DOTASK if (0)

#include "grafo.c"
#include "helper.c"

int* topoSort(grafo *g, uint *times) {
	int graus[g->V];
	int end = 0;

	int *queue = malloc(sizeof(*queue)*(g->V));
	if ( !queue ) {
		puts("topoSort: Out of Memory"); exit(1);
	}

	// Passo 1
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

	// Passo 2
	// Fila e Fila Ordenados são o mesmo vetor

	// Passo 3
	// Adicionar vertices iniciais
	for (int i = 0; i < g->V; i++) {
		if ( graus[i] == 0 ) {
			queue[end] = i;
			end++;
		}
	}

	// Passo 4
	// Remover vertices e Atualizar Fila
	for (int i = 0; i < end; i++) {

		// Passo 4.a
		int node = queue[i];

		// Passo 4.b
		// Já está na Fila Ordenados

		// 4.c
		DEBUG printf("Running node %d\n", node+1);
		DOTASK doTask( node, times[node] );
		DEBUG printf("Finished node %d\n", node+1);

		// Não precisa fazer Passo 4.b, pois a fila não esvazia

		// Pega lista de adjacencias
		clist *adj = (g->adjs)[ node ];

		// Pega o primeiro adjacente
		clist *now = adj->next;

		// Passo 4.c
		// Para cada Vertice Adjacente (now)
		while ( now != adj ) {
			int val = now->val;

			// Passo 4.d.i
			graus[val] -= 1;

			// Passo 4.d.ii
			if ( graus[val] == 0 ) {
				queue[end] = val;
				end++;
			}

			now = now->next;
		}
	}

	// Checar Ciclos
	if ( end < g->V ) {
		printf("TopoSort: Queue is Empty Cicle Found\n");
		printGrafo(g);
		exit(0);
	}

	return queue;
}

int main(int argc, char **argv) {
	int V, A;
	uint *times;

	// Ler entrada
	grafo *graf = lerEntrada(&V, &A, &times);

	// Organizar
	int *indexes = topoSort(graf, times);

	// Mostrar saída
	printf("Resultado Final: ");
	for (int i = 0; i < V; i++) {
		// printf("node: %d, %ds\n", indexes[i]+1, times[ indexes[i] ]);
		printf("%d%c", indexes[i]+1, i+1<V?' ':'\n');
	}

	free(indexes);

	free(times);
	freeGrafo(graf);

	return 0;
}
