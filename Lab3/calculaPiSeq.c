#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "timer.h"

int main (int argc, char **argv) {
	long long int elem;
	double nearPi = 0;

	double tstart, tfinish, tinit, telapsed;

	GET_TIME(tstart);

	// Leitura de argumentos
	if (argc < 2) {
		printf("usage: %s <n elementos>\n", argv[0]);
		return 0;
	} else {
		elem = atoll(argv[1]);
	}

	GET_TIME(tfinish);
	tinit = tfinish - tstart;

	GET_TIME(tstart);

	// Calcular PI
	for (int i = elem; i > 0;  i--) {
		int sinal = (i&1)*2-1;
		double div = (double) (i*2 - 1) * sinal;

		nearPi += 4/div;
	}

	GET_TIME(tfinish);
	telapsed = tfinish - tstart;

	GET_TIME(tstart);
	GET_TIME(tfinish);
	tclean = tfinish - tstart;

	printf("Nosso PI:\t%.15lf\n", nearPi);
	printf("Maths PI:\t%.15lf\n", M_PI);

	printf("\n\ntime init:\t %lf \n", tinit);
	printf("time elapsed:\t %lf \n", telapsed);
	printf("time clean up:\t %lf \n", tclean);
	printf("time total:\t %lf \n", tinit + telapsed + tclean);

	return 0;
}
