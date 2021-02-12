#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randNum( int max ) {
    return rand() % max;
}

void initVec(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        v[i] = i;
    }

    for (int i = tam-1; i > 0; i--) {
        int j = randNum( i );

        int temp = v[j];
        v[j] = v[i];
        v[i] = temp;
    }
}

int main(int argc, char **argv) {
    int tamVec = 10;
    int seed = time(NULL);

	switch (argc) {
		default:
		case 3:
			seed = atoll(argv[2]);
		case 2:
            tamVec = atoll(argv[1]);
		case 1:
			break;
	}

	srand(seed);

    int v[tamVec];

    initVec(v, tamVec);

    printf("%d\n", tamVec);
    for (int i = 0; i < tamVec-1; i++) {
        printf("%d ", v[i]);
    }
    printf("%d\n", v[tamVec-1]);

    return 0;
}
