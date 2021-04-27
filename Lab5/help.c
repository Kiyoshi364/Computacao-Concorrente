void instructions(int step) {
    puts("instructions:");
    switch (step) {
        case 0:
        printf("\t- escreva no arquivo `%s' o tamanho da entrada e os valores\n",
            IN_FILE);
        case 1:
        printf("\t- escreva no arquivo `%s' o tamanho da saída e os valores corretos\n",
            CORRECT_FILE);
        default:
        puts("\nOpcionalmente descomente a linha `#define USE_STD_OUT' para que a saída esteja no stdout");
    }
    exit(0);
}

void welcome() {
    FILE *f = fopen(IN_FILE, "r");
    if ( !f ) instructions(0);
    else fclose(f);

    f = fopen(CORRECT_FILE, "r");
    if ( !f ) instructions(1);
    else fclose(f);
}

FILE* openReadFile(char *filename) {
    FILE *f = fopen(filename, "r");
    if ( !f ) {
        fprintf(stderr, "Erro: fopen %s read\n", filename);
        exit(2);
    }
    return f;
}

FILE* openWriteFile(char *filename) {
    FILE *f = fopen(filename, "w");
    if ( !f ) {
        fprintf(stderr, "Erro: fopen %s write\n", filename);
        exit(2);
    }
    return f;
}

int readVec(int **vec, FILE *file) {
    fseek(file, 0, SEEK_SET);

    int len;
    fscanf(file, "%d", &len);

    int *v = malloc(sizeof(*v)*len);
    if ( !v ) {
        fprintf(stderr, "Erro: malloc readVec: %d ints\n", len);
        exit(3);
    }

    for ( int i = 0; i < len; i++ ) {
        int num;
        if ( fscanf(file, "%d", &num) != 1 ) {
            fprintf(stderr,
                "Erro: readVec arquivo mal formatado: len=%d erro no i=%d. EOF=%d\n",
                len, i, feof(file));
            exit(4);
        }

        v[i] = num;
    }

    if ( !feof(file) ) {
        int now = ftell(file);
        fseek(file, 0, SEEK_END);
        int end = ftell(file);

        if ( end - now > 1 ) {
            fprintf(stderr,
                "Warning: arquivo não foi lido até o fim faltam %d bytes\n",
                end - now);
        }
    }

    *vec = v;
    return len;
}

void writeVec(int *v, int len, FILE *file) {
    fseek(file, 0, SEEK_SET);

    if ( fprintf(file, "%d\n", len) < 0 ) {
        fprintf(stderr,
            "Erro: writeVec não foi possível escrever: len=%d\n", len);
        exit(4);
    }

    for ( int i = 0; i < len; i++ ) {

        if ( fprintf(file, "%d ", v[i]) < 0 ) {
            fprintf(stderr,
                "Erro: writeVec não foi possível escrever: len=%d, i=%d\n",
                len, i);
            exit(4);
        }
    }
}

int check(int *v, int len, FILE *file) {
    int *right;
    int len_right = readVec(&right, file);

    int ok = 1;

    if ( len != len_right ) {
        fprintf(stderr,
            "Erro: check tamanhos diferentes: len=%d, len_right=%d\n",
            len, len_right);
        exit(5);
    }

    for ( int i = 0; i < len; i++ ) {
        if ( right[i] != v[i] ) {
            if ( ok ) {
                ok = 0;
                puts("Está errado!!!\nindex:\tesperado\tencontrado\n");
            }
            printf("%d:\t%d\t%d\n", i, right[i], v[i]);
        }
    }

    free(right);

    return ok;
}
