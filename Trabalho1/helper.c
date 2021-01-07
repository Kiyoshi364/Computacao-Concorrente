int readToBuf(char *buf) {
	for (int i = 0; i < TAMBUFFER; ) {
		int c = getchar();

		if ( IS_SPACE(c) ) {
			buf[i] = '\0';
			return c;
		} else if ( IS_NUM(c) ) {
			buf[i] = c;
			i++;
		} else {
			// printf("Not Found: %c\n", c);
		}
	}
	return -1;
}

grafo* lerEntrada(int *V, int *A, uint **times) {
	char buf[TAMBUFFER], justNewLine = 1;
	int c;

	c = readToBuf(buf);
	*V = atoll(buf);

	c = readToBuf(buf);
	*A = atoll(buf);

	grafo *graf = newGrafo(*V);
	*times = malloc(sizeof(**times)*(*V));
	if ( !(*times) ) {
		puts("lerEntrada: Out of Memory (times).");
		exit(1);
	}

	for (int i = 0, node = 0; i < *A || node < *V; i++) {
		int other;
		c = readToBuf(buf);

		other = atoll(buf);

		if ( justNewLine ) {
			(*times)[node] = other;
		} else if ( other > 0 ) {
			addAresta(graf, node, other-1);
		}

		justNewLine = 0;
		if ( c == '\n' ) {
			node += 1;
			justNewLine = 1;
		}
	}
	return graf;
}

void doTask(int node, int t) {
	sleep(t);
}
