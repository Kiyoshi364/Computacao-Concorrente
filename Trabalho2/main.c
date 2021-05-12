#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "lock.h"

#define INT_GRANDE 35791394

#define DEBUG       if (1)

typedef struct {
    int temperatura;
    int id;
    int timestamp;
} temperatura_t;

temperatura_t temperaturas[60];
int pos = 0;
lock_t *lock;

int medirTemp() {
    return (rand() % 16) + 25;
}

typedef struct {
    int id;
} arg_t;

void* sensor(void *a) {
    arg_t *args = (arg_t *) a;
    int id = args->id;

    int timestamp = 0;
    while (1) {
        int temperatura = medirTemp();

        if ( temperatura > 30 ) {
            lock_escrita(lock, id);

            int index = pos % 60;

            temperaturas[index].temperatura = temperatura;
            temperaturas[index].id = id;
            temperaturas[index].timestamp = timestamp;

            DEBUG
            printf("--- %d [%d:%d]: %d \n", pos, id, timestamp, temperatura);

            // Isso eh para incrementar e evitar overflow
            // quando o pos fica maior que um numero grandao multiplo de 60
            // a gente "reseta" o pos para 61
            // nota: nao pode ser 1 porque pos > 60 fala que o buffer inteiro
            // eh valido
            //
            // pos += 1 - (pos > 60*INT_GRANDE) * (INT_GRANDE - 1) * 60;
            //
            // nota: int é tão grande que talvez isso não valha a pena
            pos += 1;

            unlock_escrita(lock, id);

            timestamp += 1;
        }
        sleep(1);
    }
}

void* atuador(void *a) {
    arg_t *args = (arg_t *) a;
    int id = args->id;

    int somaTemperatura = 0;
    int lastTimestamp = -1;

    int temps[15];

    while (1) {
        int newTimestamp = lastTimestamp;

        lock_leitura(lock, id);
        for ( int i = 0; i < 60; i += 1 ) {
            if ( i >= pos )
                break;

            temperatura_t temp = temperaturas[i];
            if ( temp.id != id )
                continue;

            int stamp = temp.timestamp;
            if ( stamp > lastTimestamp ) {
                somaTemperatura += temp.temperatura;
                temps[stamp % 15] = temp.temperatura;

                newTimestamp = stamp > newTimestamp ? stamp : newTimestamp;
            }
        }
        unlock_leitura(lock, id);

        // Analizar as últimas 15 temps
        int vermelho = 5;
        int amarelo = 5;
        for ( int i = 0; i < 15; i += 1 ) {
            int perigo = temps[i] > 35;

            if ( i > newTimestamp ) {
                break;
            }

            int inicio = (newTimestamp - 5) % 15;
            int fim = newTimestamp % 15;
            // subtrair do vermelho
            if (  ( inicio < fim && ( inicio < i && i <= fim ) ) ||
                  ( fim < inicio && ( inicio < i || i <= fim ) ) ) {
                vermelho -= perigo;
            }
            amarelo -= perigo;
        }

        // Média
        double media = ((double) somaTemperatura)
                        / ((double) (newTimestamp + 1));

        // Alertas
        if ( vermelho <= 0 ) {
            printf("-+-> %d: Alerta vermelho - Media(%d) = %lf\n",
                    id, newTimestamp+1, media);
        } else if ( amarelo <= 0 ) {
            printf("-+-> %d: Alerta amarelo - Media(%d) = %lf\n",
                    id, newTimestamp+1, media);
        } else {
            printf("-+-> %d: Condicao normal - Media(%d) = %lf\n",
                    id, newTimestamp+1, media);
        }

        DEBUG {
            printf(" L-> ");
            for ( int i = 0; i < 15; i += 1 ) {
                printf("%d ", temps[i]);
            }
            putchar(10);
        }

        lastTimestamp = newTimestamp;
        sleep(2);
    }
}

int main(int argc, char **argv) {
    if ( argc < 2 ) {
        printf("usage: %s <nthreads>\n", argv[0]);
        return 0;
    }

    int nthreads = atol(argv[1]);

    // Inicializar um monte de coisa
    pthread_t *tsen;
    pthread_t *tatu;
    arg_t *args;

    tsen = malloc(sizeof(*tsen)*nthreads);
    if ( !tsen ) {
        fprintf(stderr, "Erro malloc: tsen\n");
        return 1;
    }

    tatu = malloc(sizeof(*tatu)*nthreads);
    if ( !tatu ) {
        fprintf(stderr, "Erro malloc: tatu\n");
        return 1;
    }

    args = malloc(sizeof(*args)*nthreads);
    if ( !args ) {
        fprintf(stderr, "Erro malloc: args\n");
        return 1;
    }

    lock_t LOCK;
    init_lock(&LOCK);
    lock = &LOCK;

    // Chamar as threads
    for ( int i = 0; i < nthreads; i += 1 ) {
        args[i].id = i;

        if ( pthread_create(&tsen[i], NULL, sensor, &args[i]) ) {
            fprintf(stderr, "Erro pthread-create sensor: i = %d\n", i);
        }
        if ( pthread_create(&tatu[i], NULL, atuador, &args[i]) ) {
            fprintf(stderr, "Erro pthread-create atuador: i = %d\n", i);
        }
    }
    
    while (1) ;

    return 0;
}
